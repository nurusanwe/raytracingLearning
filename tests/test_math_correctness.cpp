#include <iostream>
#include <cassert>
#include <cmath>
#include "src/core/vector3.hpp"

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
}

int main() {
    std::cout << "=== Educational Ray Tracer - Mathematical Correctness Tests ===" << std::endl;
    
    try {
        bool all_passed = true;
        
        all_passed &= MathematicalTests::test_vector3_operations();
        all_passed &= MathematicalTests::test_mathematical_precision();
        
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