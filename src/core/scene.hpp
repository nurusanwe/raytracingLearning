#pragma once
#include "vector3.hpp"
#include "point3.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "../materials/lambert.hpp"
#include "../materials/cook_torrance.hpp"
#include "../materials/material_base.hpp"
#include "../lights/light_base.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include <chrono>
#include <limits>

// Scene class manages multiple primitive objects and materials for ray tracing
// Educational focus: demonstrates ray-scene intersection algorithms and performance monitoring
// Architecture: container-based approach with efficient closest-hit logic and statistics tracking
class Scene {
public:
    // Container for geometric primitives in the scene
    std::vector<Sphere> primitives;
    
    // Container for polymorphic materials with index-based referencing from primitives  
    // Supports both Lambert and Cook-Torrance materials through Material base class polymorphism
    std::vector<std::unique_ptr<Material>> materials;
    
    // Container for polymorphic lights with multiple light type support
    // Supports Point, Directional, and Area lights through Light base class polymorphism
    std::vector<std::unique_ptr<Light>> lights;
    
    // Educational performance monitoring for intersection statistics
    mutable int total_intersection_tests = 0;
    mutable int successful_intersections = 0;
    mutable float total_intersection_time_ms = 0.0f;

    // Default constructor creates empty scene
    Scene() = default;

    // Intersection result structure containing complete hit information
    // Provides all necessary data for rendering: geometry, material, and surface properties
    struct Intersection {
        bool hit;                           // Whether ray intersects any primitive
        float t;                           // Ray parameter at closest intersection
        Point3 point;                      // 3D coordinates of intersection point
        Vector3 normal;                    // Outward-pointing surface normal
        const Material* material;          // Polymorphic material properties at intersection
        const Sphere* primitive;           // Primitive object that was hit
        
        // Default constructor for no-intersection case
        Intersection() 
            : hit(false), t(0.0f), point(Point3()), normal(Vector3()), 
              material(nullptr), primitive(nullptr) {}
              
        // Constructor for valid intersection with complete information
        Intersection(float t_value, const Point3& hit_point, const Vector3& surface_normal,
                    const Material* hit_material, const Sphere* hit_primitive)
            : hit(true), t(t_value), point(hit_point), normal(surface_normal),
              material(hit_material), primitive(hit_primitive) {}
    };

