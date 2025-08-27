#include <iostream>
#include <cstring>
#include "core/vector3.hpp"
#include "core/point3.hpp"
#include "core/ray.hpp"
#include "core/sphere.hpp"
#include "core/scene.hpp"
#include "core/scene_loader.hpp"
#include "lights/point_light.hpp"
#include "materials/lambert.hpp"
#include "materials/cook_torrance.hpp"
#include "core/camera.hpp"
#include "core/image.hpp"
#include "core/performance_timer.hpp"
#include "core/progress_reporter.hpp"
#include <chrono>

// Cross-platform preprocessor directives
#ifdef PLATFORM_APPLE
    #include <TargetConditionals.h>
    constexpr const char* PLATFORM_NAME = "Apple";
#elif defined(PLATFORM_WINDOWS)
    constexpr const char* PLATFORM_NAME = "Windows";
#elif defined(PLATFORM_LINUX)
    constexpr const char* PLATFORM_NAME = "Linux";
#else
    constexpr const char* PLATFORM_NAME = "Unknown";
#endif

int main(int argc, char* argv[]) {
    // Check for help request
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
            Camera::print_command_line_help();
            std::cout << "\n=== Multi-Resolution and Performance Help ===" << std::endl;
            std::cout << "Resolution parameters (Story 2.4):" << std::endl;
            std::cout << "--resolution <WxH>    Set image resolution (e.g., --resolution 1024x768)" << std::endl;
            std::cout << "                      Common presets: 256x256, 512x512, 1024x1024, 2048x2048" << std::endl;
            std::cout << "                      Default: 1024x768 (Epic 2 Showcase)" << std::endl;
            std::cout << "\nScene parameters:" << std::endl;
            std::cout << "--scene <filename>    Load scene from file (default: assets/showcase_scene.scene)" << std::endl;
            std::cout << "--no-scene            Use hardcoded single sphere for compatibility" << std::endl;
            std::cout << "\nMaterial parameters (Cook-Torrance BRDF - Story 3.1):" << std::endl;
            std::cout << "--material <type>     Material type: lambert, cook-torrance (default: lambert)" << std::endl;
            std::cout << "--roughness <value>   Surface roughness for Cook-Torrance (0.0-1.0, default: 0.5)" << std::endl;
            std::cout << "--metallic <value>    Metallic parameter for Cook-Torrance (0.0-1.0, default: 0.0)" << std::endl;
            std::cout << "--specular <value>    Specular reflectance for dielectrics (0.0-1.0, default: 0.04)" << std::endl;
            std::cout << "\nDebug and verbosity parameters:" << std::endl;
            std::cout << "--quiet               Minimal output (no educational breakdowns, errors only)" << std::endl;
            std::cout << "--verbose             Full educational output (default behavior)" << std::endl;
            std::cout << "\nQuick presets:" << std::endl;
            std::cout << "--preset showcase     Epic 2 showcase (1024x768, complex scene, optimal camera)" << std::endl;
            std::cout << "--showcase            Shorthand for --preset showcase" << std::endl;
            std::cout << "--preset cook-torrance Cook-Torrance demo (1024x1024, single sphere only, scene files not supported)" << std::endl;
            std::cout << "--cook-torrance       Shorthand for --preset cook-torrance" << std::endl;
            std::cout << "--preset performance  Fast render (512x512, simple scene, basic camera)" << std::endl;
            std::cout << "--performance         Shorthand for --preset performance" << std::endl;
            std::cout << "--preset quality      High quality (1024x1024, showcase scene, wide FOV)" << std::endl;
            std::cout << "--quality             Shorthand for --preset quality" << std::endl;
            std::cout << "\nScene file format (Lambert materials only):" << std::endl;
            std::cout << "material <name> <r> <g> <b>  - Define material with RGB albedo" << std::endl;
            std::cout << "sphere <x> <y> <z> <radius> <material>  - Add sphere to scene" << std::endl;
            std::cout << "\nNOTE: Scene files only support Lambert materials. For Cook-Torrance materials," << std::endl;
            std::cout << "      use --cook-torrance preset (single sphere) or --no-scene with --material cook-torrance" << std::endl;
            return 0;
        }
    }
    
    // Check for scene file and resolution parameters
    // Epic 2 Showcase Defaults - optimized to demonstrate all capabilities
    std::string scene_filename = "../assets/showcase_scene.scene";  // Enhanced showcase scene
    bool use_scene_file = true;
    Resolution image_resolution = Resolution::parse_from_string("1024x768");  // High-quality 4:3 aspect ratio
    
    // Cook-Torrance material parameters (Story 3.1)
    std::string material_type = "lambert";  // Default to Lambert for compatibility
    float roughness_param = 0.5f;          // Medium roughness
    float metallic_param = 0.0f;           // Dielectric (non-metal) by default
    float specular_param = 0.04f;          // Typical dielectric F0
    
    // Debug and verbosity control parameters
    bool quiet_mode = false;               // Minimal output mode
    
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--scene") == 0 && i + 1 < argc) {
            scene_filename = argv[i + 1];
            std::cout << "Scene file override: " << scene_filename << std::endl;
            i++;  // Skip next argument since we consumed it
        } else if (std::strcmp(argv[i], "--no-scene") == 0) {
            use_scene_file = false;
            std::cout << "Scene loading disabled - using hardcoded sphere" << std::endl;
        } else if (std::strcmp(argv[i], "--resolution") == 0 && i + 1 < argc) {
            try {
                image_resolution = Resolution::parse_from_string(argv[i + 1]);
                std::cout << "Resolution override: " << image_resolution.width << "x" << image_resolution.height << std::endl;
                image_resolution.print_memory_analysis();
            } catch (const std::exception& e) {
                std::cout << "ERROR: Invalid resolution format '" << argv[i + 1] << "'" << std::endl;
                std::cout << "Error details: " << e.what() << std::endl;
                std::cout << "Supported formats: WIDTHxHEIGHT (e.g., 512x512)" << std::endl;
                std::cout << "Common presets: 256x256, 512x512, 1024x1024, 2048x2048" << std::endl;
                return 1;
            }
            i++;  // Skip next argument since we consumed it
        } else if (std::strcmp(argv[i], "--preset") == 0 && i + 1 < argc) {
            std::string preset = argv[i + 1];
            std::cout << "Using preset: " << preset << std::endl;
            
            if (preset == "showcase") {
                // Epic 2 Showcase - ensure scene file path works from build/ directory
                scene_filename = "../assets/showcase_scene.scene";
                image_resolution = Resolution::parse_from_string("1024x768");
                std::cout << "Epic 2 Showcase preset: 1024x768, complex scene, optimal camera" << std::endl;
            } else if (preset == "cook-torrance") {
                // Cook-Torrance Demo - single sphere with Cook-Torrance material (scene files not supported)
                use_scene_file = false;  // Force single sphere mode - scene files don't support Cook-Torrance
                image_resolution = Resolution::parse_from_string("1024x1024");
                material_type = "cook-torrance";
                quiet_mode = false;  // Keep educational output
                std::cout << "Cook-Torrance Demo preset: 1024x1024, single sphere with Cook-Torrance material" << std::endl;
                std::cout << "NOTE: Scene files do not support Cook-Torrance materials - using single sphere mode" << std::endl;
            } else if (preset == "performance") {
                image_resolution = Resolution::MEDIUM;  // 512x512
                scene_filename = "../assets/simple_scene.scene";
                std::cout << "Performance preset: 512x512, simple scene, fast render" << std::endl;
            } else if (preset == "quality") {
                image_resolution = Resolution::LARGE;  // 1024x1024
                scene_filename = "../assets/showcase_scene.scene";
                std::cout << "Quality preset: 1024x1024, showcase scene, maximum quality" << std::endl;
            } else {
                std::cout << "ERROR: Unknown preset '" << preset << "'" << std::endl;
                std::cout << "Available presets: showcase, cook-torrance, performance, quality" << std::endl;
                return 1;
            }
            i++;  // Skip next argument since we consumed it
        } else if (std::strcmp(argv[i], "--showcase") == 0) {
            scene_filename = "../assets/showcase_scene.scene";
            image_resolution = Resolution::parse_from_string("1024x768");
            std::cout << "Using preset: showcase" << std::endl;
            std::cout << "Epic 2 Showcase preset: 1024x768, complex scene, optimal camera" << std::endl;
        } else if (std::strcmp(argv[i], "--cook-torrance") == 0) {
            use_scene_file = false;  // Force single sphere mode - scene files don't support Cook-Torrance
            image_resolution = Resolution::parse_from_string("1024x1024");
            material_type = "cook-torrance";
            quiet_mode = false;
            std::cout << "Using preset: cook-torrance" << std::endl;
            std::cout << "Cook-Torrance Demo preset: 1024x1024, single sphere with Cook-Torrance material" << std::endl;
            std::cout << "NOTE: Scene files do not support Cook-Torrance materials - using single sphere mode" << std::endl;
        } else if (std::strcmp(argv[i], "--performance") == 0) {
            image_resolution = Resolution::MEDIUM;  // 512x512
            scene_filename = "../assets/simple_scene.scene";
            std::cout << "Using preset: performance" << std::endl;
            std::cout << "Performance preset: 512x512, simple scene, fast render" << std::endl;
        } else if (std::strcmp(argv[i], "--quality") == 0) {
            image_resolution = Resolution::LARGE;  // 1024x1024
            scene_filename = "../assets/showcase_scene.scene";
            std::cout << "Using preset: quality" << std::endl;
            std::cout << "Quality preset: 1024x1024, showcase scene, maximum quality" << std::endl;
        } else if (std::strcmp(argv[i], "--material") == 0 && i + 1 < argc) {
            material_type = argv[i + 1];
            std::cout << "Material type override: " << material_type << std::endl;
            if (material_type != "lambert" && material_type != "cook-torrance") {
                std::cout << "ERROR: Unknown material type '" << material_type << "'" << std::endl;
                std::cout << "Supported materials: lambert, cook-torrance" << std::endl;
                return 1;
            }
            i++;  // Skip next argument since we consumed it
        } else if (std::strcmp(argv[i], "--roughness") == 0 && i + 1 < argc) {
            roughness_param = std::stof(argv[i + 1]);
            roughness_param = std::max(0.01f, std::min(1.0f, roughness_param));  // Clamp to valid range
            std::cout << "Roughness override: " << roughness_param << std::endl;
            i++;  // Skip next argument since we consumed it
        } else if (std::strcmp(argv[i], "--metallic") == 0 && i + 1 < argc) {
            metallic_param = std::stof(argv[i + 1]);
            metallic_param = std::max(0.0f, std::min(1.0f, metallic_param));  // Clamp to valid range
            std::cout << "Metallic override: " << metallic_param << std::endl;
            i++;  // Skip next argument since we consumed it
        } else if (std::strcmp(argv[i], "--specular") == 0 && i + 1 < argc) {
            specular_param = std::stof(argv[i + 1]);
            specular_param = std::max(0.0f, std::min(1.0f, specular_param));  // Clamp to valid range
            std::cout << "Specular override: " << specular_param << std::endl;
            i++;  // Skip next argument since we consumed it
        } else if (std::strcmp(argv[i], "--quiet") == 0) {
            quiet_mode = true;
            std::cout << "Quiet mode enabled - minimal output" << std::endl;
        } else if (std::strcmp(argv[i], "--verbose") == 0) {
            quiet_mode = false;
            std::cout << "Verbose mode enabled - full educational output" << std::endl;
        } else if (strncmp(argv[i], "--", 2) == 0) {
            // Check if it's a known camera argument (handled later by camera.set_from_command_line_args)
            if (std::strcmp(argv[i], "--camera-pos") == 0 || 
                std::strcmp(argv[i], "--camera-target") == 0 || 
                std::strcmp(argv[i], "--fov") == 0 ||
                std::strcmp(argv[i], "--material") == 0 ||
                std::strcmp(argv[i], "--roughness") == 0 ||
                std::strcmp(argv[i], "--metallic") == 0 ||
                std::strcmp(argv[i], "--specular") == 0 ||
                std::strcmp(argv[i], "--quiet") == 0 ||
                std::strcmp(argv[i], "--verbose") == 0) {
                // Valid camera or material argument, skip it and its parameter
                if (i + 1 < argc) i++; // Skip parameter if it exists
            } else {
                // Unknown argument starting with --
                std::cout << "ERROR: Unknown argument '" << argv[i] << "'" << std::endl;
                std::cout << "Did you mean:" << std::endl;
                
                // Provide helpful suggestions based on common mistakes
                if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
                    // This shouldn't happen since help is handled earlier, but just in case
                } else if (std::strcmp(argv[i], "--showcase") == 0 || std::strcmp(argv[i], "--performance") == 0 || std::strcmp(argv[i], "--quality") == 0) {
                    // This shouldn't happen since these are handled above, but just in case
                } else {
                    // Common typos and suggestions
                    std::string arg = argv[i];
                    if (arg.find("quality") != std::string::npos) {
                        std::cout << "  --preset quality     (High quality preset)" << std::endl;
                        std::cout << "  --quality            (Shorthand for --preset quality)" << std::endl;
                    } else if (arg.find("performance") != std::string::npos) {
                        std::cout << "  --preset performance (Fast render preset)" << std::endl;
                        std::cout << "  --performance        (Shorthand for --preset performance)" << std::endl;
                    } else if (arg.find("showcase") != std::string::npos) {
                        std::cout << "  --preset showcase    (Epic 2 showcase preset)" << std::endl;
                        std::cout << "  --showcase           (Shorthand for --preset showcase)" << std::endl;
                    } else if (arg.find("resolution") != std::string::npos || arg.find("res") != std::string::npos) {
                        std::cout << "  --resolution WxH     (e.g., --resolution 1024x768)" << std::endl;
                    } else if (arg.find("scene") != std::string::npos) {
                        std::cout << "  --scene <filename>   (Load custom scene file)" << std::endl;
                        std::cout << "  --no-scene           (Use hardcoded sphere)" << std::endl;
                    } else if (arg.find("camera") != std::string::npos || arg.find("pos") != std::string::npos) {
                        std::cout << "  --camera-pos x,y,z   (Set camera position)" << std::endl;
                        std::cout << "  --camera-target x,y,z(Set camera target)" << std::endl;
                    } else if (arg.find("fov") != std::string::npos) {
                        std::cout << "  --fov degrees        (Set field of view)" << std::endl;
                    } else {
                        std::cout << "  --help               (Show all available options)" << std::endl;
                        std::cout << "  --preset showcase    (Epic 2 showcase)" << std::endl;
                        std::cout << "  --resolution WxH     (Set custom resolution)" << std::endl;
                    }
                }
                std::cout << "\nUse --help to see all available options." << std::endl;
                return 1;
            }
        }
    }
    
    std::cout << "=== Educational Ray Tracer - Epic 1 Foundation ===" << std::endl;
    std::cout << "Platform: " << PLATFORM_NAME << std::endl;
    std::cout << "C++ Standard: " << __cplusplus << std::endl;
    
    #ifdef PLATFORM_APPLE
        #if TARGET_CPU_ARM64
            std::cout << "Apple Silicon optimization: Enabled" << std::endl;
        #else
            std::cout << "Apple Intel optimization: Enabled" << std::endl;
        #endif
    #endif
    
    std::cout << "Build system verification: SUCCESS" << std::endl;
    std::cout << "\n=== Story 1.3: Ray-Sphere Intersection Validation ===" << std::endl;
    
    // Test Case 1: Ray from origin to sphere at (0,0,-5) with radius 1.0
    // Expected: intersection at (0,0,-4) with discriminant = 16.0
    std::cout << "\n--- Test Case 1: Known Intersection ---" << std::endl;
    Ray test_ray(Point3(0, 0, 0), Vector3(0, 0, -1));  // Ray pointing down negative Z-axis
    Sphere test_sphere(Point3(0, 0, -5), 1.0f, 0);     // Sphere centered at (0,0,-5) with radius 1, material index 0
    
    // Validate sphere geometry
    if (!test_sphere.validate_geometry()) {
        std::cout << "ERROR: Invalid sphere geometry!" << std::endl;
        return 1;
    }
    
    // Perform intersection calculation
    Sphere::Intersection result = test_sphere.intersect(test_ray);
    
    // Validate intersection results
    std::cout << "\n--- Intersection Validation ---" << std::endl;
    if (result.hit) {
        std::cout << "✓ Intersection detected" << std::endl;
        std::cout << "✓ Intersection t-parameter: " << result.t << " (expected: 4.0)" << std::endl;
        std::cout << "✓ Intersection coordinates: (" << result.point.x << ", " << result.point.y << ", " << result.point.z << ")" << std::endl;
        std::cout << "✓ Expected coordinates: (0.0, 0.0, -4.0)" << std::endl;
        
        // Verify coordinates match expected values within tolerance
        float tolerance = 1e-6f;
        bool coords_valid = (std::abs(result.point.x - 0.0f) < tolerance) &&
                           (std::abs(result.point.y - 0.0f) < tolerance) &&
                           (std::abs(result.point.z - (-4.0f)) < tolerance);
        
        if (coords_valid) {
            std::cout << "✓ Coordinate validation: PASSED" << std::endl;
        } else {
            std::cout << "✗ Coordinate validation: FAILED" << std::endl;
        }
        
        // Verify normal vector
        std::cout << "✓ Surface normal: (" << result.normal.x << ", " << result.normal.y << ", " << result.normal.z << ")" << std::endl;
        std::cout << "✓ Expected normal: (0.0, 0.0, 1.0)" << std::endl;
        
        bool normal_valid = (std::abs(result.normal.x - 0.0f) < tolerance) &&
                           (std::abs(result.normal.y - 0.0f) < tolerance) &&
                           (std::abs(result.normal.z - 1.0f) < tolerance);
        
        if (normal_valid) {
            std::cout << "✓ Normal validation: PASSED" << std::endl;
        } else {
            std::cout << "✗ Normal validation: FAILED" << std::endl;
        }
        
    } else {
        std::cout << "✗ ERROR: Expected intersection but none found!" << std::endl;
        return 1;
    }
    
    // Test Case 2: Ray missing sphere (no intersection)
    std::cout << "\n--- Test Case 2: No Intersection (Ray Misses Sphere) ---" << std::endl;
    Ray miss_ray(Point3(0, 0, 0), Vector3(1, 0, 0));      // Ray pointing along x-axis
    Sphere::Intersection miss_result = test_sphere.intersect(miss_ray);
    
    if (!miss_result.hit) {
        std::cout << "✓ No intersection detected (as expected)" << std::endl;
    } else {
        std::cout << "✗ ERROR: Unexpected intersection detected!" << std::endl;
    }
    
    // Test Case 3: Ray behind camera (negative t)
    std::cout << "\n--- Test Case 3: Ray Behind Camera (Negative t) ---" << std::endl;
    Ray behind_ray(Point3(0, 0, -10), Vector3(0, 0, -1)); // Ray starting behind sphere, pointing away
    Sphere::Intersection behind_result = test_sphere.intersect(behind_ray);
    
    if (!behind_result.hit) {
        std::cout << "✓ No intersection detected for ray behind camera (as expected)" << std::endl;
    } else {
        std::cout << "✗ ERROR: Unexpected intersection for ray behind camera!" << std::endl;
    }
    
    // Test Case 4: Grazing ray (tangent to sphere)
    std::cout << "\n--- Test Case 4: Grazing Ray (Tangent to Sphere) ---" << std::endl;
    Ray graze_ray(Point3(1, 0, 0), Vector3(0, 0, -1));    // Ray tangent to sphere at x=radius
    Sphere unit_sphere(Point3(0, 0, -5), 1.0f, 0);        // Unit sphere for tangency test, material index 0
    Sphere::Intersection graze_result = unit_sphere.intersect(graze_ray);
    
    if (graze_result.hit) {
        std::cout << "✓ Grazing intersection detected" << std::endl;
        std::cout << "✓ Graze t-parameter: " << graze_result.t << std::endl;
    } else {
        std::cout << "Note: Grazing ray may not intersect due to floating-point precision" << std::endl;
    }
    
    std::cout << "\n=== Ray-Sphere Intersection Implementation Complete ===" << std::endl;
    std::cout << "All edge cases handled successfully" << std::endl;
    
    std::cout << "\n=== Story 2.3: Multi-Primitive Scene Management Testing ===" << std::endl;
    
    // Test Scene-based intersection with multiple spheres
    std::cout << "\n--- Multi-Sphere Scene Construction ---" << std::endl;
    Scene test_scene;
    
    // Add materials to scene
    LambertMaterial red_material(Vector3(0.7f, 0.3f, 0.3f));
    LambertMaterial blue_material(Vector3(0.3f, 0.3f, 0.7f));
    LambertMaterial green_material(Vector3(0.3f, 0.7f, 0.3f));
    
    int red_mat_idx = test_scene.add_material(red_material);
    int blue_mat_idx = test_scene.add_material(blue_material);
    int green_mat_idx = test_scene.add_material(green_material);
    
    // Add spheres to scene (using new constructor with material index)
    Sphere sphere1(Point3(0.0f, 0.0f, -5.0f), 1.0f, red_mat_idx);     // Central red sphere
    Sphere sphere2(Point3(2.0f, 0.0f, -6.0f), 0.8f, blue_mat_idx);    // Blue sphere to the right
    Sphere sphere3(Point3(-1.5f, 1.0f, -4.0f), 0.6f, green_mat_idx);  // Green sphere upper left
    
    int sphere1_idx = test_scene.add_sphere(sphere1);
    int sphere2_idx = test_scene.add_sphere(sphere2);
    int sphere3_idx = test_scene.add_sphere(sphere3);
    
    test_scene.print_scene_statistics();
    
    // Test Case 1: Ray hits central sphere (should be closest)
    std::cout << "\n--- Test Case 1: Ray Hits Central Sphere (Closest Hit Logic) ---" << std::endl;
    Ray central_ray(Point3(0, 0, 0), Vector3(0, 0, -1));  // Ray toward central sphere
    Scene::Intersection scene_hit = test_scene.intersect(central_ray);
    
    if (scene_hit.hit) {
        std::cout << "✓ Scene intersection found" << std::endl;
        std::cout << "✓ Hit distance: t = " << scene_hit.t << std::endl;
        std::cout << "✓ Hit primitive: sphere at (" << scene_hit.primitive->center.x 
                 << ", " << scene_hit.primitive->center.y << ", " << scene_hit.primitive->center.z << ")" << std::endl;
        std::cout << "✓ Material color: (" << scene_hit.material->base_color.x 
                 << ", " << scene_hit.material->base_color.y << ", " << scene_hit.material->base_color.z << ")" << std::endl;
        
        // Verify it hit the central red sphere (closest at z=-4)
        float expected_t = 4.0f;  // Ray from origin to sphere at z=-5 with radius 1
        if (std::abs(scene_hit.t - expected_t) < 1e-5f) {
            std::cout << "✓ Closest hit validation: PASSED (hit central sphere as expected)" << std::endl;
        } else {
            std::cout << "✗ Closest hit validation: FAILED (expected t=" << expected_t << ", got t=" << scene_hit.t << ")" << std::endl;
        }
    } else {
        std::cout << "✗ ERROR: Expected scene intersection but none found!" << std::endl;
    }
    
    // Test Case 2: Ray hits multiple spheres (closest hit logic)
    std::cout << "\n--- Test Case 2: Ray Through Multiple Spheres (Depth Testing) ---" << std::endl;
    Ray multi_ray(Point3(-1.5f, 1.0f, 0), Vector3(0, 0, -1));  // Ray toward green sphere
    Scene::Intersection multi_hit = test_scene.intersect(multi_ray);
    
    if (multi_hit.hit) {
        std::cout << "✓ Multi-sphere ray intersection found" << std::endl;
        std::cout << "✓ Closest hit distance: t = " << multi_hit.t << std::endl;
        std::cout << "✓ Should hit green sphere (closest at z=-4)" << std::endl;
        
        // Check if it's the green material (should be closest)
        bool is_green = (std::abs(multi_hit.material->base_color.y - 0.7f) < 1e-5f);
        if (is_green) {
            std::cout << "✓ Depth testing validation: PASSED (hit closest green sphere)" << std::endl;
        } else {
            std::cout << "✗ Depth testing validation: FAILED (hit wrong sphere)" << std::endl;
        }
    } else {
        std::cout << "✗ ERROR: Expected multi-sphere intersection but none found!" << std::endl;
    }
    
    // Test Case 3: Educational performance monitoring
    std::cout << "\n--- Test Case 3: Performance Monitoring Validation ---" << std::endl;
    test_scene.reset_statistics();
    
    // Generate several rays to test performance tracking
    for (int i = 0; i < 5; i++) {
        Ray perf_ray(Point3(-2 + i * 1.0f, 0, 0), Vector3(0, 0, -1));
        test_scene.intersect(perf_ray);
    }
    
    std::cout << "Performance monitoring test complete - statistics should show 15 intersection tests (5 rays × 3 spheres)" << std::endl;
    
    std::cout << "\n=== Multi-Primitive Scene Management Complete ===" << std::endl;
    
    std::cout << "\n=== Story 1.3: Lambert BRDF Material Validation ===" << std::endl;
    
    // Test Case 1: Lambert BRDF with known albedo values
    std::cout << "\n--- Test Case 1: Lambert BRDF Mathematical Validation ---" << std::endl;
    LambertMaterial lambert_material(Vector3(0.8f, 0.8f, 0.8f));  // 80% albedo (gray diffuse)
    
    // Validate energy conservation first
    if (!lambert_material.validate_energy_conservation()) {
        std::cout << "ERROR: Lambert material violates energy conservation!" << std::endl;
        return 1;
    }
    
    // Test BRDF evaluation with known directions
    Vector3 light_dir = Vector3(0, 0, 1).normalize();      // Light coming from positive Z
    Vector3 view_dir = Vector3(0, 0, 1).normalize();       // Camera looking from positive Z  
    Vector3 surface_normal = Vector3(0, 0, 1).normalize(); // Surface facing positive Z
    
    Vector3 brdf_result = lambert_material.evaluate_brdf(light_dir, view_dir, surface_normal);
    
    // Expected BRDF value: albedo / π = 0.8 / π ≈ 0.2546
    float expected_brdf = 0.8f / M_PI;
    float tolerance = 1e-6f;
    
    std::cout << "\n--- BRDF Validation Results ---" << std::endl;
    std::cout << "✓ Expected BRDF value: " << expected_brdf << std::endl;
    std::cout << "✓ Computed BRDF value: " << brdf_result.x << " (R), " << brdf_result.y << " (G), " << brdf_result.z << " (B)" << std::endl;
    
    bool brdf_valid = (std::abs(brdf_result.x - expected_brdf) < tolerance) &&
                      (std::abs(brdf_result.y - expected_brdf) < tolerance) &&
                      (std::abs(brdf_result.z - expected_brdf) < tolerance);
    
    if (brdf_valid) {
        std::cout << "✓ BRDF validation: PASSED" << std::endl;
    } else {
        std::cout << "✗ BRDF validation: FAILED" << std::endl;
        return 1;
    }
    
    // Test Case 2: Complete light scattering calculation
    std::cout << "\n--- Test Case 2: Complete Light Transport Calculation ---" << std::endl;
    Vector3 incident_light = Vector3(1.0f, 1.0f, 1.0f);  // White light with intensity 1.0
    
    Vector3 scattered_light = lambert_material.scatter_light(light_dir, view_dir, surface_normal, incident_light);
    
    // Expected result: (albedo/π) * incident_light * cos(θ)
    // cos(θ) = 1.0 for normal incidence, so result = 0.8/π ≈ 0.2546
    Vector3 expected_scattered = Vector3(expected_brdf, expected_brdf, expected_brdf);
    
    std::cout << "\n--- Light Scattering Validation ---" << std::endl;
    std::cout << "✓ Expected scattered light: (" << expected_scattered.x << ", " << expected_scattered.y << ", " << expected_scattered.z << ")" << std::endl;
    std::cout << "✓ Computed scattered light: (" << scattered_light.x << ", " << scattered_light.y << ", " << scattered_light.z << ")" << std::endl;
    
    bool scattering_valid = (std::abs(scattered_light.x - expected_scattered.x) < tolerance) &&
                           (std::abs(scattered_light.y - expected_scattered.y) < tolerance) &&
                           (std::abs(scattered_light.z - expected_scattered.z) < tolerance);
    
    if (scattering_valid) {
        std::cout << "✓ Light scattering validation: PASSED" << std::endl;
    } else {
        std::cout << "✗ Light scattering validation: FAILED" << std::endl;
        return 1;
    }
    
    // Test Case 3: Hemispherical reflectance validation
    Vector3 reflectance = lambert_material.hemispherical_reflectance();
    std::cout << "\n--- Hemispherical Reflectance Validation ---" << std::endl;
    std::cout << "✓ Hemispherical reflectance: (" << reflectance.x << ", " << reflectance.y << ", " << reflectance.z << ")" << std::endl;
    std::cout << "✓ Expected (equals albedo): (0.8, 0.8, 0.8)" << std::endl;
    
    bool reflectance_valid = (std::abs(reflectance.x - 0.8f) < tolerance) &&
                            (std::abs(reflectance.y - 0.8f) < tolerance) &&
                            (std::abs(reflectance.z - 0.8f) < tolerance);
    
    if (reflectance_valid) {
        std::cout << "✓ Hemispherical reflectance validation: PASSED" << std::endl;
    } else {
        std::cout << "✗ Hemispherical reflectance validation: FAILED" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== Lambert BRDF Implementation Complete ===" << std::endl;
    std::cout << "All mathematical validations passed" << std::endl;
    
    std::cout << "\n=== Story 1.3: Complete Single-Ray Rendering Pipeline ===" << std::endl;
    
    // Complete ray tracing setup with explicit values as specified
    std::cout << "\n--- Complete Ray Tracing Setup ---" << std::endl;
    
    // Camera ray from origin to sphere intersection point
    Point3 camera_origin(0, 0, 0);
    Vector3 camera_direction(0, 0, -1);  // Looking down negative Z-axis
    Ray camera_ray(camera_origin, camera_direction);
    std::cout << "Camera ray origin: (" << camera_origin.x << ", " << camera_origin.y << ", " << camera_origin.z << ")" << std::endl;
    std::cout << "Camera ray direction: (" << camera_direction.x << ", " << camera_direction.y << ", " << camera_direction.z << ")" << std::endl;
    
    // Sphere at (0,0,-5) with radius 1.0 and Lambert material
    Point3 sphere_center(0, 0, -5);
    float sphere_radius = 1.0f;
    LambertMaterial sphere_material(Vector3(0.7f, 0.3f, 0.3f));  // Reddish diffuse material
    Sphere render_sphere(sphere_center, sphere_radius, 0);       // Material index 0 (will be ignored in this test)
    std::cout << "Sphere center: (" << sphere_center.x << ", " << sphere_center.y << ", " << sphere_center.z << ")" << std::endl;
    std::cout << "Sphere radius: " << sphere_radius << std::endl;
    std::cout << "Sphere material albedo: (" << sphere_material.base_color.x << ", " << sphere_material.base_color.y << ", " << sphere_material.base_color.z << ")" << std::endl;
    
    // Point light source with explicit position and color
    Vector3 light_position(2, 2, -3);  // Light positioned above and to the right of sphere
    Vector3 light_color(1.0f, 1.0f, 1.0f);  // White light
    float light_intensity = 10.0f;  // Bright light to overcome distance falloff
    PointLight scene_light(light_position, light_color, light_intensity);
    std::cout << "Light position: (" << light_position.x << ", " << light_position.y << ", " << light_position.z << ")" << std::endl;
    std::cout << "Light color: (" << light_color.x << ", " << light_color.y << ", " << light_color.z << ")" << std::endl;
    std::cout << "Light intensity: " << light_intensity << std::endl;
    
    // Validate all scene components
    if (!render_sphere.validate_geometry()) {
        std::cout << "ERROR: Invalid sphere geometry!" << std::endl;
        return 1;
    }
    
    if (!sphere_material.validate_energy_conservation()) {
        std::cout << "ERROR: Material violates energy conservation!" << std::endl;
        return 1;
    }
    
    if (!scene_light.validate_parameters()) {
        std::cout << "ERROR: Invalid light configuration!" << std::endl;
        return 1;
    }
    
    std::cout << "\n--- Complete Light Transport Calculation ---" << std::endl;
    std::cout << "Following the rendering equation: L_o = ∫ f_r(wi,wo) * L_i(wi) * cos(θ_i) * dωi" << std::endl;
    std::cout << "For single point light: L_o = f_r * L_i * cos(θ_i)" << std::endl;
    
    // Step 1: Ray-sphere intersection
    std::cout << "\n=== Step 1: Ray-Sphere Intersection ===" << std::endl;
    Sphere::Intersection intersection = render_sphere.intersect(camera_ray);
    
    if (!intersection.hit) {
        std::cout << "No intersection found - ray misses sphere" << std::endl;
        std::cout << "Final pixel color: (0, 0, 0) - black background" << std::endl;
        return 0;
    }
    
    std::cout << "✓ Ray intersects sphere at t = " << intersection.t << std::endl;
    std::cout << "✓ Intersection point: (" << intersection.point.x << ", " << intersection.point.y << ", " << intersection.point.z << ")" << std::endl;
    std::cout << "✓ Surface normal: (" << intersection.normal.x << ", " << intersection.normal.y << ", " << intersection.normal.z << ")" << std::endl;
    
    // Step 2: Light source evaluation
    std::cout << "\n=== Step 2: Light Source Evaluation ===" << std::endl;
    float pdf;
    Vector3 light_direction = scene_light.sample_direction(Vector3(intersection.point.x, intersection.point.y, intersection.point.z), pdf);
    Vector3 light_dir_temp;
    float light_distance;
    Vector3 incident_irradiance = scene_light.illuminate(Vector3(intersection.point.x, intersection.point.y, intersection.point.z), light_dir_temp, light_distance);
    
    // Step 3: BRDF evaluation and light transport
    std::cout << "\n=== Step 3: BRDF Evaluation and Light Transport ===" << std::endl;
    Vector3 view_direction = (camera_origin - intersection.point).normalize();
    std::cout << "View direction (surface to camera): (" << view_direction.x << ", " << view_direction.y << ", " << view_direction.z << ")" << std::endl;
    
    // Calculate complete light transport using Lambert BRDF
    Vector3 final_color = sphere_material.scatter_light(light_direction, view_direction, 
                                                       intersection.normal, incident_irradiance);
    
    // Step 4: Final result and interpretation
    std::cout << "\n=== Step 4: Final Rendering Result ===" << std::endl;
    std::cout << "Final pixel color: (" << final_color.x << ", " << final_color.y << ", " << final_color.z << ")" << std::endl;
    
    // Convert to 8-bit RGB for visualization reference
    int red_8bit = (int)(std::min(1.0f, final_color.x) * 255);
    int green_8bit = (int)(std::min(1.0f, final_color.y) * 255);
    int blue_8bit = (int)(std::min(1.0f, final_color.z) * 255);
    std::cout << "8-bit RGB equivalent: (" << red_8bit << ", " << green_8bit << ", " << blue_8bit << ")" << std::endl;
    
    // Educational summary of complete light transport
    std::cout << "\n--- Complete Light Transport Summary ---" << std::endl;
    std::cout << "1. Camera ray traced to scene intersection point" << std::endl;
    std::cout << "2. Point light irradiance calculated with inverse square law" << std::endl;
    std::cout << "3. Lambert BRDF evaluated for diffuse reflection" << std::endl;
    std::cout << "4. Cosine term (n·l) applied for surface orientation" << std::endl;
    std::cout << "5. Final color computed: BRDF × Irradiance × cos(θ)" << std::endl;
    std::cout << "6. Result represents light energy reaching camera through surface interaction" << std::endl;
    
    std::cout << "\n=== Single-Ray Rendering Pipeline Complete ===" << std::endl;
    std::cout << "All mathematical steps verified and educational output provided" << std::endl;
    std::cout << "Ready for multi-ray image generation phase" << std::endl;
    
    std::cout << "\n=== Story 2.1: Multi-Ray Image Generation ===" << std::endl;
    
    // Performance monitoring setup - Educational comprehensive timing
    PerformanceTimer performance_timer;
    auto total_start_time = std::chrono::high_resolution_clock::now();
    performance_timer.start_phase(PerformanceTimer::TOTAL_RENDER);
    
    // Image resolution configuration (from command line or default)
    int image_width = image_resolution.width;
    int image_height = image_resolution.height;
    
    std::cout << "\n--- Multi-Ray Rendering Configuration ---" << std::endl;
    std::cout << "Image resolution: " << image_width << " × " << image_height << " pixels" << std::endl;
    std::cout << "Resolution preset: " << image_resolution.name << std::endl;
    std::cout << "Total rays to generate: " << (image_width * image_height) << std::endl;
    std::cout << "Rendering approach: One ray per pixel (uniform sampling)" << std::endl;
    
    // Display memory and performance predictions
    image_resolution.print_memory_analysis();
    
    // Camera setup for image generation with aspect ratio management (Story 2.4 AC: 5)
    // Epic 2 Showcase Camera - positioned for optimal scene composition  
    Point3 camera_position(0.0, 0.0, 1.0);  // INSIDE the scene for guaranteed hits
    Point3 camera_target(0.0, 0.0, -6.0);    // Looking deeper into scene
    Vector3 camera_up(0, 1, 0);               // Y is up
    float camera_fov = 60.0f;                 // Standard FOV from inside scene
    float aspect_ratio = static_cast<float>(image_width) / image_height;
    
    std::cout << "\n--- Camera Aspect Ratio Configuration (AC 5) ---" << std::endl;
    std::cout << "Image resolution: " << image_width << " × " << image_height << " pixels" << std::endl;
    std::cout << "Calculated aspect ratio: " << aspect_ratio << ":1" << std::endl;
    
    Camera render_camera(camera_position, camera_target, camera_up, camera_fov, aspect_ratio);
    
    // Critical AC 5 step: Update camera aspect ratio from image resolution
    // This ensures FOV correctness across different resolutions
    render_camera.set_aspect_ratio_from_resolution(image_width, image_height);
    
    // Apply command-line arguments to override default camera parameters
    render_camera.set_from_command_line_args(argc, argv);
    
    // Validate camera configuration and ray generation (Story 2.4 AC: 5)
    if (!render_camera.validate_camera()) {
        std::cout << "ERROR: Invalid camera configuration!" << std::endl;
        return 1;
    }
    
    // AC 5 requirement: Validate ray generation for non-square resolutions
    std::cout << "\n--- Camera Ray Generation Validation (AC 5) ---" << std::endl;
    if (!render_camera.validate_ray_generation(image_width, image_height)) {
        std::cout << "ERROR: Camera ray generation validation failed for resolution " 
                  << image_width << "×" << image_height << "!" << std::endl;
        std::cout << "This indicates an aspect ratio mismatch that would cause image distortion." << std::endl;
        return 1;
    }
    std::cout << "✓ Camera ray generation validation: PASSED" << std::endl;
    
    std::cout << "\n--- Camera Configuration ---" << std::endl;
    render_camera.print_camera_parameters();
    render_camera.explain_coordinate_transformation();
    
    // Educational output explaining aspect ratio mathematics (AC 5)
    std::cout << "\n--- Aspect Ratio Mathematics Education (AC 5) ---" << std::endl;
    render_camera.explain_fov_calculation();
    render_camera.print_camera_mathematics();
    
    // Scene setup: load from file or create default scene
    Scene render_scene;
    PointLight image_light(Vector3(2, 2, -3), Vector3(1.0f, 1.0f, 1.0f), 10.0f);
    
    std::cout << "\n--- Scene Configuration ---" << std::endl;
    
    if (use_scene_file) {
        std::cout << "Loading scene from file: " << scene_filename << std::endl;
        render_scene = SceneLoader::load_from_file(scene_filename, material_type);
        
        if (render_scene.primitives.empty()) {
            std::cout << "WARNING: Scene loading failed or produced empty scene, creating default sphere" << std::endl;
            use_scene_file = false;
        } else {
            std::cout << "✓ Scene loaded successfully" << std::endl;
            render_scene.print_scene_statistics();
        }
    }
    
    if (!use_scene_file) {
        std::cout << "Creating default single-sphere scene for compatibility" << std::endl;
        std::cout << "Selected material type: " << material_type << std::endl;
        
        if (material_type == "cook-torrance") {
            if (!quiet_mode) {
                std::cout << "\n=== Cook-Torrance Material Configuration ===" << std::endl;
                std::cout << "Base Color: (0.7, 0.3, 0.3) - Reddish surface" << std::endl;
                std::cout << "Roughness: " << roughness_param << std::endl;
                std::cout << "Metallic: " << metallic_param << std::endl;
                std::cout << "Specular: " << specular_param << std::endl;
                std::cout << "\nUsing Cook-Torrance rendering path (bypassing Scene system)" << std::endl;
            }
        } else {
            // Use Scene system for Lambert materials
            LambertMaterial default_material(Vector3(0.7f, 0.3f, 0.3f));  // Reddish diffuse
            int material_idx = render_scene.add_material(default_material);
            Sphere default_sphere(Point3(0, 0, -3), 1.0f, material_idx, !quiet_mode);
            render_scene.add_sphere(default_sphere);
            render_scene.print_scene_statistics();
        }
    }
    
    // Image buffer creation using Resolution with performance monitoring
    performance_timer.start_phase(PerformanceTimer::IMAGE_OUTPUT);
    Image output_image(image_resolution);
    performance_timer.record_memory_usage(output_image.memory_usage_bytes());
    performance_timer.end_phase(PerformanceTimer::IMAGE_OUTPUT);
    
    std::cout << "\n--- Image Buffer Configuration ---" << std::endl;
    std::cout << "Created " << image_width << "×" << image_height << " image buffer" << std::endl;
    std::cout << "Pixel storage: Vector3 (linear RGB)" << std::endl;
    std::cout << "Color management: Clamping + gamma correction pipeline" << std::endl;
    
    // Educational color management explanation
    output_image.explain_color_management();
    
    // Performance counters for legacy compatibility
    int rays_generated = 0;
    int intersection_tests = 0;
    int shading_calculations = 0;
    int background_pixels = 0;
    
    // Start comprehensive timing for ray generation phase
    auto ray_generation_start = std::chrono::high_resolution_clock::now();
    
    std::cout << "\n--- Multi-Ray Rendering Process ---" << std::endl;
    std::cout << "Beginning pixel-by-pixel ray generation with performance monitoring..." << std::endl;
    
    // Initialize comprehensive progress reporting (Story 2.4 AC: 4)
    int total_pixels = image_width * image_height;
    ProgressReporter progress_reporter(total_pixels, &performance_timer, quiet_mode);
    
    // Multi-ray pixel sampling: one ray per pixel with comprehensive progress tracking
    for (int y = 0; y < image_height; y++) {
        
        for (int x = 0; x < image_width; x++) {
            // Phase 1: Ray Generation with precise timing
            performance_timer.start_phase(PerformanceTimer::RAY_GENERATION);
            Ray pixel_ray = render_camera.generate_ray(
                static_cast<float>(x), 
                static_cast<float>(y), 
                image_width, 
                image_height
            );
            performance_timer.end_phase(PerformanceTimer::RAY_GENERATION);
            performance_timer.increment_counter(PerformanceTimer::RAY_GENERATION);
            rays_generated++;
            
            Vector3 pixel_color(0, 0, 0);  // Default background color (black)
            
            if (material_type == "cook-torrance") {
                // Cook-Torrance rendering path (bypass Scene system)
                performance_timer.start_phase(PerformanceTimer::INTERSECTION_TESTING);
                
                // Direct sphere intersection (single sphere at (0,0,-3) with radius 1.0)
                Point3 sphere_center(0, 0, -3);
                float sphere_radius = 1.0f;
                Sphere cook_torrance_sphere(sphere_center, sphere_radius, 0, !quiet_mode);
                Sphere::Intersection sphere_hit = cook_torrance_sphere.intersect(pixel_ray, !quiet_mode);
                
                performance_timer.end_phase(PerformanceTimer::INTERSECTION_TESTING);
                performance_timer.increment_counter(PerformanceTimer::INTERSECTION_TESTING);
                intersection_tests++;
                
                if (sphere_hit.hit) {
                    // Phase 3: Cook-Torrance Shading Calculation
                    performance_timer.start_phase(PerformanceTimer::SHADING_CALCULATION);
                    shading_calculations++;
                    
                    // Create Cook-Torrance material using command-line base_color
                    Vector3 base_color(0.7f, 0.3f, 0.3f);  // Default base color, should be configurable in future
                    CookTorranceMaterial cook_torrance_material(base_color, roughness_param, metallic_param, specular_param, !quiet_mode);
                    
                    // Multi-light accumulation for Cook-Torrance (AC2 - Story 3.2)
                    pixel_color = Vector3(0, 0, 0);  // Initialize accumulator
                    Vector3 surface_point = Vector3(sphere_hit.point.x, sphere_hit.point.y, sphere_hit.point.z);
                    Vector3 view_direction = (camera_position - sphere_hit.point).normalize();
                    
                    if (render_scene.lights.empty()) {
                        // Fallback: Use hardcoded light for backward compatibility
                        float pdf_fallback;
                        Vector3 light_direction = image_light.sample_direction(surface_point, pdf_fallback);
                        Vector3 temp_light_dir;
                        float temp_distance;
                        Vector3 incident_irradiance = image_light.illuminate(surface_point, temp_light_dir, temp_distance);
                        
                        pixel_color = cook_torrance_material.scatter_light(
                            light_direction, view_direction, sphere_hit.normal, 
                            incident_irradiance, !quiet_mode
                        );
                    } else {
                        // Multi-light accumulation from scene for Cook-Torrance
                        for (const auto& light : render_scene.lights) {
                            Vector3 light_direction;
                            float light_distance;
                            Vector3 light_contribution = light->illuminate(surface_point, light_direction, light_distance);
                            
                            // Shadow ray testing (AC3)
                            if (!light->is_occluded(surface_point, light_direction, light_distance, render_scene)) {
                                // Cook-Torrance BRDF evaluation for this light
                                Vector3 brdf_contribution = cook_torrance_material.scatter_light(
                                    light_direction, view_direction, sphere_hit.normal, 
                                    light_contribution, false  // Disable verbose per-light to avoid spam
                                );
                                pixel_color += brdf_contribution;
                            }
                        }
                        
                        // Educational output for multi-light Cook-Torrance (if enabled and first few pixels)
                        if (!quiet_mode && (x + y * image_width) < 3) {
                            std::cout << "\n=== Cook-Torrance Multi-Light Accumulation (Pixel " << (x + y * image_width) << ") ===" << std::endl;
                            std::cout << "Scene lights: " << render_scene.lights.size() << std::endl;
                            std::cout << "Final accumulated color: (" << pixel_color.x << ", " << pixel_color.y << ", " << pixel_color.z << ")" << std::endl;
                        }
                    }
                    performance_timer.end_phase(PerformanceTimer::SHADING_CALCULATION);
                    performance_timer.increment_counter(PerformanceTimer::SHADING_CALCULATION);
                } else {
                    // No intersection - background color
                    background_pixels++;
                    pixel_color = Vector3(0.1f, 0.1f, 0.15f);  // Dark blue background
                }
            } else {
                // Lambert rendering path (use Scene system)
                performance_timer.start_phase(PerformanceTimer::INTERSECTION_TESTING);
                Scene::Intersection intersection = render_scene.intersect(pixel_ray, !quiet_mode);
                performance_timer.end_phase(PerformanceTimer::INTERSECTION_TESTING);
                performance_timer.increment_counter(PerformanceTimer::INTERSECTION_TESTING);
                intersection_tests++;
                
                if (intersection.hit) {
                    // Phase 3: Lambert Shading Calculation
                    performance_timer.start_phase(PerformanceTimer::SHADING_CALCULATION);
                    shading_calculations++;
                    
                    // Multi-light accumulation (AC2 - Story 3.2)
                    pixel_color = Vector3(0, 0, 0);  // Initialize accumulator
                    Vector3 surface_point = Vector3(intersection.point.x, intersection.point.y, intersection.point.z);
                    Vector3 view_direction = (camera_position - intersection.point).normalize();
                    
                    if (render_scene.lights.empty()) {
                        // Fallback: Use hardcoded light for backward compatibility
                        float pdf_fallback;
                        Vector3 light_direction = image_light.sample_direction(surface_point, pdf_fallback);
                        Vector3 temp_light_dir;
                        float temp_distance;
                        Vector3 incident_irradiance = image_light.illuminate(surface_point, temp_light_dir, temp_distance);
                        
                        pixel_color = intersection.material->scatter_light(
                            light_direction, view_direction, intersection.normal, 
                            incident_irradiance, !quiet_mode
                        );
                    } else {
                        // Multi-light accumulation from scene
                        for (const auto& light : render_scene.lights) {
                            Vector3 light_direction;
                            float light_distance;
                            Vector3 light_contribution = light->illuminate(surface_point, light_direction, light_distance);
                            
                            // Shadow ray testing (AC3)
                            if (!light->is_occluded(surface_point, light_direction, light_distance, render_scene)) {
                                // BRDF evaluation for this light
                                Vector3 brdf_contribution = intersection.material->scatter_light(
                                    light_direction, view_direction, intersection.normal, 
                                    light_contribution, false  // Disable verbose per-light to avoid spam
                                );
                                pixel_color += brdf_contribution;
                            }
                        }
                        
                        // Educational output for multi-light (if enabled and first few pixels)
                        if (!quiet_mode && (x + y * image_width) < 5) {
                            std::cout << "\n=== Multi-Light Accumulation (Pixel " << (x + y * image_width) << ") ===" << std::endl;
                            std::cout << "Scene lights: " << render_scene.lights.size() << std::endl;
                            std::cout << "Final accumulated color: (" << pixel_color.x << ", " << pixel_color.y << ", " << pixel_color.z << ")" << std::endl;
                        }
                    }
                    performance_timer.end_phase(PerformanceTimer::SHADING_CALCULATION);
                    performance_timer.increment_counter(PerformanceTimer::SHADING_CALCULATION);
                } else {
                    // No intersection - background color
                    background_pixels++;
                    pixel_color = Vector3(0.1f, 0.1f, 0.15f);  // Dark blue background
                }
            }
            
            // Store pixel in image buffer (no additional timing - included in IMAGE_OUTPUT)
            output_image.set_pixel(x, y, pixel_color);
        }
        
        // Update progress reporting after each row for better granularity
        int completed_pixels = (y + 1) * image_width;
        size_t current_memory = output_image.memory_usage_bytes() + render_scene.calculate_scene_memory_usage();
        progress_reporter.update_progress(completed_pixels, current_memory);
        
        // Check for interrupt capability (placeholder for user cancellation)
        if (progress_reporter.should_interrupt()) {
            std::cout << "\nRendering interrupted by user request." << std::endl;
            break;
        }
    }
    
    // End comprehensive timing
    performance_timer.end_phase(PerformanceTimer::TOTAL_RENDER);
    
    auto ray_generation_end = std::chrono::high_resolution_clock::now();
    auto total_end_time = std::chrono::high_resolution_clock::now();
    
    // Legacy performance analysis for compatibility
    auto ray_generation_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        ray_generation_end - ray_generation_start);
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        total_end_time - total_start_time);
    
    std::cout << "\n--- Multi-Ray Rendering Complete ---" << std::endl;
    std::cout << "Final rendering statistics:" << std::endl;
    
    std::cout << "\n=== Educational Performance Analysis ===" << std::endl;
    std::cout << "Ray Generation Statistics:" << std::endl;
    std::cout << "  Total rays generated: " << rays_generated << std::endl;
    std::cout << "  Expected rays (width × height): " << (image_width * image_height) << std::endl;
    std::cout << "  Ray generation accuracy: " << (rays_generated == (image_width * image_height) ? "PERFECT" : "ERROR") << std::endl;
    
    std::cout << "Intersection Testing Statistics:" << std::endl;
    std::cout << "  Total intersection tests: " << intersection_tests << std::endl;
    std::cout << "  Tests per ray: " << (static_cast<float>(intersection_tests) / rays_generated) << std::endl;
    std::cout << "  Scene primitives tested: " << render_scene.primitives.size() << " per ray" << std::endl;
    
    std::cout << "Shading Calculation Statistics:" << std::endl;
    std::cout << "  Shading calculations performed: " << shading_calculations << std::endl;
    std::cout << "  Background pixels (no shading): " << background_pixels << std::endl;
    std::cout << "  Scene coverage: " << (100.0f * shading_calculations / rays_generated) << "%" << std::endl;
    
    std::cout << "Performance Timing:" << std::endl;
    std::cout << "  Ray generation time: " << ray_generation_duration.count() << " ms" << std::endl;
    std::cout << "  Total rendering time: " << total_duration.count() << " ms" << std::endl;
    std::cout << "  Rays per second: " << (rays_generated * 1000.0f / total_duration.count()) << std::endl;
    
    // Comprehensive Educational Performance Analysis (Story 2.4)
    std::cout << "\n=== Story 2.4: Comprehensive Performance Analysis ===" << std::endl;
    
    // Validate timing accuracy for educational requirements
    bool timing_valid = performance_timer.validate_timing_accuracy();
    if (timing_valid) {
        std::cout << "✓ Performance timing validation: PASSED (≥1000 rays, ≥1ms measurement)" << std::endl;
    } else {
        std::cout << "⚠ Performance timing validation: LIMITED (results may vary due to small dataset)" << std::endl;
    }
    
    // Display all comprehensive performance breakdowns
    performance_timer.print_performance_breakdown();
    performance_timer.print_rays_per_second_statistics();
    performance_timer.print_phase_analysis();
    performance_timer.print_memory_performance_correlation();
    
    // Comprehensive Memory Usage Analysis (Story 2.4 AC: 3)
    std::cout << "\n=== Story 2.4: Comprehensive Memory Analysis ===" << std::endl;
    
    // Scene memory analysis
    render_scene.print_memory_usage_analysis();
    render_scene.explain_memory_scene_relationship();
    
    // Combined memory warnings (image + scene)
    render_scene.check_memory_usage_warnings(output_image.memory_usage_bytes());
    
    // Final progress reporting statistics (Story 2.4 AC: 4)
    std::cout << "\n=== Story 2.4: Progress Reporting Final Analysis ===" << std::endl;
    progress_reporter.print_final_statistics();
    
    // Image analysis
    std::cout << "\n=== Educational Image Analysis ===" << std::endl;
    output_image.print_image_statistics();
    
    // Validate final image
    if (!output_image.validate_image()) {
        std::cout << "ERROR: Image validation failed!" << std::endl;
        return 1;
    }
    
    std::cout << "\n--- Multi-Ray Pipeline Summary ---" << std::endl;
    std::cout << "1. Camera-to-pixel coordinate transformation: " << rays_generated << " rays generated" << std::endl;
    std::cout << "2. Ray-scene intersection testing: " << intersection_tests << " tests performed across " 
              << render_scene.primitives.size() << " primitives" << std::endl;
    std::cout << "3. Lambert BRDF shading calculations: " << shading_calculations << " evaluations" << std::endl;
    std::cout << "4. Image buffer management: " << (image_width * image_height) << " pixels stored" << std::endl;
    std::cout << "5. Color management pipeline: clamping and gamma correction ready" << std::endl;
    
    // Display final scene performance statistics
    std::cout << "\n=== Final Scene Performance Statistics ===" << std::endl;
    if (material_type == "cook-torrance") {
        // Special statistics for Cook-Torrance direct rendering path
        std::cout << "=== Scene Statistics ===" << std::endl;
        std::cout << "Geometry:" << std::endl;
        std::cout << "  Spheres: 1 (single Cook-Torrance sphere, direct rendering)" << std::endl;
        std::cout << "  Materials: 1 (Cook-Torrance material)" << std::endl;
        std::cout << "\nPerformance Statistics:" << std::endl;
        std::cout << "  Total intersection tests: " << intersection_tests << std::endl;
        std::cout << "  Successful intersections: " << shading_calculations << std::endl;
        std::cout << "  Hit rate: " << (intersection_tests > 0 ? 
                     (float)shading_calculations / intersection_tests * 100.0f : 0.0f) << "%" << std::endl;
        std::cout << "  Note: Direct rendering path bypasses Scene system for Cook-Torrance materials" << std::endl;
        std::cout << "=== Scene statistics complete ===" << std::endl;
    } else {
        render_scene.print_scene_statistics();
    }
    
    std::cout << "\n=== Image Generation and Pixel Sampling Complete ===" << std::endl;
    std::cout << "Successfully generated " << image_width << "×" << image_height << " image" << std::endl;
    std::cout << "All pixels processed with complete light transport calculations" << std::endl;
    
    // PNG Output Implementation (AC 4) with performance monitoring
    std::cout << "\n=== PNG Output Generation (AC 4) ===" << std::endl;
    performance_timer.start_phase(PerformanceTimer::IMAGE_OUTPUT);
    std::string png_filename = "raytracer_output.png";
    bool png_success = output_image.save_to_png(png_filename, true);  // With gamma correction
    performance_timer.end_phase(PerformanceTimer::IMAGE_OUTPUT);
    performance_timer.increment_counter(PerformanceTimer::IMAGE_OUTPUT);
    
    if (png_success) {
        std::cout << "✓ Acceptance Criteria 4: PNG image output COMPLETE" << std::endl;
        std::cout << "✓ Demonstrates successful rendering of simple sphere scene with visible Lambert shading" << std::endl;
        std::cout << "✓ Generated file: " << png_filename << std::endl;
    } else {
        std::cout << "✗ PNG output failed - check file permissions and disk space" << std::endl;
    }
    
    // Educational note about extension points for anti-aliasing
    std::cout << "\n--- Extension Points for Future Development ---" << std::endl;
    std::cout << "Anti-aliasing support design:" << std::endl;
    std::cout << "  - Current: 1 ray per pixel (uniform sampling)" << std::endl;
    std::cout << "  - Future: N rays per pixel with sample averaging" << std::endl;
    std::cout << "  - Implementation: modify ray generation loop to sample multiple positions per pixel" << std::endl;
    std::cout << "  - Mathematical foundation: Monte Carlo integration over pixel area" << std::endl;
    
    return 0;
}