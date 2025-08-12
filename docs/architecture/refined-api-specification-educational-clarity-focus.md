# Refined API Specification - Educational Clarity Focus

**Core Principle:** Simple, clear progression from Epic 1 functions to Epic 6 optimized classes with consistent educational debugging and single-source-of-truth control.

## Unified Educational Control System

### Single Educational Mode Controller
```cpp
namespace EducationalMode {
    enum Level {
        Production,    // No educational overhead - Epic 6 target
        Learning,      // Essential explanations - Epic 2-4 balance  
        Debugging      // Full mathematical transparency - Epic 1 start
    };
    
    void set_level(Level level);
    Level get_level();
    
    // All educational features controlled through this single system
    bool is_explanation_enabled();      // Controls all explanation methods
    bool is_console_output_enabled();   // Controls all printf/console output
    bool is_validation_enabled();       // Controls all mathematical validation
    
    // Simple memory management for educational features
    void set_max_memory_mb(int max_mb); // Limits educational data storage
    void cleanup_educational_data();    // Clears old calculation logs
}
```

## Epic 1: Foundation API - Maximum Simplicity

**Purpose:** Single-file functions with complete mathematical transparency

### Core Educational Functions
```cpp
namespace Epic1 {
    
    // Students start with this single function call
    Vector3 trace_single_ray() {
        // Hard-coded educational example with full mathematical breakdown
        // Students see every calculation step in the actual implementation
        
        if (EducationalMode::is_console_output_enabled()) {
            printf("=== Epic 1: Single Ray Tracing Calculation ===\n");
        }
        
        // All mathematics visible in the implementation, not separate explanation functions
        Ray camera_ray(Vector3(0, 0, 0), Vector3(0, 0, -1));
        Vector3 sphere_center(0, 0, -5);
        float sphere_radius = 1.0f;
        
        // Ray-sphere intersection with educational comments IN the code
        Vector3 oc = camera_ray.origin - sphere_center;
        float b = oc.dot(camera_ray.direction);
        float c = oc.dot(oc) - sphere_radius * sphere_radius;
        float discriminant = b * b - c;
        
        if (EducationalMode::is_console_output_enabled()) {
            printf("Ray-sphere intersection: b=%g, c=%g, discriminant=%g\n", b, c, discriminant);
        }
        
        if (discriminant < 0) return Vector3(0.2f, 0.2f, 0.8f);
        
        float t = -b - sqrt(discriminant);
        Vector3 hit_point = camera_ray.point_at(t);
        Vector3 normal = (hit_point - sphere_center).normalized();
        
        // Lambert BRDF calculation
        Vector3 light_pos(2, 2, 0);
        Vector3 light_dir = (light_pos - hit_point).normalized();
        float cos_theta = std::max(0.0f, normal.dot(light_dir));
        
        Vector3 material_color(0.7f, 0.3f, 0.3f);
        Vector3 final_color = material_color * cos_theta;
        
        if (EducationalMode::is_console_output_enabled()) {
            printf("Lambert shading: cos(θ)=%g, final_color=(%g,%g,%g)\n", 
                   cos_theta, final_color.x, final_color.y, final_color.z);
        }
        
        return final_color;
    }
}
```
