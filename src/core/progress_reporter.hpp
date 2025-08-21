#pragma once
#include "performance_timer.hpp"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

// ProgressReporter class for educational rendering progress tracking with ETA and performance insights
// Designed for larger image resolutions where rendering time becomes significant
//
// Features:
// - Progress percentage calculation with 5% reporting granularity
// - Educational timing breakdowns during rendering
// - Estimated completion time (ETA) calculations  
// - Scanline-based progress reporting for educational visualization
// - Performance scaling insights and memory pressure warnings
// - Interrupt capability for long renders with clean termination
//
// Educational focus:
// - Demonstrates relationship between resolution and rendering time
// - Shows performance scaling characteristics in real-time
// - Provides insights into bottlenecks during rendering process
// - Explains mathematical relationships between progress and completion time
//
// Mathematical foundation:
// - Progress percentage = (completed_pixels / total_pixels) * 100
// - ETA calculation = (elapsed_time / completed_pixels) * remaining_pixels
// - Rendering rate = completed_pixels / elapsed_time_seconds
// - Memory pressure = current_memory_usage / available_system_memory
class ProgressReporter {
private:
    int total_pixels;
    int completed_pixels;
    int last_reported_percentage;
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point last_update_time;
    PerformanceTimer* timer;
    
    // Progress reporting configuration
    int reporting_granularity_percent = 5;  // Report every 5%
    float minimum_reporting_interval_seconds = 2.0f;  // Minimum 2 seconds between reports
    
    // Educational statistics
    float pixels_per_second = 0.0f;
    float estimated_total_time_seconds = 0.0f;
    float estimated_remaining_time_seconds = 0.0f;
    
    // Memory monitoring for progress correlation
    size_t initial_memory_usage = 0;
    size_t current_memory_usage = 0;
    
public:
    ProgressReporter(int total_pixels, PerformanceTimer* performance_timer) 
        : total_pixels(total_pixels), completed_pixels(0), last_reported_percentage(-1),
          timer(performance_timer) {
        start_time = std::chrono::steady_clock::now();
        last_update_time = start_time;
        
        std::cout << "\n=== Progress Reporting Initialized ===" << std::endl;
        std::cout << "Total pixels to render: " << total_pixels << std::endl;
        std::cout << "Progress reporting granularity: every " << reporting_granularity_percent << "%" << std::endl;
        std::cout << "Minimum reporting interval: " << minimum_reporting_interval_seconds << " seconds" << std::endl;
        std::cout << "Educational insights: Performance scaling and ETA calculation enabled" << std::endl;
        std::cout << "=== Begin Rendering Progress ===" << std::endl;
    }
    
    // Update progress with completed pixel count and optional memory usage
    void update_progress(int pixels_completed, size_t memory_bytes = 0) {
        completed_pixels = pixels_completed;
        current_memory_usage = memory_bytes;
        
        // Calculate current progress percentage
        float progress_percentage = (static_cast<float>(completed_pixels) / total_pixels) * 100.0f;
        int progress_percent_int = static_cast<int>(progress_percentage);
        
        // Calculate elapsed time
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);
        float elapsed_seconds = elapsed_duration.count() / 1000.0f;
        
        // Check if we should report (percentage threshold OR time threshold)
        bool percentage_threshold_met = (progress_percent_int >= last_reported_percentage + reporting_granularity_percent);
        
