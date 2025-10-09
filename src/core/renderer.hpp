#pragma once

#include "vector3.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "logger.hpp"
#include "performance_timer.hpp"
#include "progress_reporter.hpp"
#include "constants.hpp"
#include <iostream>

// Renderer class encapsulates the core ray tracing rendering logic
// Extracted from main.cpp lambda to improve code organization and testability
class Renderer {
public:
    // Rendering statistics struct - consolidated performance counters
    struct Stats {
        int rays_generated = 0;
        int intersection_tests = 0;
        int shading_calculations = 0;
        int background_pixels = 0;

        void reset() {
            rays_generated = 0;
            intersection_tests = 0;
            shading_calculations = 0;
            background_pixels = 0;
        }
    };

    // Public stats for access after rendering
    Stats stats;

    // Constructor with all required dependencies
    Renderer(Camera& camera,
             Scene& scene,
             const Point3& camera_pos,
             PerformanceTimer& perf_timer)
        : render_camera(camera),
          render_scene(scene),
          camera_position(camera_pos),
          performance_timer(perf_timer) {}

    // Main rendering method
    // width, height: output image dimensions
    // Always uses Scene system for unified rendering
    Image render(int width, int height) {
        Image rendered_image(width, height);

        std::cout << "\n--- Multi-Ray Rendering Process ---" << std::endl;
        std::cout << "Beginning pixel-by-pixel ray generation with performance monitoring..." << std::endl;

        // Initialize comprehensive progress reporting (Story 2.4 AC: 4)
        int local_total_pixels = width * height;
        ProgressReporter local_progress_reporter(local_total_pixels, &performance_timer, Logger::is_quiet());

        // Performance counters for this render
        int local_rays_generated = 0;
        int local_intersection_tests = 0;
        int local_shading_calculations = 0;
        int local_background_pixels = 0;

        // Multi-ray pixel sampling: one ray per pixel with comprehensive progress tracking
        for (int y = 0; y < height; y++) {

            for (int x = 0; x < width; x++) {
                // Phase 1: Ray Generation with precise timing
                performance_timer.start_phase(PerformanceTimer::RAY_GENERATION);
                Ray pixel_ray = render_camera.generate_ray(
                    static_cast<float>(x),
                    static_cast<float>(y),
                    width,
                    height
                );
                performance_timer.end_phase(PerformanceTimer::RAY_GENERATION);
                performance_timer.increment_counter(PerformanceTimer::RAY_GENERATION);
                local_rays_generated++;

                // Unified Scene-based rendering path
                Vector3 pixel_color = render_with_scene_materials(pixel_ray, x, y, width,
                                                          local_intersection_tests,
                                                          local_shading_calculations,
                                                          local_background_pixels);

                // Store pixel in image buffer (no additional timing - included in IMAGE_OUTPUT)
                rendered_image.set_pixel(x, y, pixel_color);
            }

            // Update progress reporting after each row for better granularity
            int completed_pixels = (y + 1) * width;
            size_t current_memory = rendered_image.memory_usage_bytes() + render_scene.calculate_scene_memory_usage();
            local_progress_reporter.update_progress(completed_pixels, current_memory);

            // Check for interrupt capability (placeholder for user cancellation)
            if (local_progress_reporter.should_interrupt()) {
                std::cout << "\nRendering interrupted by user request." << std::endl;
                break;
            }
        }

        // Update global counters
        stats.rays_generated += local_rays_generated;
        stats.intersection_tests += local_intersection_tests;
        stats.shading_calculations += local_shading_calculations;
        stats.background_pixels += local_background_pixels;

        return rendered_image;
    }

private:
    Camera& render_camera;
    Scene& render_scene;
    Point3 camera_position;
    PerformanceTimer& performance_timer;

    // Scene-based rendering path
    Vector3 render_with_scene_materials(const Ray& pixel_ray, int x, int y, int width,
                                        int& local_intersection_tests,
                                        int& local_shading_calculations,
                                        int& local_background_pixels) {
        performance_timer.start_phase(PerformanceTimer::INTERSECTION_TESTING);
        Scene::Intersection intersection = render_scene.intersect(pixel_ray);
        performance_timer.end_phase(PerformanceTimer::INTERSECTION_TESTING);
        performance_timer.increment_counter(PerformanceTimer::INTERSECTION_TESTING);
        local_intersection_tests++;

        if (intersection.hit) {
            // Phase 3: Shading Calculation using scene materials
            performance_timer.start_phase(PerformanceTimer::SHADING_CALCULATION);
            local_shading_calculations++;

            // Multi-light accumulation (AC2 - Story 3.2)
            Vector3 pixel_color(0, 0, 0);  // Initialize accumulator
            Vector3 surface_point = Vector3(intersection.point.x, intersection.point.y, intersection.point.z);
            Vector3 view_direction = (camera_position - intersection.point).normalize();

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
                        light_contribution
                    );
                    pixel_color += brdf_contribution;
                }
            }

            // Educational output for multi-light (if enabled and first few pixels)
            if (Logger::is_verbose() && (x + y * width) < 5) {
                std::cout << "\n=== Multi-Light Accumulation (Pixel " << (x + y * width) << ") ===" << std::endl;
                std::cout << "Scene lights: " << render_scene.lights.size() << std::endl;
                std::cout << "Final accumulated color: (" << pixel_color.x << ", " << pixel_color.y << ", " << pixel_color.z << ")" << std::endl;
            }
            performance_timer.end_phase(PerformanceTimer::SHADING_CALCULATION);
            performance_timer.increment_counter(PerformanceTimer::SHADING_CALCULATION);

            return pixel_color;
        } else {
            // No intersection - background color
            local_background_pixels++;
            return Constants::BACKGROUND_COLOR;
        }
    }
};
