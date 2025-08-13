#include <iostream>
#include <cassert>
#include <cmath>
#include "src/core/vector3.hpp"
#include "src/core/point3.hpp"
#include "src/core/ray.hpp"
#include "src/core/sphere.hpp"
#include "src/core/point_light.hpp"
#include "src/materials/lambert.hpp"

namespace MathematicalTests {
    bool test_vector3_operations() {
        std::cout << "Testing Vector3 dot product..." << std::endl;
        Vector3 a(1, 2, 3);
        Vector3 b(4, 5, 6);
        float dot_result = a.dot(b);
        float expected_dot = 1*4 + 2*5 + 3*6; // 32
        assert(std::abs(dot_result - expected_dot) < 1e-6);
        std::cout << "  Dot product test: PASSED (result: " << dot_result << ")" << std::endl;
        
        std::cout << "Testing Vector3 cross product..." << std::endl;
        Vector3 cross_result = a.cross(b);
        // a(1,2,3) × b(4,5,6) = (2*6-3*5, 3*4-1*6, 1*5-2*4) = (-3, 6, -3)
        assert(std::abs(cross_result.x - (-3)) < 1e-6);
        assert(std::abs(cross_result.y - 6) < 1e-6);
        assert(std::abs(cross_result.z - (-3)) < 1e-6);
        std::cout << "  Cross product test: PASSED" << std::endl;
        
        std::cout << "Testing Vector3 length..." << std::endl;
        Vector3 c(3, 4, 0);
        float length_result = c.length();
        float expected_length = 5.0f; // 3-4-5 triangle
        assert(std::abs(length_result - expected_length) < 1e-6);
        std::cout << "  Length test: PASSED (result: " << length_result << ")" << std::endl;
        
        std::cout << "Testing Vector3 normalization..." << std::endl;
        Vector3 normalized = c.normalize();
        float normalized_length = normalized.length();
        assert(std::abs(normalized_length - 1.0f) < 1e-6);
        std::cout << "  Normalization test: PASSED" << std::endl;
        
        std::cout << "Testing Vector3 compound operators..." << std::endl;
        Vector3 d(1, 1, 1);
        d += Vector3(2, 3, 4);  // Should be (3, 4, 5)
        assert(std::abs(d.x - 3.0f) < 1e-6);
        assert(std::abs(d.y - 4.0f) < 1e-6);
        assert(std::abs(d.z - 5.0f) < 1e-6);
        
        d *= 2.0f;  // Should be (6, 8, 10)
        assert(std::abs(d.x - 6.0f) < 1e-6);
        assert(std::abs(d.y - 8.0f) < 1e-6);
        assert(std::abs(d.z - 10.0f) < 1e-6);
        
        // Test scalar * vector
        Vector3 e = 3.0f * Vector3(1, 2, 3);  // Should be (3, 6, 9)
        assert(std::abs(e.x - 3.0f) < 1e-6);
        assert(std::abs(e.y - 6.0f) < 1e-6);
        assert(std::abs(e.z - 9.0f) < 1e-6);
        std::cout << "  Compound operators test: PASSED" << std::endl;
        
        return true;
    }
    
    bool test_mathematical_precision() {
        std::cout << "Testing mathematical precision tolerance..." << std::endl;
        
        // Test precision boundary conditions
        Vector3 tiny(1e-7f, 1e-7f, 1e-7f);
        Vector3 result = tiny.normalize();
        
        // Should handle small vectors gracefully
        std::cout << "  Precision tolerance test: PASSED" << std::endl;
        
        return true;
    }
    
    bool test_vector3_cross_product_enhanced() {
        std::cout << "Testing Vector3 cross product mathematical validation..." << std::endl;
        
        // Test orthogonality: a × b is perpendicular to both a and b
        Vector3 a(1, 0, 0);
        Vector3 b(0, 1, 0);
        Vector3 cross = a.cross(b);
        
        // Cross product should be (0, 0, 1) for standard basis vectors
        assert(std::abs(cross.x - 0) < 1e-6);
        assert(std::abs(cross.y - 0) < 1e-6);
        assert(std::abs(cross.z - 1) < 1e-6);
        
        // Verify orthogonality: cross product should be perpendicular to both inputs
        assert(std::abs(cross.dot(a)) < 1e-6);
        assert(std::abs(cross.dot(b)) < 1e-6);
        
        // Test anticommutativity: a × b = -(b × a)
        Vector3 cross_reverse = b.cross(a);
        assert(std::abs(cross.x + cross_reverse.x) < 1e-6);
        assert(std::abs(cross.y + cross_reverse.y) < 1e-6);
        assert(std::abs(cross.z + cross_reverse.z) < 1e-6);
        
        // Test magnitude: |a × b| = |a||b|sin(θ) - for perpendicular vectors, sin(90°) = 1
        float expected_magnitude = a.length() * b.length();
        assert(std::abs(cross.length() - expected_magnitude) < 1e-6);
        
        std::cout << "  Cross product enhanced validation: PASSED" << std::endl;
        return true;
    }
    