    // Ray-scene intersection with closest-hit logic and educational monitoring
    // Algorithm: iterate through all primitives, track closest intersection with t-value comparison
    // Educational features: performance statistics, detailed console output for learning
    // Returns: complete intersection information including material and primitive references
    Intersection intersect(const Ray& ray, bool verbose = true) const {
        if (verbose) {
            std::cout << "\n=== Ray-Scene Intersection Testing ===" << std::endl;
            std::cout << "Ray origin: (" << ray.origin.x << ", " << ray.origin.y << ", " << ray.origin.z << ")" << std::endl;
            std::cout << "Ray direction: (" << ray.direction.x << ", " << ray.direction.y << ", " << ray.direction.z << ")" << std::endl;
            std::cout << "Scene primitives: " << primitives.size() << " spheres" << std::endl;
        }

        // Start timing for educational performance monitoring
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Initialize closest intersection tracking
        Intersection closest_hit;
        closest_hit.hit = false;
        closest_hit.t = std::numeric_limits<float>::max();
        
        int current_test_count = 0;
        int current_hit_count = 0;

        // Test intersection with each primitive in scene
        for (size_t i = 0; i < primitives.size(); ++i) {
            const Sphere& sphere = primitives[i];
            current_test_count++;
            total_intersection_tests++;
            
            if (verbose) {
                std::cout << "\nTesting sphere " << i << ":" << std::endl;
                std::cout << "  Center: (" << sphere.center.x << ", " << sphere.center.y << ", " << sphere.center.z << ")" << std::endl;
                std::cout << "  Radius: " << sphere.radius << std::endl;
                std::cout << "  Material index: " << sphere.material_index << std::endl;
            }

            // Perform ray-sphere intersection test
            Sphere::Intersection sphere_hit = sphere.intersect(ray, verbose);
            
            if (sphere_hit.hit) {
                current_hit_count++;
                if (verbose) {
                    std::cout << "  HIT at t = " << sphere_hit.t << std::endl;
                }
                
                // Check if this is the closest intersection so far
                if (sphere_hit.t > 0.001f && sphere_hit.t < closest_hit.t) {
                    successful_intersections++;
                    if (verbose) {
                        std::cout << "  NEW CLOSEST HIT (previous closest t = " << closest_hit.t << ")" << std::endl;
                    }
                    
                    // Validate material index bounds
                    if (sphere.material_index >= 0 && sphere.material_index < static_cast<int>(materials.size())) {
                        closest_hit.hit = true;
                        closest_hit.t = sphere_hit.t;
                        closest_hit.point = sphere_hit.point;
                        closest_hit.normal = sphere_hit.normal;
                        closest_hit.material = materials[sphere.material_index].get();
                        closest_hit.primitive = &sphere;
                        
                        if (verbose) {
                            std::cout << "  Material assigned: " << sphere.material_index << std::endl;
                        }
                    } else {
                        if (verbose) {
                            std::cout << "  ERROR: Invalid material index " << sphere.material_index 
                                      << " (valid range: 0-" << (materials.size()-1) << ")" << std::endl;
                        }
                    }
                } else if (sphere_hit.t <= 0.001f) {
                    if (verbose) {
                        std::cout << "  REJECTED: t too small (self-intersection avoidance)" << std::endl;
                    }
                } else {
                    if (verbose) {
                        std::cout << "  REJECTED: farther than current closest hit" << std::endl;
                    }
                }
            } else {
                if (verbose) {
                    std::cout << "  MISS" << std::endl;
                }
            }
        }

        // Calculate timing for educational performance monitoring
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        float intersection_time = duration.count() / 1000.0f; // Convert to milliseconds
        total_intersection_time_ms += intersection_time;

        // Educational performance statistics output
        if (verbose) {
            std::cout << "\n=== Intersection Performance Statistics ===" << std::endl;
            std::cout << "Current ray tests: " << current_test_count << std::endl;
            std::cout << "Current ray hits: " << current_hit_count << std::endl;
            std::cout << "Current ray hit rate: " << (current_test_count > 0 ? 
                         (float)current_hit_count / current_test_count * 100.0f : 0.0f) << "%" << std::endl;
            std::cout << "Current ray test time: " << intersection_time << "ms" << std::endl;
            
            std::cout << "\nCumulative statistics:" << std::endl;
            std::cout << "Total intersection tests: " << total_intersection_tests << std::endl;
            std::cout << "Total successful intersections: " << successful_intersections << std::endl;
            std::cout << "Overall hit rate: " << (total_intersection_tests > 0 ? 
                         (float)successful_intersections / total_intersection_tests * 100.0f : 0.0f) << "%" << std::endl;
            std::cout << "Average test time: " << (total_intersection_tests > 0 ? 
                         total_intersection_time_ms / total_intersection_tests : 0.0f) << "ms" << std::endl;
        }

        if (verbose) {
            if (closest_hit.hit) {
                std::cout << "\n=== Final Closest Hit Result ===" << std::endl;
                std::cout << "Hit point: (" << closest_hit.point.x << ", " << closest_hit.point.y << ", " << closest_hit.point.z << ")" << std::endl;
                std::cout << "Surface normal: (" << closest_hit.normal.x << ", " << closest_hit.normal.y << ", " << closest_hit.normal.z << ")" << std::endl;
                std::cout << "Distance: t = " << closest_hit.t << std::endl;
                if (closest_hit.material) {
                    std::cout << "Material color: (" << closest_hit.material->base_color.x << ", " 
                             << closest_hit.material->base_color.y << ", " << closest_hit.material->base_color.z << ")" << std::endl;
                }
            } else {
                std::cout << "\n=== No Intersection Found ===" << std::endl;
            }
            
            std::cout << "=== Ray-scene intersection complete ===" << std::endl;
        }
        return closest_hit;
    }

