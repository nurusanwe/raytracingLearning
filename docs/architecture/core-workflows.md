# Core Workflows

The core workflows illustrate key system interactions across the epic progression, showing how educational transparency integrates with mathematical calculations and user interactions.

## Epic 1: First Ray Tracing Experience Workflow

```mermaid
sequenceDiagram
    participant Student
    participant Epic1
    participant EducationalMode
    participant Console
    
    Student->>Epic1: Call trace_single_ray()
    Epic1->>EducationalMode: Check is_console_output_enabled()
    EducationalMode-->>Epic1: true (Debugging level)
    
    Epic1->>Console: printf("=== Epic 1: Single Ray Tracing ===")
    Epic1->>Epic1: Calculate ray-sphere intersection
    Note over Epic1: Vector3 oc = ray.origin - sphere.center<br/>float discriminant = b*b - c
    
    Epic1->>EducationalMode: Check is_console_output_enabled()
    EducationalMode-->>Epic1: true
    Epic1->>Console: printf("discriminant=%g", discriminant)
    
    Epic1->>Epic1: Calculate Lambert BRDF
    Note over Epic1: cos_theta = normal.dot(light_dir)<br/>final_color = material * cos_theta
    
    Epic1->>Console: printf("Lambert result: (%g,%g,%g)")
    Epic1-->>Student: Return final_color Vector3
    
    Student->>Console: Observe mathematical breakdown
    Note over Student: Sees every calculation step<br/>Understands ray-sphere math<br/>Connects math to visual result
```

## Epic 2: Real-time Material Parameter Manipulation Workflow

```mermaid
sequenceDiagram
    participant Student
    participant ImGuiUI
    participant Material
    participant EducationalMode
    participant RayTracer
    participant Display
    
    Student->>ImGuiUI: Adjust roughness slider
    ImGuiUI->>Material: Set roughness = 0.8f
    
    Material->>EducationalMode: Check is_explanation_enabled()
    EducationalMode-->>Material: true (Learning level)
    Material->>Console: printf("Roughness=0.8: Surface becomes rough")
    
    ImGuiUI->>RayTracer: Trigger re-render
    RayTracer->>Scene: intersect(ray)
    Scene-->>RayTracer: Intersection with Material*
    
    RayTracer->>Material: evaluate_brdf(wi, wo, normal)
    Material->>Material: Calculate Cook-Torrance BRDF
    Note over Material: D = alpha²/(π * denom²)<br/>G = masking/shadowing<br/>F = Fresnel reflection
    
    Material->>EducationalMode: Check is_console_output_enabled()
    EducationalMode-->>Material: true
    Material->>Console: printf("Cook-Torrance: D=%g, result=(%g,%g,%g)")
    
    Material-->>RayTracer: Return BRDF result
    RayTracer->>Display: Update pixel colors
    Display-->>Student: Visual feedback (rougher appearance)
    
    Student->>Console: Observe mathematical correlation
    Note over Student: Sees roughness parameter<br/>affect D term calculation<br/>and final visual result
```