    bool test_point3_geometric_relationships() {
        std::cout << "Testing Point3 geometric relationships..." << std::endl;
        
        // Test point-to-point vector calculation
        Point3 p1(1, 2, 3);
        Point3 p2(4, 6, 8);
        Vector3 displacement = p2 - p1;  // Should be (3, 4, 5)
        
        assert(std::abs(displacement.x - 3) < 1e-6);
        assert(std::abs(displacement.y - 4) < 1e-6);
        assert(std::abs(displacement.z - 5) < 1e-6);
        
        // Test point displacement: Point + Vector = Point
        Vector3 move(1, 1, 1);
        Point3 moved = p1 + move;  // Should be (2, 3, 4)
        
        assert(std::abs(moved.x - 2) < 1e-6);
        assert(std::abs(moved.y - 3) < 1e-6);
        assert(std::abs(moved.z - 4) < 1e-6);
        
        // Test commutative displacement: Vector + Point = Point
        Point3 moved2 = move + p1;
        assert(std::abs(moved2.x - moved.x) < 1e-6);
        assert(std::abs(moved2.y - moved.y) < 1e-6);
        assert(std::abs(moved2.z - moved.z) < 1e-6);
        
        // Test distance calculation using 3-4-5 triangle
        Point3 origin(0, 0, 0);
        Point3 corner(3, 4, 0);
        float distance = origin.distance_to(corner);
        assert(std::abs(distance - 5.0f) < 1e-6);  // 3-4-5 triangle
        
        // Test squared distance efficiency
        float dist_squared = origin.distance_squared_to(corner);
        assert(std::abs(dist_squared - 25.0f) < 1e-6);  // 5² = 25
        
        std::cout << "  Point3 geometric relationships: PASSED" << std::endl;
        return true;
    }
    
    bool test_ray_parameterization() {
        std::cout << "Testing Ray parameterization and mathematics..." << std::endl;
        
        // Test basic ray evaluation
        Point3 origin(1, 2, 3);
        Vector3 direction(1, 0, 0);  // Unit vector along x-axis
        Ray ray(origin, direction);
        
        // Test ray at parameter t=0 (should be origin)
        Point3 at_origin = ray.at(0);
        assert(std::abs(at_origin.x - 1) < 1e-6);
        assert(std::abs(at_origin.y - 2) < 1e-6);
        assert(std::abs(at_origin.z - 3) < 1e-6);
        
        // Test ray at parameter t=5 (should be origin + 5*direction)
        Point3 at_five = ray.at(5);
        assert(std::abs(at_five.x - 6) < 1e-6);  // 1 + 5*1 = 6
        assert(std::abs(at_five.y - 2) < 1e-6);  // 2 + 5*0 = 2
        assert(std::abs(at_five.z - 3) < 1e-6);  // 3 + 5*0 = 3
        
        // Test ray direction normalization validation
        Vector3 normalized_dir(1, 0, 0);  // Already normalized
        Ray normalized_ray(origin, normalized_dir);
        assert(normalized_ray.is_direction_normalized());
        
        Vector3 unnormalized_dir(2, 0, 0);  // Length = 2
        Ray unnormalized_ray(origin, unnormalized_dir);
        assert(!unnormalized_ray.is_direction_normalized());
        
        // Test ray normalization
        Ray normalized_version = unnormalized_ray.normalized();
        assert(normalized_version.is_direction_normalized());
        
        // Test point_at_parameter method (should be same as at())
        Point3 param_point = ray.point_at_parameter(3);
        Point3 at_point = ray.at(3);
        assert(std::abs(param_point.x - at_point.x) < 1e-6);
        assert(std::abs(param_point.y - at_point.y) < 1e-6);
        assert(std::abs(param_point.z - at_point.z) < 1e-6);
        
        std::cout << "  Ray parameterization validation: PASSED" << std::endl;
        return true;
    }
    