    // Add polymorphic material to scene and return its index for primitive referencing
    // Educational transparency: reports material assignment and validates parameters
    // Supports both Lambert and Cook-Torrance materials through Material base class polymorphism
    int add_material(std::unique_ptr<Material> material) {
        std::cout << "\n=== Adding Polymorphic Material to Scene ===" << std::endl;
        
        if (!material) {
            std::cout << "ERROR: Null material pointer" << std::endl;
            return -1;
        }
        
        // Validate material parameters before adding
        if (!material->validate_parameters()) {
            std::cout << "WARNING: Material parameters outside valid ranges" << std::endl;
            std::cout << "Educational note: Invalid parameters may cause non-physical behavior" << std::endl;
            material->clamp_to_valid_ranges();
            std::cout << "Parameters automatically clamped to valid ranges" << std::endl;
        }
        
        // Educational material type reporting
        std::cout << "Material Type: " << material->material_type_name() << std::endl;
        std::cout << "Base Color: (" << material->base_color.x << ", " 
                 << material->base_color.y << ", " << material->base_color.z << ")" << std::endl;
        
        // Additional details for specific material types
        if (material->type == MaterialType::CookTorrance) {
            // Safe cast for Cook-Torrance specific information
            CookTorranceMaterial* ct_material = static_cast<CookTorranceMaterial*>(material.get());
            std::cout << "Cook-Torrance Parameters:" << std::endl;
            std::cout << "  Roughness: " << ct_material->roughness << std::endl;
            std::cout << "  Metallic: " << ct_material->metallic << std::endl;
            std::cout << "  Specular: " << ct_material->specular << std::endl;
        }
        
        materials.push_back(std::move(material));
        int material_index = static_cast<int>(materials.size() - 1);
        
        std::cout << "Material added at index: " << material_index << std::endl;
        std::cout << "Total materials in scene: " << materials.size() << std::endl;
        std::cout << "=== Material addition complete ===" << std::endl;
        
        return material_index;
    }

    // Legacy add_material method for backward compatibility with existing Lambert code
    // Wraps Lambert material in unique_ptr and delegates to polymorphic method
    int add_material(const LambertMaterial& material) {
        auto lambert_material = std::make_unique<LambertMaterial>(material);
        return add_material(std::move(lambert_material));
    }

    // Add polymorphic light to scene and return its index
    // Educational transparency: reports light assignment and validates parameters
    // Supports Point, Directional, and Area lights through Light base class polymorphism
    int add_light(std::unique_ptr<Light> light) {
        std::cout << "\n=== Adding Polymorphic Light to Scene ===" << std::endl;
        
        if (!light) {
            std::cout << "ERROR: Null light pointer" << std::endl;
            return -1;
        }
        
        // Validate light parameters before adding
        if (!light->validate_parameters()) {
            std::cout << "WARNING: Light parameters outside valid ranges" << std::endl;
            std::cout << "Educational note: Invalid parameters may cause non-physical behavior" << std::endl;
            light->clamp_parameters();
            std::cout << "Parameters automatically clamped to valid ranges" << std::endl;
        }
        
        // Educational light type reporting
        std::cout << "Light Info: " << light->get_light_info() << std::endl;
        std::cout << "Light Color: (" << light->color.x << ", " 
                 << light->color.y << ", " << light->color.z << ")" << std::endl;
        std::cout << "Light Intensity: " << light->intensity << " (dimensionless multiplier)" << std::endl;
        
        // Additional details for specific light types
        std::string light_type_name;
        switch (light->type) {
            case LightType::Point:
                light_type_name = "Point Light";
                break;
            case LightType::Directional:
                light_type_name = "Directional Light";
                break;
            case LightType::Area:
                light_type_name = "Area Light";
                break;
        }
        std::cout << "Light Type: " << light_type_name << std::endl;
        
        lights.push_back(std::move(light));
        int light_index = static_cast<int>(lights.size() - 1);
        
        std::cout << "Light added at index: " << light_index << std::endl;
        std::cout << "Total lights in scene: " << lights.size() << std::endl;
        std::cout << "=== Light addition complete ===" << std::endl;
        
        return light_index;
    }

