#include "Renderer.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include "Sphere.hpp"
#include "Vector.hpp"
#include "global.hpp"
#include <chrono>

// In the main function of the program, we create the scene (create objects and
// lights) as well as set the options for the render (image width and height,
// maximum recursion depth, field-of-view, etc.). We then call the render
// function().

Vector3f transform(Vector3f vert) {
    //vert = vert * 200;
    //vert = vert + Vector3f(200, 120, 200);
    vert = vert + Vector3f(0, 0, 150);
    return vert;
}

int main(int argc, char** argv)
{

    // Change the definition here to change resolution
    Scene scene(1240, 780);

    Material* red = new Material(DIFFUSE, Vector3f(0.0f));
    red->Kd = Vector3f(0.63f, 0.065f, 0.05f);
    Material* green = new Material(DIFFUSE, Vector3f(0.0f));
    green->Kd = Vector3f(0.14f, 0.45f, 0.091f);
    Material* white = new Material(DIFFUSE, Vector3f(0.0f));
    white->Kd = Vector3f(0.725f, 0.71f, 0.68f);
    Material* boxWhite = new Material(REFLECTION, Vector3f(0.0f));
    //boxWhite->Kd = Vector3f(0.725f, 0.71f, 0.68f);
    boxWhite->Kd = Vector3f(0.63f, 0.065f, 0.05f);
    Material* ginger = new Material(DIFFUSE, Vector3f(0.0f));
    ginger->Kd = Vector3f(0.70f, 0.35f, 0.01f);
    Material* fulvous = new Material(DIFFUSE, Vector3f(0.0f));
    fulvous->Kd = Vector3f(0.84f, 0.44f, 0.01f);
    Material* grey = new Material(DIFFUSE, Vector3f(0.0f));
    grey->Kd = Vector3f(0.52f, 0.52f, 0.51f);
    Material* light = new Material(DIFFUSE, (8.0f * Vector3f(0.747f+0.058f, 0.747f+0.258f, 0.747f) + 15.6f * Vector3f(0.740f+0.287f,0.740f+0.160f,0.740f) + 18.4f *Vector3f(0.737f+0.642f,0.737f+0.159f,0.737f)));
    light->Kd = Vector3f(0.65f);

    std::string texture_name = "/home/elliott/Documents/cg/PA6/PA7/Assignment7/models/test/new_texture.png";
    Material* mow_texture = new Material(DIFFUSE, Vector3f(0.0f), texture_name);
    mow_texture->Kd = Vector3f(0.725f, 0.71f, 0.68f);

    std::string wall_texture_name = "/home/elliott/Documents/cg/PA6/PA7/Assignment7/models/model/Brick.png";
    Material* sandbrown = new Material(DIFFUSE, Vector3f(0.0f), wall_texture_name);
    sandbrown->Kd = Vector3f(1.0f, 0.7f, 0.4f);

    //MeshTriangle floor("../models/cornellbox/floor.obj", white);
    //MeshTriangle shortbox("../models/cornellbox/shortbox.obj", white);
    //MeshTriangle tallbox("../models/cornellbox/tallbox.obj", boxWhite);
    //MeshTriangle left("../models/cornellbox/left.obj", red);
    //MeshTriangle right("../models/cornellbox/right.obj", green);

    MeshTriangle wall("../models/model/wall3.obj",sandbrown, transform);
    MeshTriangle rockery1("../models/model/rockery1.obj",ginger, transform);
    MeshTriangle rockery2("../models/model/rockery2.obj",fulvous, transform);
    MeshTriangle boxes("../models/model/boxes.obj",boxWhite, transform);
    MeshTriangle sjtu("../models/model/sjtu.obj",grey, transform);
    MeshTriangle light_("../models/model/light.obj", light);
    //MeshTriangle mow( "/home/elliott/Documents/cg/PA6/PA7/Assignment7/models/test/spot_triangulated_good.obj", mow_texture, transform);

    //Sphere sph1(Vector3f(200, 80, 150), 60, boxWhite);
    //Sphere sph2(Vector3f(150, 100, 50), 20, red);

    //scene.Add(&floor);
    //scene.Add(&mow);
    //scene.Add(&sph1);
    //scene.Add(&sph2);
    //scene.Add(&shortbox);
    //scene.Add(&tallbox);
    //scene.Add(&left);
    //scene.Add(&right);

    scene.Add(&wall);
    scene.Add(&rockery1);
    scene.Add(&rockery2);
    scene.Add(&boxes);
    scene.Add(&sjtu);
    scene.Add(&light_);
    //scene.Add(std::make_unique<Light>(Vector3f(-20, 70, 20), 1));
    //scene.Add(std::make_unique<Light>(Vector3f(20, 70, 20), 1));
    //scene.Add(std::make_unique<Light>(Vector3f(-20, -40, 20), 1));

    scene.buildBVH();

    Renderer r;

    auto start = std::chrono::system_clock::now();
    r.Render(scene);
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";

    return 0;
}