#pragma once
#include "point3.hpp"
#include "vector3.hpp"
#include "ray.hpp"
#include <cmath>
#include <iostream>

// Sphere primitive represents a perfect sphere in 3D space
// Mathematical definition: all points at distance 'radius' from center point
// Equation: (x-cx)² + (y-cy)² + (z-cz)² = r² where (cx,cy,cz) is center, r is radius
// Ray tracing usage: fundamental primitive for rendering spherical objects
class Sphere {
public:
    Point3 center;  // Center point of sphere in 3D space
    float radius;   // Radius distance from center to surface

    // Constructor with explicit center point and radius
    // Geometric interpretation: defines sphere as locus of points at radius distance from center
    Sphere(const Point3& center, float radius) : center(center), radius(radius) {}

    // Intersection result structure containing all intersection information
    // hit: whether ray intersects sphere (discriminant ≥ 0 and t > 0)
    // t: ray parameter at intersection point (distance along ray direction)
    // point: 3D coordinates of intersection point on sphere surface
    // normal: surface normal vector at intersection point (points outward from center)
    struct Intersection {
        bool hit;           // True if intersection occurs
        float t;            // Ray parameter at intersection (ray.at(t) = intersection point)
        Point3 point;       // 3D intersection point coordinates
        Vector3 normal;     // Outward-pointing surface normal at intersection
        
        // Default constructor for no-intersection case
        Intersection() : hit(false), t(0.0f), point(Point3()), normal(Vector3()) {}
        
        // Constructor for valid intersection
        Intersection(float t_value, const Point3& hit_point, const Vector3& surface_normal)
            : hit(true), t(t_value), point(hit_point), normal(surface_normal) {}
    };