    // Add sphere primitive to scene and return its index
    // Validates sphere geometry and material index before adding to scene
    int add_sphere(const Sphere& sphere) {
        std::cout << "\n=== Adding Sphere to Scene ===" << std::endl;
        
        // Validate sphere geometry
        if (!sphere.validate_geometry()) {
            std::cout << "ERROR: Invalid sphere geometry, not adding to scene" << std::endl;
            return -1;
        }
        
        // Validate material index reference
        if (sphere.material_index < 0 || sphere.material_index >= static_cast<int>(materials.size())) {
            std::cout << "ERROR: Invalid material index " << sphere.material_index 
                     << " (valid range: 0-" << (materials.size()-1) << ")" << std::endl;
            return -1;
        }
        
        primitives.push_back(sphere);
        int sphere_index = static_cast<int>(primitives.size() - 1);
        
        std::cout << "Sphere added at index: " << sphere_index << std::endl;
        std::cout << "Sphere center: (" << sphere.center.x << ", " << sphere.center.y << ", " << sphere.center.z << ")" << std::endl;
        std::cout << "Sphere radius: " << sphere.radius << std::endl;
        std::cout << "Material reference: " << sphere.material_index << std::endl;
        std::cout << "Total spheres in scene: " << primitives.size() << std::endl;
        
        return sphere_index;
    }

    // Educational method: explain intersection process for learning purposes
    // Demonstrates ray-scene intersection algorithm step-by-step without performing actual intersection
    void explain_intersection_process(const Ray& ray) const {
        std::cout << "\n=== Educational: Ray-Scene Intersection Process ===" << std::endl;
        std::cout << "Algorithm Overview:" << std::endl;
        std::cout << "1. Initialize closest_hit with t = infinity" << std::endl;
        std::cout << "2. For each primitive in scene:" << std::endl;
        std::cout << "   a. Test ray-primitive intersection" << std::endl;
        std::cout << "   b. If hit and t > 0.001 and t < closest_t:" << std::endl;
        std::cout << "      - Update closest_hit information" << std::endl;
        std::cout << "      - Store material and primitive references" << std::endl;
        std::cout << "3. Return closest intersection or no-hit result" << std::endl;
        
        std::cout << "\nCurrent scene contents:" << std::endl;
        std::cout << "Primitives: " << primitives.size() << " spheres" << std::endl;
        std::cout << "Materials: " << materials.size() << " materials" << std::endl;
        
        std::cout << "\nExpected intersection tests for this ray: " << primitives.size() << std::endl;
        std::cout << "Self-intersection threshold: t > 0.001" << std::endl;
        std::cout << "=== Process explanation complete ===" << std::endl;
    }

