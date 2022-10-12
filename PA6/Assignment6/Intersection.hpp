#ifndef RAYTRACING_INTERSECTION_H
#define RAYTRACING_INTERSECTION_H
#include "Vector.hpp"
#include "Material.hpp"
class Object;
class Sphere;

struct Intersection
{
    Intersection(){
        happened=false; // whether intersect
        coords=Vector3f();  // intersection point
        normal=Vector3f();  // normal of the facet at the intersection point
        distance= std::numeric_limits<double>::max();   // between intersection point and ray.origin
        obj =nullptr;   // points to the intersection point
        m=nullptr;  // obj material
    }
    bool happened;
    Vector3f coords;
    Vector3f normal;
    double distance;
    Object* obj;
    Material* m;
};
#endif //RAYTRACING_INTERSECTION_H
