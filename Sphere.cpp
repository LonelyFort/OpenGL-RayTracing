//============================================================
// STUDENT NAME: Liu Yu-Wei
// NUS User ID.: E1122378
// COMMENTS TO GRADER:
//
// ============================================================

#include <cmath>
#include "Sphere.h"

using namespace std;



bool Sphere::hit( const Ray &r, double tmin, double tmax, SurfaceHitRecord &rec ) const 
{
    // Get vector origin with reference from sphere centre as origin
    Vector3d origin = r.origin() - center;

    // Form Quadratic equation and solve for t
    double a = dot(r.direction(), r.direction());
    double b = 2 * dot(r.direction(), origin);
    double c = dot(origin, origin) - radius * radius;

    // Find discriminant to determine if the ray hit
    double d = b * b - 4 * a * c;
    if (d < 0) return false;

    // 2 solutions for t. Find the t that is positive and closest
    double t;
    double t1 = (-b - sqrt(d)) / (2 * a);
    double t2 = (-b + sqrt(d)) / (2 * a);
    if (t1 >= 0) t = t1;
    else t = t2;
    if (t < tmin || t > tmax) return false;

    rec.t = t;
    rec.p = r.pointAtParam(t);
    rec.normal = r.pointAtParam(t).unitVector();
    rec.material = material;

    return true;
}



bool Sphere::shadowHit( const Ray &r, double tmin, double tmax ) const 
{
    Vector3d origin = r.origin() - center;

    double a = dot(r.direction(), r.direction());
    double b = 2 * dot(r.direction(), origin);
    double c = dot(origin, origin) - radius * radius;

    double d = b * b - 4 * a * c;
    if (d < 0) return false;

    double t;
    double t1 = (-b - sqrt(d)) / (2 * a);
    double t2 = (-b + sqrt(d)) / (2 * a);
    if (t1 >= 0) t = t1;
    else t = t2;
    return (t >= tmin && t <= tmax);
}
