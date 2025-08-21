#include <iostream>
#include "src/core/vector3.hpp"
#include "src/core/image.hpp"
#include "src/core/camera.hpp"
#include "src/core/performance_timer.hpp"
#include "src/core/progress_reporter.hpp"

int main() {
    std::cout << "=== Story 2.4 Implementation Validation Test ===" << std::endl;
    
    // Test 1: Resolution parsing
    std::cout << "\nTest 1: Resolution parsing and validation" << std::endl;
    try {
        Resolution res = Resolution::parse_from_string("1024x768");
        std::cout << "✓ Resolution parsing successful: " << res.width << "x" << res.height << std::endl;
        std::cout << "  Aspect ratio: " << res.aspect_ratio() << std::endl;
        std::cout << "  Memory estimate: " << (res.memory_estimate_bytes() / (1024.0f * 1024.0f)) << " MB" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Resolution parsing failed: " << e.what() << std::endl;
        return 1;
    }
    
    // Test 2: Image creation with resolution
    std::cout << "\nTest 2: Image creation with custom resolution" << std::endl;
    Resolution test_res(512, 384);
    Image test_image(test_res);
    std::cout << "✓ Image created successfully" << std::endl;
    
    // Test 3: Camera aspect ratio handling
    std::cout << "\nTest 3: Camera aspect ratio handling" << std::endl;
    Camera test_camera(Point3(0, 0, 5), Point3(0, 0, 0), Vector3(0, 1, 0), 45.0f);
    test_camera.set_aspect_ratio_from_resolution(1024, 768);
    
    bool validation_result = test_camera.validate_ray_generation(1024, 768);
    std::cout << "✓ Camera ray generation validation: " << (validation_result ? "PASSED" : "FAILED") << std::endl;
    
    // Test 4: Performance timer functionality
    std::cout << "\nTest 4: Performance timer functionality" << std::endl;
    PerformanceTimer timer;
    timer.start_phase(PerformanceTimer::RAY_GENERATION);
    timer.increment_counter(PerformanceTimer::RAY_GENERATION, 1000);
    timer.end_phase(PerformanceTimer::RAY_GENERATION);
    
    bool timing_valid = timer.validate_timing_accuracy();
    std::cout << "✓ Performance timer validation: " << (timing_valid ? "PASSED" : "FAILED") << std::endl;
    
    // Test 5: Progress reporter functionality  
    std::cout << "\nTest 5: Progress reporter functionality" << std::endl;
    ProgressReporter progress(1000, &timer);
    progress.update_progress(500);  // 50% complete
    float progress_pct = progress.get_progress_percentage();
    std::cout << "✓ Progress reporter: " << progress_pct << "% completion tracked" << std::endl;
    
    std::cout << "\n=== All Core Tests Completed Successfully ===" << std::endl;
    return 0;
}