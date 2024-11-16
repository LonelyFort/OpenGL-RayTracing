//============================================================
// STUDENT NAME: Liu Yu-Wei
// NUS User ID.: E1122378
// COMMENTS TO GRADER:
//
// ============================================================

#include "Util.h"
#include "Vector3d.h"
#include "Color.h"
#include "Image.h"
#include "Ray.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Surface.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "Scene.h"
#include "Raytrace.h"
#include <string>


// Constants for Scene 1.
static constexpr int imageWidth1 = 640;
static constexpr int imageHeight1 = 480;
static constexpr int reflectLevels1 = 2;  // 0 -- object does not reflect scene.
static constexpr int hasShadow1 = false;
static constexpr std::string_view outImageFile1 = "out1.png";

// Constants for Scene 2.
static constexpr int imageWidth2 = 640;
static constexpr int imageHeight2 = 480;
static constexpr int reflectLevels2 = 2;  // 0 -- object does not reflect scene.
static constexpr int hasShadow2 = true;
static constexpr std::string_view outImageFile2 = "out2.png";



///////////////////////////////////////////////////////////////////////////
// Raytrace the whole image of the scene and write it to a file.
///////////////////////////////////////////////////////////////////////////

void RenderImage( const std::string &imageFilename, const Scene &scene, 
                  int reflectLevels, bool hasShadow )
{
    int imgWidth = scene.camera.getImageWidth();
    int imgHeight = scene.camera.getImageHeight();

    Image image( imgWidth, imgHeight ); // To store the result of ray tracing.

    double startTime = Util::GetCurrRealTime();
    double startCPUTime = Util::GetCurrCPUTime();

    // Generate image, rendering in parallel on Windows and Linux.
    #ifndef __APPLE__
    #pragma warning( push )
    #pragma warning( disable : 6993 )
    #pragma omp parallel for
    #endif
    for ( int y = 0; y < imgHeight; y++ )
    {
        double pixelPosY = y + 0.5;

        for ( int x = 0; x < imgWidth; x++ )
        {
            double pixelPosX = x + 0.5;
            Ray ray = scene.camera.getRay( pixelPosX, pixelPosY );
            Color pixelColor = Raytrace::TraceRay( ray, scene, reflectLevels, hasShadow );
            pixelColor.clamp();
            image.setPixel( x, y, pixelColor );
        }
    }
    #ifndef __APPLE__
    #pragma warning( pop )
    #endif

    double cpuTimeElapsed = Util::GetCurrCPUTime() - startCPUTime;
    double realTimeElapsed = Util::GetCurrRealTime() - startTime;
    std::cout << "CPU time taken = " << cpuTimeElapsed << "sec" << std::endl;
    std::cout << "Real time taken = " << realTimeElapsed << "sec" << std::endl;

    // Write image to file.
    if ( !image.writeToFile( imageFilename ) ) return;
    else Util::ErrorExit("File: %s could not be written.\n", imageFilename.c_str() );
}



// Forward declarations. These functions are defined later in the file.
void DefineScene1( Scene &scene, int imageWidth, int imageHeight );
void DefineScene2( Scene &scene, int imageWidth, int imageHeight );



int main()
{

// Define Scene 1.

    Scene scene1;
    DefineScene1( scene1, imageWidth1, imageHeight1 );

// Render Scene 1.

    std::cout << "Render Scene 1..." << std::endl;
    RenderImage( std::string(outImageFile1), scene1, reflectLevels1, hasShadow1 );
    std::cout << "Scene 1 completed." << std::endl;

// Delete Scene 1 surfaces.

    for (auto& surface : scene1.surfaces)
    {
        delete surface;
    }


// Define Scene 2.

    Scene scene2;
    DefineScene2( scene2, imageWidth2, imageHeight2 );

// Render Scene 2.

    std::cout << "Render Scene 2..." << std::endl;
    RenderImage( std::string(outImageFile2), scene2, reflectLevels2, hasShadow2 );
    std::cout << "Scene 2 completed." << std::endl;

// Delete Scene 2 surfaces.

    for (auto& surface : scene2.surfaces)
    {
        delete surface;
    }

    std::cout << "All done. Press Enter to exit." << std::endl;
    std::cin.get();
    return 0;
}



