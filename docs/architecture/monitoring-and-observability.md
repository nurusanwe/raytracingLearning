# Monitoring and Observability

Educational monitoring focuses on learning insights and performance understanding rather than production system monitoring.

## Educational Performance Monitoring

### Learning-Focused Performance Analysis
```cpp
// src/monitoring/educational_monitoring.hpp
namespace EducationalMonitoring {
    
    struct LearningMetrics {
        // Performance learning metrics
        float current_rays_per_second = 0;
        float educational_overhead_percent = 0;
        std::map<std::string, float> epic_performance_progression;
        
        // Educational engagement metrics
        int console_explanations_shown = 0;
        int ui_parameter_changes = 0;
        int brdf_comparisons_performed = 0;
        
        // Mathematical accuracy metrics
        int parameter_validation_warnings = 0;
        int numerical_stability_issues = 0;
        int energy_conservation_violations = 0;
    };
    
    class EducationalMonitor {
    private:
        LearningMetrics current_session;
        std::chrono::steady_clock::time_point session_start;
        
    public:
        void begin_learning_session(const std::string& epic_name) {
            session_start = std::chrono::steady_clock::now();
            current_session = LearningMetrics{};
            
            if (EducationalMode::show_calculations()) {
                printf("=== Learning Session Started: %s ===\n", epic_name.c_str());
            }
        }
        
        void record_performance_measurement(const std::string& operation, float time_ms) {
            current_session.epic_performance_progression[operation] = time_ms;
            
            if (EducationalMode::show_calculations()) {
                printf("Performance: %s took %.3f ms\n", operation.c_str(), time_ms);
            }
        }
        
        void generate_learning_report() const {
            auto session_duration = std::chrono::steady_clock::now() - session_start;
            float session_minutes = std::chrono::duration<float>(session_duration).count() / 60.0f;
            
            printf("=== Learning Session Report ===\n");
            printf("Session Duration: %.1f minutes\n", session_minutes);
            printf("Educational Engagement:\n");
            printf("  Console explanations viewed: %d\n", current_session.console_explanations_shown);
            printf("  Parameter adjustments made: %d\n", current_session.ui_parameter_changes);
            printf("  BRDF comparisons performed: %d\n", current_session.brdf_comparisons_performed);
            
            printf("\nPerformance Learning:\n");
            for (const auto& [operation, time] : current_session.epic_performance_progression) {
                printf("  %s: %.3f ms\n", operation.c_str(), time);
            }
            
            printf("\nMathematical Accuracy:\n");
            printf("  Parameter warnings: %d\n", current_session.parameter_validation_warnings);
            printf("  Numerical issues: %d\n", current_session.numerical_stability_issues);
            
            // Educational insights based on metrics
            if (current_session.console_explanations_shown < 5) {
                printf("\nLearning Suggestion: Try enabling more mathematical explanations\n");
                printf("to better understand the ray tracing calculations.\n");
            }
            
            if (current_session.ui_parameter_changes > 20) {
                printf("\nLearning Insight: You're actively exploring parameter relationships!\n");
                printf("This hands-on experimentation builds intuition for BRDF behavior.\n");
            }
            
            printf("===============================\n");
        }
    };
}
```
