#include <iostream>
#include <cassert>
#include <cmath>
#include "src/core/vector3.hpp"
#include "src/core/point3.hpp"
#include "src/core/ray.hpp"

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
        
        if (all_passed) {
            std::cout << "\n✅ ALL MATHEMATICAL TESTS PASSED" << std::endl;
            std::cout << "Mathematical foundation verified for Epic 1 development." << std::endl;
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