///////////////////////////////////////////////////////////////////////////
// Modelling of Scene 1.
///////////////////////////////////////////////////////////////////////////

void DefineScene1( Scene &scene, int imageWidth, int imageHeight )
{
    scene.backgroundColor = Color( 0.2f, 0.3f, 0.5f );

    scene.amLight.I_a = Color( 1.0f, 1.0f, 1.0f ) * 0.25f;

// Define materials.

    // Light red.
    Material lightRed = Material();
    lightRed.k_d = Color( 0.8f, 0.4f, 0.4f );
    lightRed.k_a = lightRed.k_d;
    lightRed.k_r = Color( 0.8f, 0.8f, 0.8f ) / 1.5f;
    lightRed.k_rg = Color( 0.8f, 0.8f, 0.8f ) / 3.0f;
    lightRed.n = 64.0f;

    // Light green.
    Material lightGreen = Material();
    lightGreen.k_d = Color( 0.4f, 0.8f, 0.4f );
    lightGreen.k_a = lightGreen.k_d;
    lightGreen.k_r = Color( 0.8f, 0.8f, 0.8f ) / 1.5f;
    lightGreen.k_rg = Color( 0.8f, 0.8f, 0.8f ) / 3.0f;
    lightGreen.n = 64.0f;

    // Light blue.
    Material lightBlue = Material();
    lightBlue.k_d = Color( 0.4f, 0.4f, 0.8f ) * 0.9f;
    lightBlue.k_a = lightBlue.k_d;
    lightBlue.k_r = Color( 0.8f, 0.8f, 0.8f ) / 1.5f;
    lightBlue.k_rg = Color( 0.8f, 0.8f, 0.8f ) / 2.5f;
    lightBlue.n = 64.0f;

    // Yellow.
    Material yellow = Material();
    yellow.k_d = Color( 0.6f, 0.6f, 0.2f );
    yellow.k_a = yellow.k_d;
    yellow.k_r = Color( 0.8f, 0.8f, 0.8f ) / 1.5f;
    yellow.k_rg = Color( 0.8f, 0.8f, 0.8f ) / 3.0f;
    yellow.n = 64.0f;

    // Gray.
    Material gray = Material();
    gray.k_d = Color( 0.6f, 0.6f, 0.6f );
    gray.k_a = gray.k_d;
    gray.k_r = Color( 0.6f, 0.6f, 0.6f );
    gray.k_rg = Color( 0.8f, 0.8f, 0.8f ) / 3.0f;
    gray.n = 128.0f;

    // Insert into scene materials vector.
    scene.materials = { lightRed, lightGreen, lightBlue, yellow, gray };


// Define point light sources.

    scene.ptLights.resize(2);

    PointLightSource light0 = { Vector3d(100.0, 120.0, 10.0), Color(1.0f, 1.0f, 1.0f) * 0.6f };
    PointLightSource light1 = { Vector3d(5.0, 80.0, 60.0), Color(1.0f, 1.0f, 1.0f) * 0.6f };

    scene.ptLights = { light0, light1 };


// Define surface primitives.

    scene.surfaces.resize(15);

    auto horzPlane = new Plane( 0.0, 1.0, 0.0, 0.0, scene.materials[2] ); // Horizontal plane.
    auto leftVertPlane = new Plane( 1.0, 0.0, 0.0, 0.0, scene.materials[4] ); // Left vertical plane.
    auto rightVertPlane = new Plane( 0.0, 0.0, 1.0, 0.0, scene.materials[4] ); // Right vertical plane.
    auto bigSphere =  new Sphere( Vector3d( 40.0, 20.0, 42.0 ), 22.0, scene.materials[0] ); // Big sphere.
    auto smallSphere = new Sphere( Vector3d( 75.0, 10.0, 40.0 ), 12.0, scene.materials[1] ); // Small sphere.

    // Cube +y face.
    auto cubePosYTri1 = new Triangle( Vector3d( 50.0, 20.0, 90.0 ),
                                      Vector3d( 50.0, 20.0, 70.0 ),
                                      Vector3d( 30.0, 20.0, 70.0 ),
                                      scene.materials[3] );
    auto cubePosYTri2 = new Triangle( Vector3d( 50.0, 20.0, 90.0 ),
                                      Vector3d( 30.0, 20.0, 70.0 ),
                                      Vector3d( 30.0, 20.0, 90.0 ),
                                      scene.materials[3] );

    // Cube +x face.
    auto cubePosXTri1 = new Triangle( Vector3d( 50.0, 0.0, 70.0 ),
                                      Vector3d( 50.0, 20.0, 70.0 ),
                                      Vector3d( 50.0, 20.0, 90.0 ),
                                      scene.materials[3]);
    auto cubePosXTri2 = new Triangle( Vector3d( 50.0, 0.0, 70.0 ),
                                      Vector3d( 50.0, 20.0, 90.0 ),
                                      Vector3d( 50.0, 0.0, 90.0 ),
                                      scene.materials[3] );

    // Cube -x face.
    auto cubeNegXTri1 = new Triangle( Vector3d( 30.0, 0.0, 90.0 ),
                                      Vector3d( 30.0, 20.0, 90.0 ),
                                      Vector3d( 30.0, 20.0, 70.0 ),
                                      scene.materials[3]);
    auto cubeNegXTri2 = new Triangle( Vector3d( 30.0, 0.0, 90.0 ),
                                      Vector3d( 30.0, 20.0, 70.0 ),
                                      Vector3d( 30.0, 0.0, 70.0 ),
                                      scene.materials[3] );

    // Cube +z face.
    auto cubePosZTri1 = new Triangle( Vector3d( 50.0, 0.0, 90.0 ),
                                      Vector3d( 50.0, 20.0, 90.0 ),
                                      Vector3d( 30.0, 20.0, 90.0 ),
                                      scene.materials[3]);
    auto cubePosZTri2 = new Triangle( Vector3d( 50.0, 0.0, 90.0 ),
                                      Vector3d( 30.0, 20.0, 90.0 ),
                                      Vector3d( 30.0, 0.0, 90.0 ),
                                      scene.materials[3] );

    // Cube -z face.
    auto cubeNegZTri1 = new Triangle( Vector3d( 30.0, 0.0, 70.0 ),
                                      Vector3d( 30.0, 20.0, 70.0 ),
                                      Vector3d( 50.0, 20.0, 70.0 ),
                                      scene.materials[3] );
    auto cubeNegZTri2 = new Triangle( Vector3d( 30.0, 0.0, 70.0 ),
                                      Vector3d( 50.0, 20.0, 70.0 ),
                                      Vector3d( 50.0, 0.0, 70.0 ),
                                      scene.materials[3] );

    scene.surfaces = { horzPlane, leftVertPlane, rightVertPlane, 
                       bigSphere, smallSphere,
                       cubePosXTri1, cubePosXTri2,
                       cubePosYTri1, cubePosYTri2,
                       cubePosZTri1, cubePosZTri2,
                       cubeNegXTri1, cubeNegXTri2,
                       cubeNegZTri1, cubeNegZTri2 };


// Define camera.

    scene.camera = Camera( Vector3d( 150.0, 120.0, 150.0 ),  // eye
                           Vector3d( 45.0, 22.0, 55.0 ),  // lookAt
                           Vector3d( 0.0, 1.0, 0.0 ),  //upVector
                           (-1.0 * imageWidth) / imageHeight,  // left
                           (1.0 * imageWidth) / imageHeight,  // right
                           -1.0, 1.0, 3.0,  // bottom, top, near
                           imageWidth, imageHeight );  // image_width, image_height
}