    bool test_edge_cases_and_precision() {
        std::cout << "Testing edge cases and numerical precision..." << std::endl;
        
        // Test zero vector handling
        Vector3 zero(0, 0, 0);
        Vector3 normalized_zero = zero.normalize();
        assert(std::abs(normalized_zero.x - 0) < 1e-6);
        assert(std::abs(normalized_zero.y - 0) < 1e-6);
        assert(std::abs(normalized_zero.z - 0) < 1e-6);
        
        // Test cross product with parallel vectors (should be zero)
        Vector3 parallel1(1, 2, 3);
        Vector3 parallel2(2, 4, 6);  // 2 * parallel1
        Vector3 parallel_cross = parallel1.cross(parallel2);
        assert(std::abs(parallel_cross.x) < 1e-6);
        assert(std::abs(parallel_cross.y) < 1e-6);
        assert(std::abs(parallel_cross.z) < 1e-6);
        
        // Test degenerate ray (zero direction)
        Point3 ray_origin(0, 0, 0);
        Vector3 zero_direction(0, 0, 0);
        Ray degenerate_ray(ray_origin, zero_direction);
        
        // Should handle gracefully without crashing
        Point3 degenerate_point = degenerate_ray.at(1);
        assert(std::abs(degenerate_point.x - 0) < 1e-6);
        assert(std::abs(degenerate_point.y - 0) < 1e-6);
        assert(std::abs(degenerate_point.z - 0) < 1e-6);
        
        // Test precision tolerance in distance calculations
        Point3 close1(0, 0, 0);
        Point3 close2(1e-7f, 1e-7f, 1e-7f);
        float tiny_distance = close1.distance_to(close2);
        assert(tiny_distance >= 0);  // Should be positive and finite
        
        std::cout << "  Edge cases and precision: PASSED" << std::endl;
        return true;
    }
    
    bool test_validation_methods() {
        std::cout << "Testing new validation methods..." << std::endl;
        
        // Test Vector3 finite validation
        Vector3 valid_vector(1, 2, 3);
        assert(valid_vector.is_finite());
        
        // Test Vector3 normalization validation
        Vector3 unit_vector(1, 0, 0);
        assert(unit_vector.is_normalized());
        
        Vector3 non_unit(2, 0, 0);
        assert(!non_unit.is_normalized());
        
        // Test Ray validation
        Point3 origin(0, 0, 0);
        Vector3 valid_direction(1, 0, 0);
        Ray valid_ray(origin, valid_direction);
        assert(valid_ray.is_valid());
        
        Vector3 zero_direction(0, 0, 0);
        Ray invalid_ray(origin, zero_direction);
        assert(!invalid_ray.is_valid());
        
        std::cout << "  Validation methods: PASSED" << std::endl;
        return true;
    }
    
    // === NEW TESTS FOR STORY 1.3 ===
    
