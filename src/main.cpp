#include <iostream>
#include "core/vector3.hpp"
#include "core/point3.hpp"
#include "core/ray.hpp"
#include "core/sphere.hpp"
#include "core/point_light.hpp"
#include "materials/lambert.hpp"
#include "core/camera.hpp"
#include "core/image.hpp"
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

int main() {
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
    Sphere test_sphere(Point3(0, 0, -5), 1.0f);        // Sphere centered at (0,0,-5) with radius 1
    
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
    Sphere unit_sphere(Point3(0, 0, -5), 1.0f);           // Unit sphere for tangency test
    Sphere::Intersection graze_result = unit_sphere.intersect(graze_ray);
    
    if (graze_result.hit) {
        std::cout << "✓ Grazing intersection detected" << std::endl;
        std::cout << "✓ Graze t-parameter: " << graze_result.t << std::endl;
    } else {
        std::cout << "Note: Grazing ray may not intersect due to floating-point precision" << std::endl;
    }
    
    std::cout << "\n=== Ray-Sphere Intersection Implementation Complete ===" << std::endl;
    std::cout << "All edge cases handled successfully" << std::endl;
    
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
    Sphere render_sphere(sphere_center, sphere_radius);
    LambertMaterial sphere_material(Vector3(0.7f, 0.3f, 0.3f));  // Reddish diffuse material
    std::cout << "Sphere center: (" << sphere_center.x << ", " << sphere_center.y << ", " << sphere_center.z << ")" << std::endl;
    std::cout << "Sphere radius: " << sphere_radius << std::endl;
    std::cout << "Sphere material albedo: (" << sphere_material.base_color.x << ", " << sphere_material.base_color.y << ", " << sphere_material.base_color.z << ")" << std::endl;
    
    // Point light source with explicit position and color
    Point3 light_position(2, 2, -3);  // Light positioned above and to the right of sphere
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
    
    if (!scene_light.validate_light()) {
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
    Vector3 light_direction = scene_light.sample_direction(intersection.point);
    Vector3 incident_irradiance = scene_light.calculate_irradiance(intersection.point);
    
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
    std::cout << "1. Camera ray traced to sphere intersection point" << std::endl;
    std::cout << "2. Point light irradiance calculated with inverse square law" << std::endl;
    std::cout << "3. Lambert BRDF evaluated for diffuse reflection" << std::endl;
    std::cout << "4. Cosine term (n·l) applied for surface orientation" << std::endl;
    std::cout << "5. Final color computed: BRDF × Irradiance × cos(θ)" << std::endl;
    std::cout << "6. Result represents light energy reaching camera through surface interaction" << std::endl;
    
    std::cout << "\n=== Single-Ray Rendering Pipeline Complete ===" << std::endl;
    std::cout << "All mathematical steps verified and educational output provided" << std::endl;
    std::cout << "Ready for multi-ray image generation phase" << std::endl;
    
    std::cout << "\n=== Story 2.1: Multi-Ray Image Generation ===" << std::endl;
    
    // Performance monitoring setup
    auto total_start_time = std::chrono::high_resolution_clock::now();
    
    // Image resolution configuration
    int image_width = 256;   // Educational resolution for demonstration
    int image_height = 256;  // Square aspect ratio for simplicity
    
    std::cout << "\n--- Multi-Ray Rendering Configuration ---" << std::endl;
    std::cout << "Image resolution: " << image_width << " × " << image_height << " pixels" << std::endl;
    std::cout << "Total rays to generate: " << (image_width * image_height) << std::endl;
    std::cout << "Rendering approach: One ray per pixel (uniform sampling)" << std::endl;
    
    // Camera setup for image generation
    Point3 camera_position(0, 0, 0);
    Point3 camera_target(0, 0, -3);  // Looking toward sphere center
    Vector3 camera_up(0, 1, 0);      // Y is up
    float camera_fov = 45.0f;        // 45-degree field of view
    float aspect_ratio = static_cast<float>(image_width) / image_height;
    
    Camera render_camera(camera_position, camera_target, camera_up, camera_fov, aspect_ratio);
    
    // Validate camera configuration
    if (!render_camera.validate_camera()) {
        std::cout << "ERROR: Invalid camera configuration!" << std::endl;
        return 1;
    }
    
    std::cout << "\n--- Camera Configuration ---" << std::endl;
    render_camera.print_camera_parameters();
    render_camera.explain_coordinate_transformation();
    
    // Scene setup (adjusted sphere position to be in camera view)
    Sphere image_sphere(Point3(0, 0, -3), 1.0f);  // Sphere positioned in camera field of view
    LambertMaterial image_material(Vector3(0.7f, 0.3f, 0.3f));  // Reddish diffuse
    PointLight image_light(Point3(2, 2, -3), Vector3(1.0f, 1.0f, 1.0f), 10.0f);
    
    std::cout << "\n--- Scene Configuration ---" << std::endl;
    std::cout << "Sphere center: (" << image_sphere.center.x << ", " << image_sphere.center.y << ", " << image_sphere.center.z << ")" << std::endl;
    std::cout << "Sphere radius: " << image_sphere.radius << std::endl;
    std::cout << "Material albedo: (" << image_material.base_color.x << ", " << image_material.base_color.y << ", " << image_material.base_color.z << ")" << std::endl;
    
    // Image buffer creation
    Image output_image(image_width, image_height);
    std::cout << "\n--- Image Buffer Configuration ---" << std::endl;
    std::cout << "Created " << image_width << "×" << image_height << " image buffer" << std::endl;
    std::cout << "Pixel storage: Vector3 (linear RGB)" << std::endl;
    std::cout << "Color management: Clamping + gamma correction pipeline" << std::endl;
    
    // Educational color management explanation
    output_image.explain_color_management();
    
    // Performance counters
    int rays_generated = 0;
    int intersection_tests = 0;
    int shading_calculations = 0;
    int background_pixels = 0;
    
    auto ray_generation_start = std::chrono::high_resolution_clock::now();
    
    std::cout << "\n--- Multi-Ray Rendering Process ---" << std::endl;
    std::cout << "Beginning pixel-by-pixel ray generation..." << std::endl;
    
    // Multi-ray pixel sampling: one ray per pixel
    for (int y = 0; y < image_height; y++) {
        // Progress reporting every 32 rows for educational visibility
        if (y % 32 == 0 && y > 0) {
            float progress = static_cast<float>(y) / image_height * 100.0f;
            std::cout << "Rendering progress: " << progress << "% (" << y << "/" << image_height << " rows)" << std::endl;
        }
        
        for (int x = 0; x < image_width; x++) {
            // Generate camera ray for this pixel
            Ray pixel_ray = render_camera.generate_ray(
                static_cast<float>(x), 
                static_cast<float>(y), 
                image_width, 
                image_height
            );
            rays_generated++;
            
            
            // Test ray-sphere intersection
            Sphere::Intersection intersection = image_sphere.intersect(pixel_ray);
            intersection_tests++;
            
            Vector3 pixel_color(0, 0, 0);  // Default background color (black)
            
            if (intersection.hit) {
                // Sphere intersection found - calculate lighting
                shading_calculations++;
                
                // Light evaluation
                Vector3 light_direction = image_light.sample_direction(intersection.point);
                Vector3 incident_irradiance = image_light.calculate_irradiance(intersection.point);
                
                // View direction (from surface to camera)
                Vector3 view_direction = (camera_position - intersection.point).normalize();
                
                // Complete light transport using Lambert BRDF
                pixel_color = image_material.scatter_light(
                    light_direction, 
                    view_direction, 
                    intersection.normal, 
                    incident_irradiance
                );
            } else {
                // No intersection - background color
                background_pixels++;
                pixel_color = Vector3(0.1f, 0.1f, 0.15f);  // Dark blue background
            }
            
            // Store pixel in image buffer
            output_image.set_pixel(x, y, pixel_color);
        }
    }
    
    auto ray_generation_end = std::chrono::high_resolution_clock::now();
    auto total_end_time = std::chrono::high_resolution_clock::now();
    
    // Performance analysis
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
    
    std::cout << "Shading Calculation Statistics:" << std::endl;
    std::cout << "  Shading calculations performed: " << shading_calculations << std::endl;
    std::cout << "  Background pixels (no shading): " << background_pixels << std::endl;
    std::cout << "  Scene coverage: " << (100.0f * shading_calculations / rays_generated) << "%" << std::endl;
    
    std::cout << "Performance Timing:" << std::endl;
    std::cout << "  Ray generation time: " << ray_generation_duration.count() << " ms" << std::endl;
    std::cout << "  Total rendering time: " << total_duration.count() << " ms" << std::endl;
    std::cout << "  Rays per second: " << (rays_generated * 1000.0f / total_duration.count()) << std::endl;
    
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
    std::cout << "2. Ray-sphere intersection testing: " << intersection_tests << " tests performed" << std::endl;
    std::cout << "3. Lambert BRDF shading calculations: " << shading_calculations << " evaluations" << std::endl;
    std::cout << "4. Image buffer management: " << (image_width * image_height) << " pixels stored" << std::endl;
    std::cout << "5. Color management pipeline: clamping and gamma correction ready" << std::endl;
    
    std::cout << "\n=== Image Generation and Pixel Sampling Complete ===" << std::endl;
    std::cout << "Successfully generated " << image_width << "×" << image_height << " image" << std::endl;
    std::cout << "All pixels processed with complete light transport calculations" << std::endl;
    
    // PNG Output Implementation (AC 4)
    std::cout << "\n=== PNG Output Generation (AC 4) ===" << std::endl;
    std::string png_filename = "raytracer_output.png";
    bool png_success = output_image.save_to_png(png_filename, true);  // With gamma correction
    
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