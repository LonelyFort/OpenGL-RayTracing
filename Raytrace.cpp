//============================================================
// STUDENT NAME: Liu Yu-Wei
// NUS User ID.: E1122378
// COMMENTS TO GRADER:
//
// ============================================================

#include <cmath>
#include <cfloat>
#include "Vector3d.h"
#include "Color.h"
#include "Ray.h"
#include "Material.h"
#include "Surface.h"
#include "Light.h"
#include "Scene.h"
#include "Raytrace.h"


// This is for avoiding the "epsilon problem" or the shadow acne problem.
static constexpr double DEFAULT_TMIN = 10e-6;

// Use this for tmax for non-shadow ray intersection test.
static constexpr double DEFAULT_TMAX = DBL_MAX;


//////////////////////////////////////////////////////////////////////////////
// Compute the outgoing mirror reflection vector.
// Input incoming vector L is pointing AWAY from surface point.
// Assume normal vector N is unit vector.
// The output reflection vector is pointing AWAY from surface point, and
// has same length as incoming vector L.
//////////////////////////////////////////////////////////////////////////////

static Vector3d mirrorReflect( const Vector3d &L, const Vector3d &N )
{
    return ( 2.0 * dot( N, L ) ) * N - L;
}



//////////////////////////////////////////////////////////////////////////////
// Compute I_source * [ k_d * (N.L) + k_r * (R.V)^n ].
// Input vectors L, N and V are pointing AWAY from surface point.
// Assume all vector L, N and V are unit vectors.
//////////////////////////////////////////////////////////////////////////////

static Color computePhongLighting( const Vector3d &L, const Vector3d &N, const Vector3d &V,
                                   const Material &mat, const PointLightSource &ptLight )
{
    Vector3d R = mirrorReflect(L, N);

    auto N_dot_L = (float)dot(N, L);
    if (N_dot_L < 0.0f) N_dot_L = 0.0f;

    auto R_dot_V = (float)dot(R, V);
    if (R_dot_V < 0.0f) R_dot_V = 0.0f;
    float R_dot_V_pow_n = powf(R_dot_V, (float)mat.n);

    return ptLight.I_source * (mat.k_d * N_dot_L + mat.k_r * R_dot_V_pow_n);
}



//////////////////////////////////////////////////////////////////////////////
// Traces a ray into the scene.
// reflectLevels: specifies number of levels of reflections (0 for no reflection).
// hasShadow: specifies whether to generate shadows.
//////////////////////////////////////////////////////////////////////////////

Color Raytrace::TraceRay( const Ray &ray, const Scene &scene, 
                          int reflectLevels, bool hasShadow )
{
    Ray uRay( ray );
    uRay.makeUnitDirection();  // Normalize ray direction.


// Find whether and where the ray hits some surface.
// Take the nearest hit point.

    bool hasHitSomething = false;
    double nearest_t = DEFAULT_TMAX;
    SurfaceHitRecord nearestHitRec;

    for (const auto& surface : scene.surfaces)
    {
        SurfaceHitRecord tempHitRec;
        bool hasHit = surface->hit( uRay, DEFAULT_TMIN, DEFAULT_TMAX, tempHitRec );

        if ( hasHit && tempHitRec.t < nearest_t )
        {
            hasHitSomething = true;
            nearest_t = tempHitRec.t;
            nearestHitRec = tempHitRec;
        }
    }

    if ( !hasHitSomething ) return scene.backgroundColor;

    nearestHitRec.normal.makeUnitVector();
    Vector3d N = nearestHitRec.normal;  // Unit vector.
    Vector3d V = -uRay.direction();     // Unit vector.

    Color result( 0.0f, 0.0f, 0.0f );   // The result will be accumulated here.


// Add to result the phong lighting contributed by each point light source.
// Compute for shadow if hasShadow is true.

    for (PointLightSource lightsrc : scene.ptLights) {
        Vector3d L = (lightsrc.position - nearestHitRec.p).unitVector();

        Color kshadow(1.0, 1.0, 1.0);

        if (hasShadow) {

            //initiate Shadow Ray
            Ray shadowRay(nearestHitRec.p, L);

            //check blockage
            for (auto& surface : scene.surfaces) {
                double maxT = (lightsrc.position - nearestHitRec.p).length();
                if (surface->shadowHit(shadowRay, DEFAULT_TMIN, maxT)) {
                    kshadow.setRGB(0.0, 0.0, 0.0);
                    break;
                }
            }
        }
        result += kshadow * computePhongLighting(L, N, V, nearestHitRec.material, lightsrc);
    }

// Add to result the global ambient lighting.

    result += scene.amLight.I_a * nearestHitRec.material.k_a;

// Add to result the reflection of the scene.

    if (reflectLevels > 0) {
        //reflect ray from camera for subsequent reflections
        Vector3d reflectedVector = mirrorReflect(V, N);
        Ray reflectedRay = Ray(nearestHitRec.p, reflectedVector);

        result += nearestHitRec.material.k_rg * TraceRay(reflectedRay, scene, reflectLevels - 1, hasShadow);
    }
    return result;
}
