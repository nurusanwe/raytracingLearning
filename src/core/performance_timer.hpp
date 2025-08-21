#pragma once
#include <chrono>
#include <iostream>
#include <map>
#include <string>

// PerformanceTimer class for comprehensive phase-specific timing analysis
// Designed for educational ray tracing performance monitoring with detailed breakdown
//
// Features:
// - Separate timing for ray generation, intersection testing, and shading calculations
// - High-precision timing using std::chrono::high_resolution_clock
// - Performance statistics with rays per second calculations
// - Educational console output with performance insights
// - Memory usage tracking integration
//
// Usage:
// 1. Call start_phase() before beginning a rendering phase
// 2. Call end_phase() after completing the phase
// 3. Use increment_counter() to track operation counts
// 4. Call print_performance_breakdown() for educational analysis
//
// Mathematical foundation:
// - Performance measured in milliseconds with microsecond precision
// - Rays per second = total_rays / (total_time_seconds)
// - Phase percentages = phase_time / total_time * 100
// - Statistical validation requires minimum 1000-ray batches for accuracy
class PerformanceTimer {
public:
    enum Phase {
        RAY_GENERATION,
        INTERSECTION_TESTING,
        SHADING_CALCULATION,
        IMAGE_OUTPUT,
        TOTAL_RENDER
    };
    
private:
    std::map<Phase, std::chrono::steady_clock::time_point> phase_start_times;
    std::map<Phase, float> phase_durations;  // In milliseconds
    std::map<Phase, int> phase_counters;
    std::map<Phase, std::string> phase_names;
    
    // Memory usage tracking
    size_t memory_usage_bytes = 0;
    
    // Performance validation
    bool timing_active = false;
    std::chrono::steady_clock::time_point session_start_time;
    
public:
    PerformanceTimer() {
        // Initialize phase names for educational output
        phase_names[RAY_GENERATION] = "Ray Generation";
        phase_names[INTERSECTION_TESTING] = "Intersection Testing";
        phase_names[SHADING_CALCULATION] = "Shading Calculation";
        phase_names[IMAGE_OUTPUT] = "Image Output";
        phase_names[TOTAL_RENDER] = "Total Render";
        
        // Initialize all durations and counters to zero
        for (auto& pair : phase_names) {
            phase_durations[pair.first] = 0.0f;
            phase_counters[pair.first] = 0;
        }
        
        session_start_time = std::chrono::steady_clock::now();
    }
    
    void start_phase(Phase phase) {
        phase_start_times[phase] = std::chrono::steady_clock::now();
        timing_active = true;
    }
    
    void end_phase(Phase phase) {
        if (phase_start_times.find(phase) == phase_start_times.end()) {
            std::cout << "WARNING: end_phase() called without matching start_phase() for " 
                     << phase_names[phase] << std::endl;
            return;
        }
        
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - phase_start_times[phase]);
        
        // Convert to milliseconds with microsecond precision
        phase_durations[phase] += duration.count() / 1000.0f;
        