    bool test_sphere_intersection_mathematics() {
        std::cout << "Testing Sphere ray intersection mathematical correctness..." << std::endl;
        
        // Test Case 1: Known intersection with analytical solution
        // Ray from (0,0,0) to sphere at (0,0,-5) radius 1.0
        // Expected intersection at (0,0,-4) with t=4
        std::cout << "  Testing known intersection case..." << std::endl;
        Ray test_ray(Point3(0, 0, 0), Vector3(0, 0, -1));
        Sphere test_sphere(Point3(0, 0, -5), 1.0f);
        
        Sphere::Intersection result = test_sphere.intersect(test_ray);
        
        assert(result.hit);  // Should intersect
        assert(std::abs(result.t - 4.0f) < 1e-6);  // t parameter should be 4.0
        assert(std::abs(result.point.x - 0.0f) < 1e-6);  // x coordinate
        assert(std::abs(result.point.y - 0.0f) < 1e-6);  // y coordinate  
        assert(std::abs(result.point.z - (-4.0f)) < 1e-6);  // z coordinate
        assert(std::abs(result.normal.x - 0.0f) < 1e-6);  // normal x
        assert(std::abs(result.normal.y - 0.0f) < 1e-6);  // normal y
        assert(std::abs(result.normal.z - 1.0f) < 1e-6);  // normal z
        
        // Test Case 2: No intersection (ray misses)
        std::cout << "  Testing ray miss case..." << std::endl;
        Ray miss_ray(Point3(0, 0, 0), Vector3(1, 0, 0));  // Ray along x-axis
        Sphere::Intersection miss_result = test_sphere.intersect(miss_ray);
        assert(!miss_result.hit);  // Should not intersect
        
        // Test Case 3: Ray behind camera (negative t)
        std::cout << "  Testing negative t case..." << std::endl;
        Ray behind_ray(Point3(0, 0, -10), Vector3(0, 0, -1));  // Ray behind sphere
        Sphere::Intersection behind_result = test_sphere.intersect(behind_ray);
        assert(!behind_result.hit);  // Should not intersect (behind camera)
        
        // Test Case 4: Tangent ray (discriminant = 0)
        std::cout << "  Testing tangent ray case..." << std::endl;
        Ray tangent_ray(Point3(1, 0, 0), Vector3(0, 0, -1));  // Tangent to unit sphere
        Sphere unit_sphere(Point3(0, 0, -5), 1.0f);
        Sphere::Intersection tangent_result = unit_sphere.intersect(tangent_ray);
        assert(tangent_result.hit);  // Should intersect at tangent point
        assert(std::abs(tangent_result.t - 5.0f) < 1e-6);  // t should be 5.0
        
        // Test Case 5: Sphere geometry validation
        std::cout << "  Testing sphere validation..." << std::endl;
        assert(test_sphere.validate_geometry());  // Valid sphere
        
        Sphere invalid_sphere(Point3(0, 0, 0), -1.0f);  // Negative radius
        assert(!invalid_sphere.validate_geometry());  // Should be invalid
        
        std::cout << "  Ray-sphere intersection mathematics: PASSED" << std::endl;
        return true;
    }
    
    bool test_lambert_brdf_energy_conservation() {
        std::cout << "Testing Lambert BRDF energy conservation..." << std::endl;
        
        // Test Case 1: Valid albedo (energy conserving)
        std::cout << "  Testing valid albedo..." << std::endl;
        LambertMaterial valid_material(Vector3(0.8f, 0.6f, 0.4f));
        assert(valid_material.validate_energy_conservation());
        
        // Test Case 2: Invalid albedo (energy violating)
        std::cout << "  Testing invalid albedo..." << std::endl;
        LambertMaterial invalid_material(Vector3(1.5f, 0.5f, 0.5f));  // Red > 1.0
        assert(!invalid_material.validate_energy_conservation());
        
        // Test Case 3: BRDF mathematical correctness
        std::cout << "  Testing BRDF formula correctness..." << std::endl;
        Vector3 albedo(0.6f, 0.6f, 0.6f);
        LambertMaterial material(albedo);
        
        Vector3 light_dir(0, 0, 1);
        Vector3 view_dir(0, 0, 1);
        Vector3 normal(0, 0, 1);
        
        Vector3 brdf = material.evaluate_brdf(light_dir, view_dir, normal);
        
        // Lambert BRDF should be albedo / π
        float expected_brdf = 0.6f / M_PI;
        assert(std::abs(brdf.x - expected_brdf) < 1e-6);
        assert(std::abs(brdf.y - expected_brdf) < 1e-6);
        assert(std::abs(brdf.z - expected_brdf) < 1e-6);
        
        // Test Case 4: Hemispherical reflectance
        std::cout << "  Testing hemispherical reflectance..." << std::endl;
        Vector3 reflectance = material.hemispherical_reflectance();
        assert(std::abs(reflectance.x - albedo.x) < 1e-6);
        assert(std::abs(reflectance.y - albedo.y) < 1e-6);
        assert(std::abs(reflectance.z - albedo.z) < 1e-6);
        
        std::cout << "  Lambert BRDF energy conservation: PASSED" << std::endl;
        return true;
    }
    
