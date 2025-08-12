# Testing Strategy

The testing approach focuses on **mathematical correctness validation** and **educational learning verification** rather than traditional software testing patterns.

## Educational Testing Pyramid

```
              Learning Validation Tests
                 /              \
         Mathematical Tests    Visual Tests
        /                    \              \
   Unit Tests         Integration Tests   Performance Tests
```

## Core Testing Framework

### Mathematical Correctness Testing
```cpp
// tests/test_math_correctness.cpp - Primary educational validation
#include "src/core/vector3.hpp"
#include "src/core/material.hpp"
#include "src/core/scene.hpp"

namespace MathematicalTests {
    
    // Epic 1: Foundation mathematics validation
    bool test_vector3_operations() {
        Vector3 a(1, 2, 3);
        Vector3 b(4, 5, 6);
        
        // Validate dot product calculation
        float dot_result = a.dot(b);
        float expected_dot = 1*4 + 2*5 + 3*6; // 32
        assert(abs(dot_result - expected_dot) < 1e-6);
        
        // Validate normalization
        Vector3 normalized = a.normalized();
        assert(abs(normalized.length() - 1.0f) < 1e-6);
        
        return true;
    }
    
    bool test_ray_sphere_intersection() {
        // Validate quadratic equation solution for ray-sphere intersection
        Ray ray(Vector3(0, 0, 0), Vector3(0, 0, -1));
        Vector3 sphere_center(0, 0, -5);
        float sphere_radius = 1.0f;
        
        // Calculate intersection using student's implementation
        Scene scene;
        // Add sphere to scene...
        Scene::Intersection hit = scene.intersect(ray);
        
        // Validate mathematical correctness
        assert(hit.hit == true);
        assert(abs(hit.t - 4.0f) < 1e-6); // Expected t = 5 - 1 = 4
        
        return true;
    }
    
    // Epic 3: BRDF energy conservation validation
    bool test_cook_torrance_energy_conservation() {
        Material material(Vector3(0.7f, 0.3f, 0.3f), 0.5f, MaterialType::CookTorrance);
        
        // Integrate BRDF over hemisphere - should not exceed 1.0 (energy conservation)
        float total_energy = 0.0f;
        int samples = 10000;
        
        for (int i = 0; i < samples; i++) {
            // Generate random hemisphere directions
            Vector3 wi = generate_random_hemisphere_direction();
            Vector3 wo = Vector3(0, 0, 1); // Fixed outgoing direction
            Vector3 normal(0, 0, 1);
            
            Vector3 brdf_value = material.evaluate_brdf(wi, wo, normal);
            float cos_theta = normal.dot(wi);
            total_energy += brdf_value.length() * cos_theta * (2 * M_PI / samples);
        }
        
        // Energy conservation: total energy should not exceed 1.0
        assert(total_energy <= 1.01f); // Small tolerance for numerical precision
        
        return true;
    }
}
```
