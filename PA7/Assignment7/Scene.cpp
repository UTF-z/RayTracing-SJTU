//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"

void Scene::buildBVH()
{
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        if (objects[k]->hasEmit())
        {
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        if (objects[k]->hasEmit())
        {
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum)
            {
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
    const Ray &ray,
    const std::vector<Object *> &objects,
    float &tNear, uint32_t &index, Object **hitObject) const
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear)
        {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }

    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    if (depth == 7)
        return Vector3f();
    Intersection isect = intersect(ray);
    Vector3f normal = isect.normal;
    Vector3f hitPoint = isect.coords;
    if (!isect.happened)
        return Vector3f();
    switch (isect.m->m_type)
    {
    case MaterialType::REFLECTION_AND_REFRACTION:
    {
        Vector3f reflectionDirection = normalize(reflect(ray.direction, normal));
        Vector3f refractionDirection = normalize(refract(ray.direction, normal, isect.m->ior));
        Vector3f reflectionRayOrig = (dotProduct(reflectionDirection, normal) < 0) ? hitPoint - normal * EPSILON : hitPoint + normal * EPSILON;
        Vector3f refractionRayOrig = (dotProduct(refractionDirection, normal) < 0) ? hitPoint - normal * EPSILON : hitPoint + normal * EPSILON;
        Vector3f reflectionColor = castRay(Ray(reflectionRayOrig, reflectionDirection), depth + 1);
        Vector3f refractionColor = castRay(Ray(refractionRayOrig, refractionDirection), depth + 1);
        float kr;
        fresnel(ray.direction, normal, isect.m->ior, kr);
        return reflectionColor * kr + refractionColor * (1 - kr);
        break;
    }
    case MaterialType::REFLECTION:
    {
        float kr;
        fresnel(ray.direction, normal, isect.m->ior, kr);
        Vector3f reflectionDirection = reflect(ray.direction, normal);
        Vector3f reflectionRayOrig = (dotProduct(reflectionDirection, normal) < 0) ? hitPoint + normal * EPSILON : hitPoint - normal * EPSILON;
        return castRay(Ray(reflectionRayOrig, reflectionDirection), depth + 1) * kr;
        break;
    }
    case MaterialType::DIFFUSE:
    {
        if (isect.m->hasEmission())
        {
            if (depth == 0)
            {
                Vector3f emit = isect.m->getEmission();
                return emit;
            }
            return Vector3f();
        }
        Intersection lightP;
        float lightPdf = 1.0;

        sampleLight(lightP, lightPdf);
        Vector3f To_light = lightP.coords - isect.coords;
        float dist2 = dotProduct(To_light, To_light);
        Vector3f lightDir = normalize(To_light);
        Ray lightRay(isect.coords, lightDir);
        Vector3f lightComp;
        Intersection block = intersect(lightRay);
        if ((block.coords - lightP.coords).norm() < EPSILON * 100)
        {
            Vector3f emit = lightP.emit;
            Vector3f NN = lightP.normal;
            Vector3f brdf = isect.m->eval(lightRay.direction, -ray.direction, normal);
            float cosine1 = dotProduct(normal, lightRay.direction);
            float cosine2 = dotProduct(NN, -lightRay.direction);
            lightComp = emit * brdf * cosine1 * cosine2 / (dist2 * lightPdf);
        }
        float rr_test = get_random_float();
        Vector3f objComp;
        if (depth < 3) {
            Vector3f wi = isect.m->sample(ray.direction, normal).normalized();
            Ray newRay(isect.coords, wi);
            float cosine = dotProduct(wi, normal);
            Vector3f brdf = isect.m->eval(wi, -ray.direction, normal);
            float pdf = isect.m->pdf(wi, -ray.direction, normal);
            objComp = castRay(newRay, depth + 1) * brdf * cosine / (pdf);
        }
        else if (rr_test < RussianRoulette)
        {
            Vector3f wi = isect.m->sample(ray.direction, normal).normalized();
            Ray newRay(isect.coords, wi);
            float cosine = dotProduct(wi, normal);
            Vector3f brdf = isect.m->eval(wi, -ray.direction, normal);
            float pdf = isect.m->pdf(wi, -ray.direction, normal);
            objComp = castRay(newRay, depth + 1) * brdf * cosine / (pdf * RussianRoulette);
        }
        return lightComp + objComp;
    }
    default:
        return Vector3f();
    }
    return Vector3f();
}