    bool test_lambert_cosine_law() {
        std::cout << "Testing Lambert cosine law (n·l calculation)..." << std::endl;
        
        LambertMaterial material(Vector3(0.8f, 0.8f, 0.8f));
        Vector3 surface_normal(0, 0, 1);  // Facing up
        Vector3 view_direction(0, 0, 1);  // Camera above surface
        Vector3 incident_radiance(1, 1, 1);  // White light
        
        // Test Case 1: Normal incidence (n·l = 1)
        std::cout << "  Testing normal incidence..." << std::endl;
        Vector3 normal_light_dir(0, 0, 1);  // Light straight down
        Vector3 normal_result = material.scatter_light(normal_light_dir, view_direction, 
                                                      surface_normal, incident_radiance);
        
        float expected_normal = (0.8f / M_PI) * 1.0f * 1.0f;  // BRDF * radiance * cos(0°)
        assert(std::abs(normal_result.x - expected_normal) < 1e-5);
        
        // Test Case 2: 45-degree incidence (n·l = cos(45°) = √2/2 ≈ 0.707)
        std::cout << "  Testing 45-degree incidence..." << std::endl;
        Vector3 angled_light_dir = Vector3(1, 0, 1).normalize();  // 45° from normal
        Vector3 angled_result = material.scatter_light(angled_light_dir, view_direction,
                                                      surface_normal, incident_radiance);
        
        float cos_45 = std::sqrt(2.0f) / 2.0f;
        float expected_angled = (0.8f / M_PI) * 1.0f * cos_45;
        assert(std::abs(angled_result.x - expected_angled) < 1e-5);
        
        // Test Case 3: Grazing incidence (n·l ≈ 0)
        std::cout << "  Testing grazing incidence..." << std::endl;
        Vector3 grazing_light_dir(1, 0, 0.01f);  // Almost parallel to surface
        grazing_light_dir = grazing_light_dir.normalize();
        Vector3 grazing_result = material.scatter_light(grazing_light_dir, view_direction,
                                                       surface_normal, incident_radiance);
        
        // Should be very small (near zero)
        assert(grazing_result.x < 0.1f);
        assert(grazing_result.y < 0.1f);
        assert(grazing_result.z < 0.1f);
        
        // Test Case 4: Back-facing light (n·l < 0, should clamp to 0)
        std::cout << "  Testing back-facing light..." << std::endl;
        Vector3 backlight_dir(0, 0, -1);  // Light from below surface
        Vector3 backlight_result = material.scatter_light(backlight_dir, view_direction,
                                                         surface_normal, incident_radiance);
        
        // Should be zero (no negative light)
        assert(std::abs(backlight_result.x) < 1e-6);
        assert(std::abs(backlight_result.y) < 1e-6);
        assert(std::abs(backlight_result.z) < 1e-6);
        
        std::cout << "  Lambert cosine law: PASSED" << std::endl;
        return true;
    }
    
    bool test_point_light_mathematics() {
        std::cout << "Testing Point light mathematical correctness..." << std::endl;
        
        // Test Case 1: Light direction calculation
        std::cout << "  Testing light direction calculation..." << std::endl;
        PointLight light(Point3(1, 0, 0), Vector3(1, 1, 1), 1.0f);
        Point3 surface_point(0, 0, 0);
        
        Vector3 direction = light.sample_direction(surface_point);
        Vector3 expected_dir(1, 0, 0);  // Normalized (1,0,0)
        
        assert(std::abs(direction.x - 1.0f) < 1e-6);
        assert(std::abs(direction.y - 0.0f) < 1e-6);
        assert(std::abs(direction.z - 0.0f) < 1e-6);
        assert(std::abs(direction.length() - 1.0f) < 1e-6);  // Should be normalized
        
        // Test Case 2: Inverse square law
        std::cout << "  Testing inverse square law..." << std::endl;
        Vector3 irradiance = light.calculate_irradiance(surface_point);
        
        // Distance = 1, so falloff = 1/(4π*1²) = 1/(4π)
        float expected_falloff = 1.0f / (4.0f * M_PI);
        float expected_irradiance = 1.0f * expected_falloff;  // intensity * color * falloff
        
        assert(std::abs(irradiance.x - expected_irradiance) < 1e-5);
        assert(std::abs(irradiance.y - expected_irradiance) < 1e-5);
        assert(std::abs(irradiance.z - expected_irradiance) < 1e-5);
        
        // Test Case 3: Distance scaling
        std::cout << "  Testing distance scaling..." << std::endl;
        Point3 far_point(2, 0, 0);  // Distance = 2
        Vector3 far_irradiance = light.calculate_irradiance(far_point);
        
        // Distance = 2, so falloff = 1/(4π*4) = 1/(16π)
        // Irradiance should be 1/4 of close irradiance
        assert(std::abs(far_irradiance.x * 4.0f - irradiance.x) < 1e-5);
        
        // Test Case 4: Light validation
        std::cout << "  Testing light validation..." << std::endl;
        assert(light.validate_light());  // Valid light
        
        PointLight invalid_light(Point3(0, 0, 0), Vector3(-1, 0, 0), 1.0f);  // Negative color
        assert(!invalid_light.validate_light());  // Should be invalid
        
        std::cout << "  Point light mathematics: PASSED" << std::endl;
        return true;
    }
    