        // Clear the start time to prevent double-ending
        phase_start_times.erase(phase);
    }
    
    void increment_counter(Phase phase, int count = 1) {
        phase_counters[phase] += count;
    }
    
    // Educational reporting methods
    void print_performance_breakdown() const {
        std::cout << "\n=== Educational Performance Analysis ===" << std::endl;
        
        float total_time = phase_durations.at(TOTAL_RENDER);
        if (total_time <= 0.0f) {
            // Calculate total from individual phases if TOTAL_RENDER wasn't measured
            total_time = phase_durations.at(RAY_GENERATION) + 
                        phase_durations.at(INTERSECTION_TESTING) + 
                        phase_durations.at(SHADING_CALCULATION) + 
                        phase_durations.at(IMAGE_OUTPUT);
        }
        
        std::cout << "Performance Timing Breakdown:" << std::endl;
        std::cout << "  Total rendering time: " << total_time << " ms" << std::endl;
        
        for (const auto& pair : phase_names) {
            Phase phase = pair.first;
            if (phase == TOTAL_RENDER) continue;  // Skip total for individual breakdown
            
            float duration = phase_durations.at(phase);
            float percentage = (total_time > 0) ? (duration / total_time * 100.0f) : 0.0f;
            int count = phase_counters.at(phase);
            
            std::cout << "  " << pair.second << ": " << duration << " ms (" 
                     << percentage << "%) - " << count << " operations" << std::endl;
            
            // Phase-specific educational insights
            if (phase == RAY_GENERATION && count > 0) {
                float rays_per_ms = count / std::max(0.001f, duration);
                std::cout << "    Ray generation rate: " << (rays_per_ms * 1000.0f) << " rays/second" << std::endl;
            } else if (phase == INTERSECTION_TESTING && count > 0) {
                float tests_per_ms = count / std::max(0.001f, duration);
                std::cout << "    Intersection test rate: " << (tests_per_ms * 1000.0f) << " tests/second" << std::endl;
            } else if (phase == SHADING_CALCULATION && count > 0) {
                float shading_per_ms = count / std::max(0.001f, duration);
                std::cout << "    Shading calculation rate: " << (shading_per_ms * 1000.0f) << " calculations/second" << std::endl;
            }
        }
        
        std::cout << "=== End Performance Analysis ===" << std::endl;
    }
    
    void print_rays_per_second_statistics() const {
        std::cout << "\n=== Rays Per Second Statistics ===" << std::endl;
        
        int total_rays = phase_counters.at(RAY_GENERATION);
        float total_time_seconds = phase_durations.at(TOTAL_RENDER) / 1000.0f;
        
        if (total_time_seconds <= 0.0f) {
            // Calculate from individual phases
            total_time_seconds = (phase_durations.at(RAY_GENERATION) + 
                                phase_durations.at(INTERSECTION_TESTING) + 
                                phase_durations.at(SHADING_CALCULATION) + 
                                phase_durations.at(IMAGE_OUTPUT)) / 1000.0f;
        }
        
        if (total_rays > 0 && total_time_seconds > 0) {
            float rays_per_second = total_rays / total_time_seconds;
            std::cout << "Total rays generated: " << total_rays << std::endl;
            std::cout << "Total rendering time: " << total_time_seconds << " seconds" << std::endl;
            std::cout << "Rays per second: " << rays_per_second << std::endl;
            
            // Performance classification for educational purposes
            if (rays_per_second > 1000000) {
                std::cout << "Performance class: Excellent (>1M rays/sec)" << std::endl;
            } else if (rays_per_second > 100000) {
                std::cout << "Performance class: Good (>100K rays/sec)" << std::endl;
            } else if (rays_per_second > 10000) {
                std::cout << "Performance class: Moderate (>10K rays/sec)" << std::endl;
            } else {
                std::cout << "Performance class: Educational (suitable for learning)" << std::endl;
            }
            
            // Educational insights
            if (total_rays >= 1000) {
                std::cout << "Statistical validity: GOOD (≥1000 rays for reliable measurement)" << std::endl;
            } else {
                std::cout << "Statistical validity: LIMITED (<1000 rays - results may vary)" << std::endl;
            }
        } else {
            std::cout << "Insufficient data for rays per second calculation" << std::endl;
        }
        
        std::cout << "=== End Rays Per Second Statistics ===" << std::endl;
    }
    
    void print_phase_analysis() const {
        std::cout << "\n=== Educational Phase Analysis ===" << std::endl;
        
        float total_time = phase_durations.at(RAY_GENERATION) + 
                          phase_durations.at(INTERSECTION_TESTING) + 
                          phase_durations.at(SHADING_CALCULATION) + 
                          phase_durations.at(IMAGE_OUTPUT);
        
        if (total_time <= 0) {
            std::cout << "No timing data available for phase analysis" << std::endl;
            return;
        }
        
        // Analyze phase distribution
        std::cout << "Phase Distribution Analysis:" << std::endl;
        
        float ray_gen_percent = (phase_durations.at(RAY_GENERATION) / total_time) * 100.0f;
        float intersection_percent = (phase_durations.at(INTERSECTION_TESTING) / total_time) * 100.0f;
        float shading_percent = (phase_durations.at(SHADING_CALCULATION) / total_time) * 100.0f;
        float output_percent = (phase_durations.at(IMAGE_OUTPUT) / total_time) * 100.0f;
        
        std::cout << "  Ray Generation: " << ray_gen_percent << "%" << std::endl;
        std::cout << "  Intersection Testing: " << intersection_percent << "%" << std::endl;
        std::cout << "  Shading Calculation: " << shading_percent << "%" << std::endl;
        std::cout << "  Image Output: " << output_percent << "%" << std::endl;
        
        // Educational performance insights
        std::cout << "\nPerformance Insights:" << std::endl;
        
        if (intersection_percent > 50.0f) {
            std::cout << "  - Intersection testing is the bottleneck (>50% of time)" << std::endl;
            std::cout << "  - Consider optimizing scene data structures (BVH, spatial partitioning)" << std::endl;
        }
        
        if (shading_percent > 40.0f) {
            std::cout << "  - Shading calculations dominate (>40% of time)" << std::endl;
            std::cout << "  - Complex materials or lighting may be performance-intensive" << std::endl;
        }
        
        if (ray_gen_percent > 30.0f) {
            std::cout << "  - Ray generation overhead is significant (>30% of time)" << std::endl;
            std::cout << "  - Camera ray calculations may benefit from optimization" << std::endl;
        }
        
        if (output_percent > 20.0f) {
            std::cout << "  - Image output overhead is high (>20% of time)" << std::endl;
            std::cout << "  - File I/O or color conversion may be slow" << std::endl;
        }
        
        // Balanced performance indication
        if (ray_gen_percent < 25.0f && intersection_percent < 40.0f && 
            shading_percent < 40.0f && output_percent < 15.0f) {
            std::cout << "  - Performance distribution is well-balanced across phases" << std::endl;
        }
        
        std::cout << "=== End Phase Analysis ===" << std::endl;
    }
    
    // Memory monitoring integration
    void record_memory_usage(size_t bytes) {
        memory_usage_bytes = bytes;
    }
    
    void print_memory_performance_correlation() const {
        if (memory_usage_bytes == 0) {
            std::cout << "No memory usage data recorded" << std::endl;
            return;
        }
        
        std::cout << "\n=== Memory-Performance Correlation ===" << std::endl;
        
        float memory_mb = memory_usage_bytes / (1024.0f * 1024.0f);
        int total_rays = phase_counters.at(RAY_GENERATION);
        float total_time_seconds = phase_durations.at(TOTAL_RENDER) / 1000.0f;
        
        if (total_time_seconds <= 0.0f) {
            total_time_seconds = (phase_durations.at(RAY_GENERATION) + 
                                phase_durations.at(INTERSECTION_TESTING) + 
                                phase_durations.at(SHADING_CALCULATION) + 
                                phase_durations.at(IMAGE_OUTPUT)) / 1000.0f;
        }
        
        std::cout << "Memory usage: " << memory_mb << " MB" << std::endl;
        
        if (total_rays > 0 && total_time_seconds > 0) {
            float rays_per_second = total_rays / total_time_seconds;
            float rays_per_mb = total_rays / std::max(0.1f, memory_mb);
            
            std::cout << "Memory efficiency: " << rays_per_mb << " rays per MB" << std::endl;
            std::cout << "Performance density: " << (rays_per_second / memory_mb) << " (rays/sec)/MB" << std::endl;
            
            // Educational memory insights
            if (memory_mb > 100.0f) {
                std::cout << "Educational note: Large memory usage may impact performance" << std::endl;
                std::cout << "Consider smaller resolutions for educational experiments" << std::endl;
            }
            
            if (rays_per_mb < 10000.0f) {
                std::cout << "Educational note: Memory efficiency could be improved" << std::endl;
                std::cout << "Each MB of memory processes " << rays_per_mb << " rays" << std::endl;
            }
        }
        
        std::cout << "=== End Memory-Performance Correlation ===" << std::endl;
    }
    
    // Validation and utility methods
    bool validate_timing_accuracy() const {
        // Check if timing measurements meet educational accuracy requirements
        int total_rays = phase_counters.at(RAY_GENERATION);
        float total_time = phase_durations.at(TOTAL_RENDER);
        
        if (total_time <= 0.0f) {
            total_time = phase_durations.at(RAY_GENERATION) + 
                        phase_durations.at(INTERSECTION_TESTING) + 
                        phase_durations.at(SHADING_CALCULATION) + 
                        phase_durations.at(IMAGE_OUTPUT);
        }
        
        // Requirement: minimum 1000-ray batches for statistical validity
        bool sufficient_rays = total_rays >= 1000;
        
        // Requirement: measurement accuracy within 1 millisecond
        bool sufficient_time = total_time >= 1.0f;
        
        if (!sufficient_rays) {
            std::cout << "Timing validation: INSUFFICIENT RAYS (" << total_rays << " < 1000)" << std::endl;
        }
        
        if (!sufficient_time) {
            std::cout << "Timing validation: INSUFFICIENT TIME (" << total_time << " ms < 1.0 ms)" << std::endl;
        }
        
        return sufficient_rays && sufficient_time;
    }
    
    void reset_statistics() {
        for (auto& pair : phase_durations) {
            pair.second = 0.0f;
        }
        for (auto& pair : phase_counters) {
            pair.second = 0;
        }
        phase_start_times.clear();
        memory_usage_bytes = 0;
        session_start_time = std::chrono::steady_clock::now();
    }
};