        auto time_since_last_report = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_update_time);
        bool time_threshold_met = (time_since_last_report.count() / 1000.0f >= minimum_reporting_interval_seconds);
        
        if (percentage_threshold_met || time_threshold_met || completed_pixels == total_pixels) {
            print_progress_update(progress_percentage, elapsed_seconds);
            last_reported_percentage = progress_percent_int;
            last_update_time = current_time;
        }
    }
    
    // Print detailed progress update with educational insights
    void print_progress_update(float progress_percentage, float elapsed_seconds) const {
        std::cout << "\n--- Rendering Progress Update ---" << std::endl;
        std::cout << std::fixed << std::setprecision(1);
        std::cout << "Progress: " << progress_percentage << "% (" 
                  << completed_pixels << "/" << total_pixels << " pixels)" << std::endl;
        
        // Calculate performance metrics
        if (elapsed_seconds > 0 && completed_pixels > 0) {
            float current_pixels_per_second = completed_pixels / elapsed_seconds;
            float estimated_total_seconds = total_pixels / current_pixels_per_second;
            float estimated_remaining_seconds = estimated_total_seconds - elapsed_seconds;
            
            std::cout << "Elapsed time: " << format_time(elapsed_seconds) << std::endl;
            std::cout << "Rendering rate: " << static_cast<int>(current_pixels_per_second) << " pixels/second" << std::endl;
            
            // ETA calculation and display
            if (estimated_remaining_seconds > 0 && progress_percentage < 99.9f) {
                std::cout << "Estimated remaining: " << format_time(estimated_remaining_seconds) << std::endl;
                std::cout << "Estimated total time: " << format_time(estimated_total_seconds) << std::endl;
            } else {
                std::cout << "Estimated remaining: Almost complete!" << std::endl;
            }
            
            // Educational performance insights
            print_educational_insights(current_pixels_per_second, elapsed_seconds);
        }
        
        // Memory pressure monitoring
        if (current_memory_usage > 0) {
            print_memory_pressure_update();
        }
        
        std::cout << "--- End Progress Update ---" << std::endl;
    }
    
    // Educational insights about performance scaling during rendering
    void print_educational_insights(float pixels_per_second, float elapsed_seconds) const {
        std::cout << "\nEducational Performance Insights:" << std::endl;
        
        // Performance classification
        if (pixels_per_second > 10000) {
            std::cout << "  Performance class: Excellent (>10K pixels/sec)" << std::endl;
        } else if (pixels_per_second > 1000) {
            std::cout << "  Performance class: Good (>1K pixels/sec)" << std::endl;
        } else if (pixels_per_second > 100) {
            std::cout << "  Performance class: Moderate (>100 pixels/sec)" << std::endl;
        } else {
            std::cout << "  Performance class: Educational (detailed rendering)" << std::endl;
        }
        
        // Scaling insights
        if (elapsed_seconds > 10.0f) {
            std::cout << "  Scaling insight: Linear relationship between pixels and time" << std::endl;
            std::cout << "  Mathematical note: doubling resolution quadruples rendering time" << std::endl;
        }
        
        // Performance prediction accuracy
        if (completed_pixels > total_pixels * 0.1f) { // After 10% completion
            std::cout << "  Prediction accuracy: ETA estimates become more reliable after 10% completion" << std::endl;
        }
    }
    
    // Memory pressure monitoring during rendering
    void print_memory_pressure_update() const {
        float memory_mb = current_memory_usage / (1024.0f * 1024.0f);
        
        std::cout << "\nMemory Usage Update:" << std::endl;
        std::cout << "  Current memory: " << std::fixed << std::setprecision(1) << memory_mb << " MB" << std::endl;
        
        if (memory_mb > 200.0f) {
            std::cout << "  ⚠️  HIGH MEMORY USAGE WARNING" << std::endl;
            std::cout << "  Educational note: Consider smaller resolutions for learning" << std::endl;
        } else if (memory_mb > 100.0f) {
            std::cout << "  🔶 Moderate memory usage - monitor system performance" << std::endl;
        }
    }
    
    // Print final rendering statistics with comprehensive educational analysis
    void print_final_statistics() const {
        auto end_time = std::chrono::steady_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        float total_seconds = total_duration.count() / 1000.0f;
        
        std::cout << "\n=== Final Rendering Statistics ===" << std::endl;
        std::cout << "Total pixels rendered: " << total_pixels << std::endl;
        std::cout << "Total rendering time: " << format_time(total_seconds) << std::endl;
        
        if (total_seconds > 0) {
            float final_pixels_per_second = total_pixels / total_seconds;
            std::cout << "Average rendering rate: " << static_cast<int>(final_pixels_per_second) << " pixels/second" << std::endl;
            
            // Educational performance summary
            print_performance_scaling_analysis(total_seconds, final_pixels_per_second);
        }
        
        std::cout << "=== End Final Statistics ===" << std::endl;
    }
    
    // Educational analysis of performance scaling characteristics
    void print_performance_scaling_analysis(float total_seconds, float pixels_per_second) const {
        std::cout << "\nEducational Performance Scaling Analysis:" << std::endl;
        
        // Resolution impact analysis
        int resolution_side = static_cast<int>(std::sqrt(total_pixels));
        std::cout << "  Resolution analyzed: ~" << resolution_side << "x" << resolution_side << " pixels" << std::endl;
        
        // Performance scaling predictions
        std::cout << "  Scaling predictions:" << std::endl;
        std::cout << "    2x resolution (4x pixels): ~" << format_time(total_seconds * 4) << " estimated time" << std::endl;
        std::cout << "    0.5x resolution (0.25x pixels): ~" << format_time(total_seconds * 0.25f) << " estimated time" << std::endl;
        
        // Educational recommendations
        std::cout << "  Educational recommendations:" << std::endl;
        if (total_seconds > 60.0f) {
            std::cout << "    - Consider smaller resolutions for interactive learning" << std::endl;
            std::cout << "    - Current resolution good for final high-quality results" << std::endl;
        } else if (total_seconds > 10.0f) {
            std::cout << "    - Good balance between detail and rendering speed" << std::endl;
            std::cout << "    - Suitable for educational experiments" << std::endl;
        } else {
            std::cout << "    - Fast rendering enables rapid iteration and learning" << std::endl;
            std::cout << "    - Try higher resolutions for more detailed results" << std::endl;
        }
    }
    
    // Predict completion time based on current progress (called externally)
    void predict_completion_time() const {
        if (completed_pixels == 0) {
            std::cout << "Completion time prediction: Insufficient data (0% complete)" << std::endl;
            return;
        }
        
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);
        float elapsed_seconds = elapsed_duration.count() / 1000.0f;
        
        float progress_ratio = static_cast<float>(completed_pixels) / total_pixels;
        float estimated_total_seconds = elapsed_seconds / progress_ratio;
        float estimated_remaining_seconds = estimated_total_seconds - elapsed_seconds;
        
        std::cout << "\n=== Completion Time Prediction ===" << std::endl;
        std::cout << "Progress: " << (progress_ratio * 100.0f) << "%" << std::endl;
        std::cout << "Elapsed: " << format_time(elapsed_seconds) << std::endl;
        std::cout << "Estimated remaining: " << format_time(estimated_remaining_seconds) << std::endl;
        std::cout << "Estimated total: " << format_time(estimated_total_seconds) << std::endl;
        std::cout << "=== End Prediction ===" << std::endl;
    }
    
    // Check if rendering should be interrupted (can be extended for user input)
    bool should_interrupt() const {
        // Placeholder for interrupt capability
        // In a real implementation, this would check for user input (e.g., Ctrl+C)
        // or other termination conditions
        return false;
    }
    
    // Get current progress as percentage (0.0 to 100.0)
    float get_progress_percentage() const {
        return (static_cast<float>(completed_pixels) / total_pixels) * 100.0f;
    }
    
    // Get pixels rendered per second
    float get_pixels_per_second() const {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);
        float elapsed_seconds = elapsed_duration.count() / 1000.0f;
        
        if (elapsed_seconds > 0 && completed_pixels > 0) {
            return completed_pixels / elapsed_seconds;
        }
        return 0.0f;
    }

private:
    // Helper function to format time in human-readable format
    std::string format_time(float seconds) const {
        if (seconds < 60.0f) {
            return std::to_string(static_cast<int>(seconds)) + "s";
        } else if (seconds < 3600.0f) {
            int minutes = static_cast<int>(seconds / 60);
            int remaining_seconds = static_cast<int>(seconds) % 60;
            return std::to_string(minutes) + "m " + std::to_string(remaining_seconds) + "s";
        } else {
            int hours = static_cast<int>(seconds / 3600);
            int remaining_minutes = static_cast<int>((seconds - hours * 3600) / 60);
            return std::to_string(hours) + "h " + std::to_string(remaining_minutes) + "m";
        }
    }
};