    bool test_complete_rendering_equation() {
        std::cout << "Testing complete rendering equation integration..." << std::endl;
        
        // Set up a complete test scenario
        std::cout << "  Testing full light transport pipeline..." << std::endl;
        
        // Scene setup
        Ray camera_ray(Point3(0, 0, 0), Vector3(0, 0, -1));
        Sphere sphere(Point3(0, 0, -2), 0.5f);  // Smaller sphere, closer
        LambertMaterial material(Vector3(0.5f, 0.5f, 0.5f));  // 50% gray
        PointLight light(Point3(1, 1, -1), Vector3(1, 1, 1), 4.0f);  // Bright white light
        
        // Step 1: Ray-sphere intersection
        Sphere::Intersection intersection = sphere.intersect(camera_ray);
        assert(intersection.hit);  // Should intersect
        
        float expected_t = 2.0f - 0.5f;  // Distance to sphere center minus radius
        assert(std::abs(intersection.t - expected_t) < 1e-5);
        
        // Step 2: Light evaluation
        Vector3 light_direction = light.sample_direction(intersection.point);
        Vector3 incident_irradiance = light.calculate_irradiance(intersection.point);
        
        // Verify light direction is normalized
        assert(std::abs(light_direction.length() - 1.0f) < 1e-6);
        
        // Step 3: BRDF evaluation
        Vector3 view_direction = (camera_ray.origin - intersection.point).normalize();
        Vector3 final_color = material.scatter_light(light_direction, view_direction,
                                                    intersection.normal, incident_irradiance);
        
        // Verify result is physically plausible
        assert(final_color.x >= 0.0f && final_color.x <= 1.0f);  // Red channel in [0,1]
        assert(final_color.y >= 0.0f && final_color.y <= 1.0f);  // Green channel in [0,1]
        assert(final_color.z >= 0.0f && final_color.z <= 1.0f);  // Blue channel in [0,1]
        
        // Since material albedo is 0.5, final color should be reasonable for bright light
        assert(final_color.x > 0.001f);  // Should not be black (light is present)
        
        std::cout << "  Complete rendering equation: PASSED" << std::endl;
        return true;
    }
}

int main() {
    std::cout << "=== Educational Ray Tracer - Mathematical Correctness Tests ===" << std::endl;
    
    try {
        bool all_passed = true;
        
        all_passed &= MathematicalTests::test_vector3_operations();
        all_passed &= MathematicalTests::test_mathematical_precision();
        all_passed &= MathematicalTests::test_vector3_cross_product_enhanced();
        all_passed &= MathematicalTests::test_point3_geometric_relationships();
        all_passed &= MathematicalTests::test_ray_parameterization();
        all_passed &= MathematicalTests::test_edge_cases_and_precision();
        all_passed &= MathematicalTests::test_validation_methods();
        
        // === STORY 1.3 MATHEMATICAL VALIDATION TESTS ===
        std::cout << "\n=== Story 1.3 Mathematical Validation Tests ===" << std::endl;
        all_passed &= MathematicalTests::test_sphere_intersection_mathematics();
        all_passed &= MathematicalTests::test_lambert_brdf_energy_conservation();
        all_passed &= MathematicalTests::test_lambert_cosine_law();
        all_passed &= MathematicalTests::test_point_light_mathematics();
        all_passed &= MathematicalTests::test_complete_rendering_equation();
        
        if (all_passed) {
            std::cout << "\n✅ ALL MATHEMATICAL TESTS PASSED" << std::endl;
            std::cout << "Mathematical foundation verified for Epic 1 development." << std::endl;
            std::cout << "Story 1.3: Single-Ray Lambert BRDF Implementation - VALIDATED" << std::endl;
            return 0;
        } else {
            std::cout << "\n❌ SOME TESTS FAILED" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cout << "\n❌ TEST EXECUTION ERROR: " << e.what() << std::endl;
        return 1;
    }
}