    // Print comprehensive scene statistics for educational and debugging purposes
    void print_scene_statistics() const {
        std::cout << "\n=== Scene Statistics ===" << std::endl;
        std::cout << "Geometry:" << std::endl;
        std::cout << "  Spheres: " << primitives.size() << std::endl;
        std::cout << "  Materials: " << materials.size() << std::endl;
        
        std::cout << "\nPerformance Statistics:" << std::endl;
        std::cout << "  Total intersection tests: " << total_intersection_tests << std::endl;
        std::cout << "  Successful intersections: " << successful_intersections << std::endl;
        std::cout << "  Hit rate: " << (total_intersection_tests > 0 ? 
                     (float)successful_intersections / total_intersection_tests * 100.0f : 0.0f) << "%" << std::endl;
        std::cout << "  Total intersection time: " << total_intersection_time_ms << "ms" << std::endl;
        std::cout << "  Average time per test: " << (total_intersection_tests > 0 ? 
                     total_intersection_time_ms / total_intersection_tests : 0.0f) << "ms" << std::endl;
        
        if (primitives.size() > 0) {
            std::cout << "\nSphere Details:" << std::endl;
            for (size_t i = 0; i < primitives.size(); ++i) {
                const Sphere& sphere = primitives[i];
                std::cout << "  Sphere " << i << ": center(" << sphere.center.x << "," << sphere.center.y << "," << sphere.center.z 
                         << "), radius=" << sphere.radius << ", material=" << sphere.material_index << std::endl;
            }
        }
        
        if (materials.size() > 0) {
            std::cout << "\nMaterial Details:" << std::endl;
            for (size_t i = 0; i < materials.size(); ++i) {
                const Material* material = materials[i].get();
                std::cout << "  Material " << i << ": " << material->material_type_name() 
                         << " - base_color(" << material->base_color.x << "," 
                         << material->base_color.y << "," << material->base_color.z << ")" << std::endl;
                
                // Additional details for Cook-Torrance materials
                if (material->type == MaterialType::CookTorrance) {
                    const CookTorranceMaterial* ct_material = static_cast<const CookTorranceMaterial*>(material);
                    std::cout << "    Roughness: " << ct_material->roughness 
                             << ", Metallic: " << ct_material->metallic 
                             << ", Specular: " << ct_material->specular << std::endl;
                }
            }
        }
        
        std::cout << "=== Scene statistics complete ===" << std::endl;
    }

    // Reset performance monitoring statistics for new measurement periods
    void reset_statistics() const {
        total_intersection_tests = 0;
        successful_intersections = 0;
        total_intersection_time_ms = 0.0f;
        std::cout << "Scene performance statistics reset" << std::endl;
    }

    // Memory usage monitoring methods for educational analysis
    
    // Calculate total memory usage of scene data (primitives and materials)
    size_t calculate_scene_memory_usage() const {
        size_t total_bytes = 0;
        
        // Memory used by sphere primitives
        total_bytes += primitives.size() * sizeof(Sphere);
        
        // Memory used by polymorphic materials
        for (const auto& material : materials) {
            if (material->type == MaterialType::Lambert) {
                total_bytes += sizeof(LambertMaterial);
            } else if (material->type == MaterialType::CookTorrance) {
                total_bytes += sizeof(CookTorranceMaterial);
            } else {
                // Default to base Material size for unknown types
                total_bytes += sizeof(Material);
            }
        }
        
        // Memory used by unique_ptr containers
        total_bytes += materials.size() * sizeof(std::unique_ptr<Material>);
        
        // Memory used by containers (approximate overhead)
        total_bytes += (primitives.capacity() - primitives.size()) * sizeof(Sphere);
        total_bytes += (materials.capacity() - materials.size()) * sizeof(std::unique_ptr<Material>);
        
        return total_bytes;
    }
    
