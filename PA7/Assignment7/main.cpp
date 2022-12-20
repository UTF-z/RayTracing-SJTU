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


Vector3f rotate(Vector3f vert, float ax, float ay, float az) {
    ax = ax / 180.0 * M_PI;
    ay = ay / 180.0 * M_PI;
    az = az / 180.0 * M_PI;
    float x = vert.x, y = vert.y, z = vert.z;
    Vector3f nv = Vector3f(vert.x, vert.y * cos(ax) - vert.z * sin(ax), vert.y * sin(ax) + vert.z * cos(ax));
    nv = Vector3f(nv.x * cos(ay) + nv.z * sin(ay), nv.y, -nv.x * sin(ay) + nv.z * cos(ay));
    nv = Vector3f(nv.x * cos(az) - nv.y * sin(az), nv.x * sin(az) + nv.y * cos(az), nv.z);
    return nv;
}

Vector3f transform(Vector3f vert) {
    vert = rotate(vert, 0, -10, 0);
    vert = vert + Vector3f(25, 0, 250);
    return vert;
}

int main(int argc, char** argv)
{

    // Change the definition here to change resolution
    Scene scene(1240, 780);

    Material* boxWhite = new Material(REFLECTION, Vector3f(0.0f));
    //boxWhite->Kd = Vector3f(0.725f, 0.71f, 0.68f);
    boxWhite->Kd = Vector3f(0.63f, 0.065f, 0.05f);

    std::string rock1_texture_name = "/home/elliott/Documents/cg/PA6/PA7/Assignment7/models/model/rock1.png";
    Material* ginger = new Material(DIFFUSE, Vector3f(0.0f), rock1_texture_name);
    ginger->Kd = Vector3f(0.70f, 0.35f, 0.01f);

    std::string rock2_texture_name = "/home/elliott/Documents/cg/PA6/PA7/Assignment7/models/model/rock2.png";
    Material* fulvous = new Material(DIFFUSE, Vector3f(0.0f), rock2_texture_name);
    fulvous->Kd = Vector3f(0.84f, 0.44f, 0.01f);

    Material* grey = new Material(DIFFUSE, Vector3f(0.0f));
    grey->Kd = Vector3f(0.52f, 0.52f, 0.51f);

    Material* light = new Material(DIFFUSE, (8.0f * Vector3f(0.747f+0.058f, 0.747f+0.258f, 0.747f) + 15.6f * Vector3f(0.740f+0.287f,0.740f+0.160f,0.740f) + 18.4f *Vector3f(0.737f+0.642f,0.737f+0.159f,0.737f)));
    light->Kd = Vector3f(0.65f);

    std::string wall_texture_name = "/home/elliott/Documents/cg/PA6/PA7/Assignment7/models/model/Brick.png";
    Material* sandbrown = new Material(DIFFUSE, Vector3f(0.0f), wall_texture_name);
    sandbrown->Kd = Vector3f(1.0f, 0.7f, 0.4f);

    std::string negx_texture_name = "/home/elliott/Documents/cg/PA6/PA7/Assignment7/models/model/negx.jpg";
    Material* negx_texture = new Material(BACKGROUND, Vector3f(0.0f), negx_texture_name);

    std::string negy_texture_name = "/home/elliott/Documents/cg/PA6/PA7/Assignment7/models/model/negy.jpg";
    Material* negy_texture = new Material(BACKGROUND, Vector3f(0.0f), negy_texture_name);

    std::string negz_texture_name = "/home/elliott/Documents/cg/PA6/PA7/Assignment7/models/model/negz.jpg";
    Material* negz_texture = new Material(BACKGROUND, Vector3f(0.0f), negz_texture_name);

    std::string posx_texture_name = "/home/elliott/Documents/cg/PA6/PA7/Assignment7/models/model/posx.jpg";
    Material* posx_texture = new Material(BACKGROUND, Vector3f(0.0f), posx_texture_name);

    std::string posy_texture_name = "/home/elliott/Documents/cg/PA6/PA7/Assignment7/models/model/posy.jpg";
    Material* posy_texture = new Material(BACKGROUND, Vector3f(0.0f), posy_texture_name);

    std::string posz_texture_name = "/home/elliott/Documents/cg/PA6/PA7/Assignment7/models/model/posz.jpg";
    Material* posz_texture = new Material(BACKGROUND, Vector3f(0.0f), posz_texture_name);

    MeshTriangle wall("../models/model/wall.obj",sandbrown, transform);
    MeshTriangle rockery1("../models/model/rockery1.obj",ginger, transform);
    MeshTriangle rockery2("../models/model/rockery2.obj",fulvous, transform);
    MeshTriangle boxes("../models/model/boxes.obj",boxWhite, transform);
    MeshTriangle sjtu("../models/model/sjtu.obj",grey, transform);
    MeshTriangle light_("../models/model/light.obj", light);

    MeshTriangle negx("../models/model/negx.obj",negx_texture, transform);
    MeshTriangle negy("../models/model/negy.obj",negy_texture, transform);
    MeshTriangle negz("../models/model/negz.obj",negz_texture, transform);

    MeshTriangle posx("../models/model/posx.obj",posx_texture, transform);
    MeshTriangle posy("../models/model/posy.obj",posy_texture, transform);
    MeshTriangle posz("../models/model/posz.obj",posz_texture, transform);

    //Sphere sph1(Vector3f(200, 80, 150), 60, boxWhite);
    //Sphere sph2(Vector3f(150, 100, 50), 20, red);

    scene.Add(&light_);
    scene.Add(&wall);
    //scene.Add(&rockery1);
    //scene.Add(&rockery2);
    //scene.Add(&boxes);
    //scene.Add(&sjtu);
    scene.Add(&negx);
    scene.Add(&negy);
    scene.Add(&negz);
    scene.Add(&posx);
    scene.Add(&posy);
    scene.Add(&posz);

    scene.buildBVH();

    Renderer r;

    auto start = std::chrono::system_clock::now();
    r.Render(scene, 1);
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";

    return 0;
}