///////////////////////////////////////////////////////////////////////////
// Modeling of Scene 2.
///////////////////////////////////////////////////////////////////////////

void DrawTetrahedron(Vector3d c, Scene& s, Material m, double scale, bool rotateX, bool rotateY, bool rotateZ);

void DrawTetrahedron(Vector3d c, Scene& s,  Material m, double scale, bool rotateX, bool rotateY, bool rotateZ) {
    
    // draw out the four coordinates of the tetrahedron

    Vector3d v1 = Vector3d(1, 0, - (1 / sqrt(2)));

    Vector3d v2 = Vector3d(-1, 0, -(1 / sqrt(2)));

    Vector3d v3 = Vector3d(0, 1, 1 / sqrt(2));

    Vector3d v4 = Vector3d(0, -1, 1 / sqrt(2));

    // do necessary transformation

    v1 *= scale; v2 *= scale; v3 *= scale; v4 *= scale;

    double rotation = 180;

    if (rotateX) {
        v1 = Vector3d(v1.x(), dot(v1, Vector3d(0, cos(rotation), -sin(rotation))),
            dot(v1, Vector3d(0, sin(rotation), cos(rotation))));

        v2 = Vector3d(v2.x(), dot(v2, Vector3d(0, cos(rotation), -sin(rotation))),
            dot(v2, Vector3d(0, sin(rotation), cos(rotation))));

        v3 = Vector3d(v3.x(), dot(v3, Vector3d(0, cos(rotation), -sin(rotation))),
            dot(v3, Vector3d(0, sin(rotation), cos(rotation))));

        v4 = Vector3d(v4.x(), dot(v4, Vector3d(0, cos(rotation), -sin(rotation))),
            dot(v4, Vector3d(0, sin(rotation), cos(rotation))));
    }

    else if (rotateY) {
        v1 = Vector3d(dot(v1, Vector3d(cos(rotation), 0, sin(rotation))), v1.y(),
            dot(v1, Vector3d(-sin(rotation), 0, cos(rotation))));

        v2 = Vector3d(dot(v2, Vector3d(cos(rotation), 0, sin(rotation))), v2.y(),
            dot(v2, Vector3d(-sin(rotation), 0, cos(rotation))));

        v3 = Vector3d(dot(v3, Vector3d(cos(rotation), 0, sin(rotation))), v3.y(),
            dot(v3, Vector3d(-sin(rotation), 0, cos(rotation))));

        v4 = Vector3d(dot(v4, Vector3d(cos(rotation), 0, sin(rotation))), v4.y(),
            dot(v4, Vector3d(-sin(rotation), 0, cos(rotation))));
    }

    else if (rotateZ) {
        v1 = Vector3d(dot(v1, Vector3d(cos(rotation), -sin(rotation), 0)),
            dot(v1, Vector3d(sin(rotation), cos(rotation), 0)),
            v1.z());

        v2 = Vector3d(dot(v2, Vector3d(cos(rotation), -sin(rotation), 0)),
            dot(v2, Vector3d(sin(rotation), cos(rotation), 0)),
            v2.z());

        v3 = Vector3d(dot(v3, Vector3d(cos(rotation), -sin(rotation), 0)),
            dot(v3, Vector3d(sin(rotation), cos(rotation), 0)),
            v3.z());

        v4 = Vector3d(dot(v4, Vector3d(cos(rotation), -sin(rotation), 0)),
            dot(v4, Vector3d(sin(rotation), cos(rotation), 0)),
            v4.z());
    }

    // transform to centre c

    v1 += c; v2 += c; v3 += c; v4 += c;

    // Draw Spike

    auto t1 = new Triangle(v1, v2, v3, m);
    auto t2 = new Triangle(v1, v3, v4, m);
    auto t3 = new Triangle(v2, v3, v4, m);
    auto t4 = new Triangle(v1, v2, v4, m);

    s.surfaces.push_back(t1);
    s.surfaces.push_back(t2);
    s.surfaces.push_back(t3);
    s.surfaces.push_back(t4);
    
}