    // Ray-sphere intersection using quadratic equation solution
    // Mathematical approach: substitute ray equation P(t) = O + t*D into sphere equation
    // Geometric interpretation: find where infinite ray line crosses sphere surface
    // Returns closest intersection with t > 0 (in front of ray origin)
    Intersection intersect(const Ray& ray) const {
        std::cout << "\n=== Ray-Sphere Intersection Calculation ===" << std::endl;
        std::cout << "Ray origin: (" << ray.origin.x << ", " << ray.origin.y << ", " << ray.origin.z << ")" << std::endl;
        std::cout << "Ray direction: (" << ray.direction.x << ", " << ray.direction.y << ", " << ray.direction.z << ")" << std::endl;
        std::cout << "Sphere center: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
        std::cout << "Sphere radius: " << radius << std::endl;

        // Vector from ray origin to sphere center
        // Geometric interpretation: displacement needed to go from ray start to sphere center
        Vector3 oc = ray.origin - center;
        std::cout << "Origin-to-center vector (oc): (" << oc.x << ", " << oc.y << ", " << oc.z << ")" << std::endl;

        // Quadratic equation coefficients for ray-sphere intersection
        // Derived from: |O + t*D - C|² = r²
        // Expanded to: (D·D)t² + 2(OC·D)t + (OC·OC - r²) = 0
        // Where O=origin, D=direction, C=center, OC=origin-center
        
        // Coefficient 'a': D·D (direction vector dot product with itself)
        // Geometric interpretation: squared length of direction vector
        float a = ray.direction.dot(ray.direction);
        std::cout << "Quadratic coefficient a = D·D = " << a << std::endl;
        
        // Coefficient 'b': 2(OC·D) (twice the projection of oc onto direction)
        // Geometric interpretation: how much origin-center vector aligns with ray direction
        float b = 2.0f * oc.dot(ray.direction);
        std::cout << "Quadratic coefficient b = 2(OC·D) = " << b << std::endl;
        
        // Coefficient 'c': OC·OC - r² (squared distance from origin to center minus squared radius)
        // Geometric interpretation: how far ray origin is from sphere surface
        float c = oc.dot(oc) - radius * radius;
        std::cout << "Quadratic coefficient c = OC·OC - r² = " << c << std::endl;

        // Discriminant determines intersection type:
        // Δ > 0: two intersections (ray passes through sphere)
        // Δ = 0: one intersection (ray tangent to sphere)  
        // Δ < 0: no intersection (ray misses sphere)
        float discriminant = b * b - 4 * a * c;
        std::cout << "Discriminant Δ = b² - 4ac = " << discriminant << std::endl;

        // No intersection if discriminant is negative
        if (discriminant < 0) {
            std::cout << "No intersection: discriminant < 0 (ray misses sphere)" << std::endl;
            return Intersection();  // Default constructor creates hit=false
        }

        // Calculate both intersection points using quadratic formula
        // t = (-b ± √Δ) / 2a
        float sqrt_discriminant = std::sqrt(discriminant);
        std::cout << "Square root of discriminant: √Δ = " << sqrt_discriminant << std::endl;
        
        float t1 = (-b - sqrt_discriminant) / (2 * a);  // Near intersection
        float t2 = (-b + sqrt_discriminant) / (2 * a);  // Far intersection
        std::cout << "Intersection parameters: t1 = " << t1 << ", t2 = " << t2 << std::endl;

        // Choose closest intersection in front of ray (t > 0)
        // For rays (not lines), we only consider forward intersections
        float t_hit;
        if (t1 > 1e-6f) {  // Use small epsilon to avoid self-intersection
            t_hit = t1;    // Closer intersection is valid
            std::cout << "Using closer intersection t1 = " << t_hit << std::endl;
        } else if (t2 > 1e-6f) {
            t_hit = t2;    // Only far intersection is valid (ray starts inside sphere)
            std::cout << "Using farther intersection t2 = " << t_hit << " (ray starts inside sphere)" << std::endl;
        } else {
            std::cout << "No valid intersection: both t values ≤ 0 (intersections behind ray origin)" << std::endl;
            return Intersection();  // Both intersections behind ray origin
        }

        // Calculate intersection point using ray equation P(t) = O + t*D
        Point3 hit_point = ray.at(t_hit);
        std::cout << "Intersection point: (" << hit_point.x << ", " << hit_point.y << ", " << hit_point.z << ")" << std::endl;

        // Calculate surface normal at intersection point
        // Normal points outward from sphere center to surface point
        // Formula: N = (P - C) / |P - C| where P=intersection point, C=center
        Vector3 normal = (hit_point - center).normalize();
        std::cout << "Surface normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
        
        // Verify normal is unit length
        std::cout << "Normal length verification: |N| = " << normal.length() << " (should be ≈ 1.0)" << std::endl;
        
        std::cout << "=== Intersection calculation complete ===" << std::endl;

        return Intersection(t_hit, hit_point, normal);
    }

    // Validates sphere geometry for mathematical correctness
    // Checks: positive radius, finite center coordinates
    // Usage: defensive programming to catch invalid sphere configurations
    bool validate_geometry() const {
        if (radius <= 0) {
            std::cout << "Invalid sphere: radius must be positive, got " << radius << std::endl;
            return false;
        }
        
        if (!std::isfinite(center.x) || !std::isfinite(center.y) || !std::isfinite(center.z)) {
            std::cout << "Invalid sphere: center coordinates must be finite" << std::endl;
            return false;
        }
        
        if (!std::isfinite(radius)) {
            std::cout << "Invalid sphere: radius must be finite" << std::endl;
            return false;
        }
        
        return true;
    }
    
    // Calculate surface area for validation and educational purposes
    // Formula: A = 4πr² (standard sphere surface area)
    // Usage: mathematical verification and debugging
    float surface_area() const {
        return 4.0f * M_PI * radius * radius;
    }
    
    // Calculate volume for validation and educational purposes  
    // Formula: V = (4/3)πr³ (standard sphere volume)
    // Usage: mathematical verification and debugging
    float volume() const {
        return (4.0f / 3.0f) * M_PI * radius * radius * radius;
    }
};