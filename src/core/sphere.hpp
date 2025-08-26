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
    Point3 center;      // Center point of sphere in 3D space
    float radius;       // Radius distance from center to surface
    int material_index; // Index into Scene's materials vector for material properties

    // Constructor with explicit center point, radius, and material index
    // Geometric interpretation: defines sphere as locus of points at radius distance from center
    // Material reference: enables material property lookup in Scene's materials container
    Sphere(const Point3& center, float radius, int material_idx, bool verbose = false) 
        : center(center), radius(radius), material_index(material_idx) {
        validate_and_clamp_parameters(verbose);
    }

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
    //
    // MATHEMATICAL DERIVATION:
    //
    // Sphere equation: |P - C|² = r²  where P=point, C=center, r=radius
    // Ray equation: P(t) = O + t*D    where O=origin, D=direction, t=parameter
    //
    // Substituting ray into sphere:
    //   |O + t*D - C|² = r²
    //   |oc + t*D|² = r²           where oc = O - C
    //   (oc + t*D)·(oc + t*D) = r²
    //   oc·oc + 2t(oc·D) + t²(D·D) = r²
    //   (D·D)t² + 2(oc·D)t + (oc·oc - r²) = 0
    //
    // Standard quadratic form: at² + bt + c = 0
    //   a = D·D     (squared length of direction)
    //   b = 2(oc·D) (projection of origin-center onto direction)
    //   c = oc·oc - r² (squared distance from origin to center, minus r²)
    //
    // Solutions using quadratic formula:
    //   t = (-b ± √(b² - 4ac)) / (2a)
    //
    // Discriminant interpretation:
    //   Δ = b² - 4ac
    //   Δ > 0: Two intersections (ray passes through sphere)
    //   Δ = 0: One intersection (ray tangent to sphere)
    //   Δ < 0: No intersection (ray misses sphere)
    //
    // ASCII Art Diagram:
    //
    //    Ray-Sphere Intersection Cases:
    //
    //    Case 1: Two intersections (Δ > 0)
    //         O----t1---t2-----> D
    //            \  (  ) /
    //             \ ( C ) /
    //              \(   )/  
    //               \   /
    //                \ /
    //
    //    Case 2: Tangent (Δ = 0)
    //         O-------t---------> D
    //                |
    //                ( C )
    //               (     )
    //                \   /
    //                 \ /
    //
    //    Case 3: Miss (Δ < 0)
    //         O-----------------> D
    //                 ( C )
    //                (     )
    //                 \   /
    //                  \ /
    //
    // Reference: "Real-Time Rendering" by Akenine-Möller et al. (4th ed.)
    //           "Ray Tracing Gems" edited by Haines & Shirley (2019)
    Intersection intersect(const Ray& ray, bool verbose = true) const {
        if (verbose) {
            std::cout << "\n=== Ray-Sphere Intersection Calculation ===" << std::endl;
            std::cout << "Ray origin: (" << ray.origin.x << ", " << ray.origin.y << ", " << ray.origin.z << ")" << std::endl;
            std::cout << "Ray direction: (" << ray.direction.x << ", " << ray.direction.y << ", " << ray.direction.z << ")" << std::endl;
            std::cout << "Sphere center: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
            std::cout << "Sphere radius: " << radius << std::endl;

        }
        
        // Vector from ray origin to sphere center
        // Geometric interpretation: displacement needed to go from ray start to sphere center
        Vector3 oc = ray.origin - center;
        if (verbose) {
            std::cout << "Origin-to-center vector (oc): (" << oc.x << ", " << oc.y << ", " << oc.z << ")" << std::endl;
        }

        // Quadratic equation coefficients for ray-sphere intersection
        // Derived from: |O + t*D - C|² = r²
        // Expanded to: (D·D)t² + 2(OC·D)t + (OC·OC - r²) = 0
        // Where O=origin, D=direction, C=center, OC=origin-center
        
        // Coefficient 'a': D·D (direction vector dot product with itself)
        // Geometric interpretation: squared length of direction vector
        float a = ray.direction.dot(ray.direction);
        if (verbose) {
            std::cout << "Quadratic coefficient a = D·D = " << a << std::endl;
        }
        
        // Coefficient 'b': 2(OC·D) (twice the projection of oc onto direction)
        // Geometric interpretation: how much origin-center vector aligns with ray direction
        float b = 2.0f * oc.dot(ray.direction);
        if (verbose) {
            std::cout << "Quadratic coefficient b = 2(OC·D) = " << b << std::endl;
        }
        
        // Coefficient 'c': OC·OC - r² (squared distance from origin to center minus squared radius)
        // Geometric interpretation: how far ray origin is from sphere surface
        float c = oc.dot(oc) - radius * radius;
        if (verbose) {
            std::cout << "Quadratic coefficient c = OC·OC - r² = " << c << std::endl;
        }

        // Discriminant determines intersection type:
        // Δ > 0: two intersections (ray passes through sphere)
        // Δ = 0: one intersection (ray tangent to sphere)  
        // Δ < 0: no intersection (ray misses sphere)
        float discriminant = b * b - 4 * a * c;
        if (verbose) {
            std::cout << "Discriminant Δ = b² - 4ac = " << discriminant << std::endl;
        }

        // No intersection if discriminant is negative
        if (discriminant < 0) {
            if (verbose) {
                std::cout << "No intersection: discriminant < 0 (ray misses sphere)" << std::endl;
            }
            return Intersection();  // Default constructor creates hit=false
        }

        // Calculate both intersection points using quadratic formula
        // t = (-b ± √Δ) / 2a
        float sqrt_discriminant = std::sqrt(discriminant);
        if (verbose) {
            std::cout << "Square root of discriminant: √Δ = " << sqrt_discriminant << std::endl;
        }
        
        float t1 = (-b - sqrt_discriminant) / (2 * a);  // Near intersection
        float t2 = (-b + sqrt_discriminant) / (2 * a);  // Far intersection
        if (verbose) {
            std::cout << "Intersection parameters: t1 = " << t1 << ", t2 = " << t2 << std::endl;
        }

        // Choose closest intersection in front of ray (t > 0)
        // For rays (not lines), we only consider forward intersections
        float t_hit;
        if (t1 > 1e-6f) {  // Use small epsilon to avoid self-intersection
            t_hit = t1;    // Closer intersection is valid
            if (verbose) {
                std::cout << "Using closer intersection t1 = " << t_hit << std::endl;
            }
        } else if (t2 > 1e-6f) {
            t_hit = t2;    // Only far intersection is valid (ray starts inside sphere)
            if (verbose) {
                std::cout << "Using farther intersection t2 = " << t_hit << " (ray starts inside sphere)" << std::endl;
            }
        } else {
            if (verbose) {
                std::cout << "No valid intersection: both t values ≤ 0 (intersections behind ray origin)" << std::endl;
            }
            return Intersection();  // Both intersections behind ray origin
        }

        // Calculate intersection point using ray equation P(t) = O + t*D
        Point3 hit_point = ray.at(t_hit);
        if (verbose) {
            std::cout << "Intersection point: (" << hit_point.x << ", " << hit_point.y << ", " << hit_point.z << ")" << std::endl;
        }

        // Calculate surface normal at intersection point
        // Normal points outward from sphere center to surface point
        // Formula: N = (P - C) / |P - C| where P=intersection point, C=center
        Vector3 normal = (hit_point - center).normalize();
        if (verbose) {
            std::cout << "Surface normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
            
            // Verify normal is unit length
            std::cout << "Normal length verification: |N| = " << normal.length() << " (should be ≈ 1.0)" << std::endl;
            
            std::cout << "=== Intersection calculation complete ===" << std::endl;
        }

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

    // Enhanced validation for sphere parameters including material index
    // Checks: finite center coordinates, positive radius, valid material index
    bool is_valid() const {
        bool center_finite = std::isfinite(center.x) && std::isfinite(center.y) && std::isfinite(center.z);
        bool radius_valid = radius > 0.0f && radius < 1000.0f && std::isfinite(radius);
        bool material_valid = material_index >= 0;
        
        if (!center_finite) {
            std::cout << "Invalid sphere: center coordinates must be finite" << std::endl;
        }
        if (!radius_valid) {
            std::cout << "Invalid sphere: radius must be positive, finite, and < 1000, got " << radius << std::endl;
        }
        if (!material_valid) {
            std::cout << "Invalid sphere: material index must be >= 0, got " << material_index << std::endl;
        }
        
        return center_finite && radius_valid && material_valid;
    }
    
    // Parameter validation and clamping for robust sphere construction
    // Ensures sphere parameters are within valid mathematical ranges
    void validate_and_clamp_parameters(bool verbose = false) {
        if (verbose) {
            std::cout << "\n=== Sphere Parameter Validation ===" << std::endl;
            std::cout << "Original parameters:" << std::endl;
            std::cout << "  Center: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
            std::cout << "  Radius: " << radius << std::endl;
            std::cout << "  Material index: " << material_index << std::endl;
        }
        
        // Validate and fix center coordinates
        if (!std::isfinite(center.x) || !std::isfinite(center.y) || !std::isfinite(center.z)) {
            if (verbose) {
                std::cout << "WARNING: Invalid sphere center coordinates, setting to origin" << std::endl;
            }
            center = Point3(0, 0, 0);
        }
        
        // Validate and clamp radius
        if (radius <= 0.0f) {
            if (verbose) {
                std::cout << "WARNING: Invalid sphere radius " << radius << ", clamping to 0.1" << std::endl;
            }
            radius = 0.1f;
        } else if (radius > 1000.0f) {
            if (verbose) {
                std::cout << "WARNING: Very large sphere radius " << radius << ", clamping to 1000.0" << std::endl;
            }
            radius = 1000.0f;
        } else if (!std::isfinite(radius)) {
            if (verbose) {
                std::cout << "WARNING: Non-finite sphere radius, setting to 1.0" << std::endl;
            }
            radius = 1.0f;
        }
        
        // Validate material index (cannot fix here as we don't know scene materials)
        if (material_index < 0) {
            if (verbose) {
                std::cout << "WARNING: Negative material index " << material_index << ", setting to 0" << std::endl;
            }
            material_index = 0;
        }
        
        if (verbose) {
            std::cout << "Validated parameters:" << std::endl;
            std::cout << "  Center: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
            std::cout << "  Radius: " << radius << std::endl;
            std::cout << "  Material index: " << material_index << std::endl;
            std::cout << "=== Parameter validation complete ===" << std::endl;
        }
    }
    
    // Educational method: explain intersection mathematics step-by-step
    // Provides detailed explanation of ray-sphere intersection algorithm
    void explain_intersection_math(const Ray& ray) const {
        std::cout << "\n=== Educational: Ray-Sphere Intersection Mathematics ===" << std::endl;
        std::cout << "Sphere equation: |P - C|² = r²" << std::endl;
        std::cout << "Ray equation: P(t) = O + t*D" << std::endl;
        std::cout << "Where: P=point, C=center, r=radius, O=origin, D=direction, t=parameter" << std::endl;
        
        std::cout << "\nSubstituting ray into sphere equation:" << std::endl;
        std::cout << "|O + t*D - C|² = r²" << std::endl;
        std::cout << "|oc + t*D|² = r²  (where oc = O - C)" << std::endl;
        std::cout << "(oc + t*D)·(oc + t*D) = r²" << std::endl;
        std::cout << "oc·oc + 2t(oc·D) + t²(D·D) = r²" << std::endl;
        std::cout << "(D·D)t² + 2(oc·D)t + (oc·oc - r²) = 0" << std::endl;
        
        Vector3 oc = ray.origin - center;
        float a = ray.direction.dot(ray.direction);
        float b = 2.0f * oc.dot(ray.direction);
        float c = oc.dot(oc) - radius * radius;
        
        std::cout << "\nFor current ray and sphere:" << std::endl;
        std::cout << "a = D·D = " << a << std::endl;
        std::cout << "b = 2(oc·D) = " << b << std::endl;
        std::cout << "c = oc·oc - r² = " << c << std::endl;
        
        float discriminant = b * b - 4 * a * c;
        std::cout << "discriminant = b² - 4ac = " << discriminant << std::endl;
        
        if (discriminant < 0) {
            std::cout << "Result: No intersection (discriminant < 0)" << std::endl;
        } else if (discriminant == 0) {
            std::cout << "Result: Tangent intersection (discriminant = 0)" << std::endl;
        } else {
            std::cout << "Result: Two intersections (discriminant > 0)" << std::endl;
            float sqrt_d = std::sqrt(discriminant);
            float t1 = (-b - sqrt_d) / (2 * a);
            float t2 = (-b + sqrt_d) / (2 * a);
            std::cout << "t1 = " << t1 << ", t2 = " << t2 << std::endl;
        }
        
        std::cout << "=== Mathematical explanation complete ===" << std::endl;
    }
    
    // Print sphere properties for educational and debugging purposes
    void print_sphere_properties() const {
        std::cout << "\n=== Sphere Properties ===" << std::endl;
        std::cout << "Center: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
        std::cout << "Radius: " << radius << std::endl;
        std::cout << "Material index: " << material_index << std::endl;
        std::cout << "Surface area: " << surface_area() << std::endl;
        std::cout << "Volume: " << volume() << std::endl;
        std::cout << "Geometry valid: " << (is_valid() ? "YES" : "NO") << std::endl;
        std::cout << "=== Properties display complete ===" << std::endl;
    }
};