void DefineScene2( Scene &scene, int imageWidth, int imageHeight )
{
    // background and ambient

    scene.backgroundColor = Color(0.098f, 0.098f, 0.439f);

    scene.amLight.I_a = Color(0.6f, 0.6f, 0.6f) * 0.25f;
    
    // materials

    // Midnight blue
    Material midnightBlue = Material();
    midnightBlue.k_d = Color(0.098f, 0.098f, 0.439f);
    midnightBlue.k_a = midnightBlue.k_d;
    midnightBlue.k_r = Color(0.8f, 0.8f, 0.8f) / 1.5f;
    midnightBlue.k_rg = Color(0.8f, 0.8f, 0.8f) / 3.0f;
    midnightBlue.n = 128.0f;

    // Yellow.
    Material yellow = Material();
    yellow.k_d = Color(1.0f, 1.0f, 0.0f);
    yellow.k_a = yellow.k_d;
    yellow.k_r = Color(1.0f, 1.0f, 1.0f);
    yellow.k_rg = Color(1.0f, 1.0f, 1.0f);
    yellow.n = 8.0f;

    // Bright Yellow.
    Material Brightyellow = Material();
    Brightyellow.k_d = Color(1.0f, 1.0f, 0.0f);
    Brightyellow.k_a = Brightyellow.k_d;
    Brightyellow.k_r = Color(1.0f, 1.0f, 1.0f);
    Brightyellow.k_rg = Color(1.0f, 1.0f, 1.0f);
    Brightyellow.n = 0.0f;


    // Insert material 
    scene.materials = { Brightyellow, midnightBlue, yellow };


    // point light sources

    scene.ptLights.resize(2);

    PointLightSource light0 = { Vector3d(100.0, 120.0, 10.0), Color(1.0f, 1.0f, 1.0f) * 0.3f };
    PointLightSource light1 = { Vector3d(5.0, 80.0, 60.0), Color(1.0f, 1.0f, 1.0f) * 0.3f };

    scene.ptLights = { light0, light1 }; 


    // surface primitives

    scene.surfaces.resize(15);

    auto horzPlane = new Plane(0.0, 1.0, 0.0, 0.0, scene.materials[1]); // Horizontal plane.
    auto leftVertPlane = new Plane(1.0, 0.0, 0.0, 0.0, scene.materials[1]); // Left vertical plane.
    auto rightVertPlane = new Plane(0.0, 0.0, 1.0, 0.0, scene.materials[1]); // Right vertical plane.
    auto Moon = new Sphere(Vector3d(70.0, 40.0, 80.0), 30.0, scene.materials[2]);
    scene.surfaces = { horzPlane, leftVertPlane, rightVertPlane, Moon};

    // Draw the stars
    /*DrawTetrahedron(Vector3d(70, 40, 80), scene, scene.materials[0], 30, 0, 0, 0);
    DrawTetrahedron(Vector3d(70, 40, 80), scene, scene.materials[0], -30, 0, 0, 0);*/

    DrawTetrahedron(Vector3d(35, 75, 100), scene, scene.materials[0], 15, 1, 0, 0);
    DrawTetrahedron(Vector3d(35, 75, 100), scene, scene.materials[0], -15, 1, 0, 0);

    DrawTetrahedron(Vector3d(120, 23, 70), scene, scene.materials[0], 5, 0, 0, 1);
    DrawTetrahedron(Vector3d(120, 23, 70), scene, scene.materials[0], -5, 0, 0, 1);

    DrawTetrahedron(Vector3d(45, 50, 130), scene, scene.materials[0], 5, 0, 0, 1);
    DrawTetrahedron(Vector3d(45, 50, 130), scene, scene.materials[0], -5, 0, 0, 1);

    DrawTetrahedron(Vector3d(120, 80, 70), scene, scene.materials[0], 5, 1, 0, 1);
    DrawTetrahedron(Vector3d(120, 80, 70), scene, scene.materials[0], -5, 1, 0, 1);

    DrawTetrahedron(Vector3d(35, 70, 20), scene, scene.materials[0], 10, 0, 1, 0);
    DrawTetrahedron(Vector3d(35, 70, 20), scene, scene.materials[0], -10, 0, 1, 0);

    DrawTetrahedron(Vector3d(56, 11, 130), scene, scene.materials[0], 10, 1, 0, 1);
    DrawTetrahedron(Vector3d(56, 11, 130), scene, scene.materials[0], -10, 1, 0, 1);


    // camera
    scene.camera = Camera(Vector3d(200.0, 120.0, 200.0),  // eye
        Vector3d(45.0, 22.0, 55.0),  // lookAt
        Vector3d(0.0, 1.0, 0.0),  //upVector
        (-1.0 * imageWidth) / imageHeight,  // left
        (1.0 * imageWidth) / imageHeight,  // right
        -1.0, 1.0, 3.0,  // bottom, top, near
        imageWidth, imageHeight);  // image_width, image_height

}