    // Print comprehensive memory usage analysis with educational explanations
    void print_memory_usage_analysis() const {
        std::cout << "\n=== Scene Memory Usage Analysis ===" << std::endl;
        
        size_t sphere_memory = primitives.size() * sizeof(Sphere);
        
        // Calculate polymorphic material memory
        size_t material_memory = 0;
        size_t lambert_count = 0;
        size_t cook_torrance_count = 0;
        for (const auto& material : materials) {
            if (material->type == MaterialType::Lambert) {
                material_memory += sizeof(LambertMaterial);
                lambert_count++;
            } else if (material->type == MaterialType::CookTorrance) {
                material_memory += sizeof(CookTorranceMaterial);
                cook_torrance_count++;
            } else {
                material_memory += sizeof(Material);
            }
        }
        
        size_t container_overhead = (primitives.capacity() - primitives.size()) * sizeof(Sphere) +
                                   (materials.capacity() - materials.size()) * sizeof(std::unique_ptr<Material>);
        size_t total_scene_memory = calculate_scene_memory_usage();
        
        std::cout << "Scene Data Memory Breakdown:" << std::endl;
        std::cout << "  Spheres: " << primitives.size() << " × " << sizeof(Sphere) 
                  << " bytes = " << sphere_memory << " bytes" << std::endl;
        std::cout << "  Materials: " << materials.size() << " total (" 
                  << lambert_count << " Lambert, " << cook_torrance_count << " Cook-Torrance)" << std::endl;
        std::cout << "    Lambert: " << lambert_count << " × " << sizeof(LambertMaterial) 
                  << " bytes = " << (lambert_count * sizeof(LambertMaterial)) << " bytes" << std::endl;
        std::cout << "    Cook-Torrance: " << cook_torrance_count << " × " << sizeof(CookTorranceMaterial) 
                  << " bytes = " << (cook_torrance_count * sizeof(CookTorranceMaterial)) << " bytes" << std::endl;
        std::cout << "    Material memory total: " << material_memory << " bytes" << std::endl;
        std::cout << "  Container overhead: " << container_overhead << " bytes" << std::endl;
        std::cout << "  Total scene memory: " << total_scene_memory << " bytes ("
                  << (total_scene_memory / 1024.0f) << " KB)" << std::endl;
        
        // Educational insights about memory scaling
        std::cout << "\nMemory Scaling Analysis:" << std::endl;
        if (primitives.size() > 0) {
            float bytes_per_sphere = static_cast<float>(sphere_memory) / primitives.size();
            std::cout << "  Memory per sphere: " << bytes_per_sphere << " bytes" << std::endl;
            std::cout << "  Linear scaling: O(n) where n = number of spheres" << std::endl;
            
            if (primitives.size() > 1000) {
                std::cout << "  NOTE: Large primitive count may impact intersection performance" << std::endl;
                std::cout << "  Consider spatial acceleration structures for complex scenes" << std::endl;
            }
        }
        
        if (materials.size() > 0) {
            float bytes_per_material = static_cast<float>(material_memory) / materials.size();
            std::cout << "  Memory per material: " << bytes_per_material << " bytes" << std::endl;
            std::cout << "  Material memory is typically small compared to geometry" << std::endl;
        }
        
        // Memory efficiency analysis
        std::cout << "\nMemory Efficiency:" << std::endl;
        if (container_overhead > total_scene_memory * 0.5f) {
            std::cout << "  WARNING: High container overhead (" << (container_overhead * 100.0f / total_scene_memory) 
                      << "% of total)" << std::endl;
            std::cout << "  Consider using reserve() or shrink_to_fit() to optimize memory" << std::endl;
        } else {
            std::cout << "  Container overhead: " << (container_overhead * 100.0f / total_scene_memory) 
                      << "% (reasonable)" << std::endl;
        }
        
        std::cout << "=== End Scene Memory Analysis ===" << std::endl;
    }
    
    // Memory usage warnings for educational guidance
    void check_memory_usage_warnings(size_t image_memory_bytes = 0) const {
        size_t scene_memory = calculate_scene_memory_usage();
        size_t total_memory = scene_memory + image_memory_bytes;
        
        std::cout << "\n=== Memory Usage Warnings ===" << std::endl;
        
        // Convert to MB for easier understanding
        float scene_mb = scene_memory / (1024.0f * 1024.0f);
        float image_mb = image_memory_bytes / (1024.0f * 1024.0f);
        float total_mb = total_memory / (1024.0f * 1024.0f);
        
        std::cout << "Memory Usage Summary:" << std::endl;
        std::cout << "  Scene data: " << scene_mb << " MB" << std::endl;
        std::cout << "  Image buffer: " << image_mb << " MB" << std::endl;
        std::cout << "  Total memory: " << total_mb << " MB" << std::endl;
        
        // Educational warnings based on memory usage
        if (total_mb > 100.0f) {
            std::cout << "\n⚠️  WARNING: High memory usage detected!" << std::endl;
            std::cout << "Educational guidance:" << std::endl;
            std::cout << "  - Total memory exceeds 100MB threshold" << std::endl;
            std::cout << "  - Consider smaller image resolutions for educational experiments" << std::endl;
            std::cout << "  - Large memory usage may impact system performance" << std::endl;
            
            if (image_mb > scene_mb * 10) {
                std::cout << "  - Image buffer dominates memory usage (reduce resolution)" << std::endl;
            }
            if (scene_mb > 10.0f) {
                std::cout << "  - Scene complexity is high (consider simpler scenes)" << std::endl;
            }
        } else if (total_mb > 50.0f) {
            std::cout << "\n🔶 NOTICE: Moderate memory usage" << std::endl;
            std::cout << "Educational note: Memory usage is reasonable for learning purposes" << std::endl;
        } else {
            std::cout << "\n✅ Memory usage is optimal for educational ray tracing" << std::endl;
        }
        
        // Quadratic scaling educational explanation
        if (image_mb > 1.0f) {
            std::cout << "\nEducational Insight - Memory Scaling:" << std::endl;
            std::cout << "  - Image memory scales quadratically: O(width × height)" << std::endl;
            std::cout << "  - Doubling resolution (e.g., 512→1024) quadruples memory" << std::endl;
            std::cout << "  - This demonstrates why memory management is crucial in graphics" << std::endl;
        }
        
        std::cout << "=== End Memory Warnings ===" << std::endl;
    }
    
    // Show relationship between scene complexity and memory requirements
    void explain_memory_scene_relationship() const {
        std::cout << "\n=== Educational: Memory-Scene Relationship ===" << std::endl;
        
        size_t primitive_memory = primitives.size() * sizeof(Sphere);
        size_t material_memory = materials.size() * sizeof(LambertMaterial);
        
        std::cout << "Scene Complexity Metrics:" << std::endl;
        std::cout << "  Primitive count: " << primitives.size() << " spheres" << std::endl;
        std::cout << "  Material count: " << materials.size() << " materials" << std::endl;
        std::cout << "  Memory per primitive: " << sizeof(Sphere) << " bytes" << std::endl;
        std::cout << "  Memory per material: " << sizeof(LambertMaterial) << " bytes" << std::endl;
        
        std::cout << "\nLinear Scaling Analysis:" << std::endl;
        std::cout << "  Current primitive memory: " << primitive_memory << " bytes" << std::endl;
        std::cout << "  If doubled to " << (primitives.size() * 2) << " spheres: " 
                  << (primitive_memory * 2) << " bytes" << std::endl;
        std::cout << "  Memory scaling: O(n) linear with primitive count" << std::endl;
        
        // Performance implications
        std::cout << "\nPerformance-Memory Trade-offs:" << std::endl;
        std::cout << "  Scene memory: " << (calculate_scene_memory_usage() / 1024.0f) << " KB" << std::endl;
        std::cout << "  Intersection cost: O(n) per ray (n = primitive count)" << std::endl;
        
        if (primitives.size() > 10) {
            std::cout << "  Educational note: " << primitives.size() << " primitives requires " 
                      << primitives.size() << " intersection tests per ray" << std::endl;
            std::cout << "  Real-world optimization: Use spatial acceleration (BVH, octrees)" << std::endl;
        }
        
        std::cout << "=== End Memory-Scene Relationship ===" << std::endl;
    }
};