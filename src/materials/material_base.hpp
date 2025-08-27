#pragma once
#include "../core/vector3.hpp"
#include <iostream>
#include <algorithm>

// Material Type enumeration for polymorphic material system
// Supports current Lambert and Cook-Torrance materials with extensibility for future OpenPBR
enum class MaterialType {
    Lambert,        // Perfectly diffuse material following Lambert's cosine law
    CookTorrance,   // Microfacet material using Cook-Torrance BRDF with D, G, F terms
    OpenPBR         // Future: Complete OpenPBR material model (Epic 4)
};

// Abstract Material base class for polymorphic material evaluation
// Provides common interface for all material types while maintaining educational transparency
// Design principles:
// - Virtual BRDF evaluation allows different material mathematical models
// - Educational debugging methods preserve learning-focused console output
// - Parameter validation ensures physically plausible material properties
// - Clean polymorphic interface supports future material system extensions
class Material {
public:
    Vector3 base_color;     // Surface albedo/reflectance color (RGB channels)
    MaterialType type;      // Material type identifier for polymorphic behavior
    
    // Constructor with base color and material type
    // Parameters:
    //   color: surface base color/albedo (default: neutral gray)
    //   mat_type: material type for polymorphic identification
    Material(const Vector3& color = Vector3(0.7f, 0.7f, 0.7f), MaterialType mat_type = MaterialType::Lambert) 
        : base_color(color), type(mat_type) {}
    
    // Virtual destructor ensures proper polymorphic cleanup
    virtual ~Material() = default;
    
    // Pure virtual BRDF evaluation - implemented by concrete material classes
    // Each material type provides its own mathematical model:
    // - Lambert: f_r = ρ/π (constant diffuse reflection)
    // - Cook-Torrance: f_r = (D×G×F)/(4×cos(θl)×cos(θv)) (microfacet theory)
    // - Future OpenPBR: Advanced layered material model
    // Parameters:
    //   wi: incident light direction (pointing toward surface, normalized)
    //   wo: outgoing view direction (pointing toward camera, normalized)
    //   normal: surface normal at intersection point (outward-pointing, normalized)
    //   verbose: enable educational mathematical breakdown output
    virtual Vector3 evaluate_brdf(const Vector3& wi, const Vector3& wo, const Vector3& normal, bool verbose = true) const = 0;
    
    // Pure virtual parameter validation - implemented by concrete material classes
    // Each material type defines its own physically valid parameter ranges:
    // - Lambert: albedo ∈ [0,1] for energy conservation
    // - Cook-Torrance: roughness ∈ [0.01,1], metallic ∈ [0,1], specular ∈ [0,1]
    // - Future materials: specific parameter constraints per material model
    virtual bool validate_parameters() const = 0;
    
    // Pure virtual parameter clamping - implemented by concrete material classes
    // Automatically adjusts material parameters to physically valid ranges
    // Prevents numerical instability and non-physical behavior
    // Educational output shows any clamping performed for learning purposes
    virtual void clamp_to_valid_ranges() = 0;
    
    // Virtual educational debugging method - optionally overridden by concrete materials
    // Provides comprehensive mathematical breakdown of BRDF evaluation process
    // Default implementation provides basic information, specific materials add detailed explanations
    // Parameters: same as evaluate_brdf for direct educational comparison
    virtual void explain_brdf_evaluation(const Vector3& wi, const Vector3& wo, const Vector3& normal) const {
        std::cout << "\n=== Generic Material BRDF Evaluation ===" << std::endl;
        std::cout << "Material Type: " << material_type_name() << std::endl;
        std::cout << "Base Color: (" << base_color.x << ", " << base_color.y << ", " << base_color.z << ")" << std::endl;
        std::cout << "Incident direction (wi): (" << wi.x << ", " << wi.y << ", " << wi.z << ")" << std::endl;
        std::cout << "Outgoing direction (wo): (" << wo.x << ", " << wo.y << ", " << wo.z << ")" << std::endl;
        std::cout << "Surface normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
        std::cout << "Note: Override explain_brdf_evaluation() in concrete material for detailed mathematical breakdown" << std::endl;
        std::cout << "=== Generic evaluation explanation complete ===" << std::endl;
    }
    
    // Virtual light scattering calculation with full rendering equation
    // Default implementation uses BRDF evaluation with cosine term and incident radiance
    // Can be overridden by specific materials for optimized or specialized calculations
    // Parameters:
    // light_direction: direction from surface to light source (normalized)
    // view_direction: direction from surface to camera (normalized)
    // normal: surface normal at intersection point (outward-pointing, normalized)
    // incident_radiance: incoming light energy (L_i in rendering equation)
    // verbose: enable educational console output
    virtual Vector3 scatter_light(const Vector3& light_direction, const Vector3& view_direction, 
                                 const Vector3& normal, const Vector3& incident_radiance, bool verbose = true) const {
        if (verbose) {
            std::cout << "\n=== Generic Material Light Scattering ===" << std::endl;
            std::cout << "Material Type: " << material_type_name() << std::endl;
        }
        
        // Calculate cosine term: n·l (normal dot light_direction)
        float cos_theta = normal.dot(light_direction);
        if (verbose) {
            std::cout << "Raw cosine term n·l = " << cos_theta << std::endl;
        }
        
        // Clamp to positive values: surfaces don't emit light when facing away
        cos_theta = std::max(0.0f, cos_theta);
        if (verbose) {
            std::cout << "Clamped cosine term max(0, n·l) = " << cos_theta << std::endl;
        }
        
        // Evaluate material-specific BRDF
        Vector3 brdf = evaluate_brdf(light_direction, view_direction, normal, verbose);
        
        // Full rendering equation evaluation: L_o = f_r * L_i * cos(θ)
        Vector3 outgoing_radiance = Vector3(
            brdf.x * incident_radiance.x * cos_theta,  // Red channel
            brdf.y * incident_radiance.y * cos_theta,  // Green channel  
            brdf.z * incident_radiance.z * cos_theta   // Blue channel
        );
        
        if (verbose) {
            std::cout << "Final outgoing radiance: L_o = f_r * L_i * cos(θ) = (" 
                      << outgoing_radiance.x << ", " << outgoing_radiance.y << ", " << outgoing_radiance.z << ")" << std::endl;
            std::cout << "=== Generic light scattering calculation complete ===" << std::endl;
        }
        
        return outgoing_radiance;
    }
    
    // Helper method to get human-readable material type name for educational output
    // Provides consistent naming across different material implementations
    const char* material_type_name() const {
        switch (type) {
            case MaterialType::Lambert: return "Lambert (Diffuse)";
            case MaterialType::CookTorrance: return "Cook-Torrance (Microfacet)";
            case MaterialType::OpenPBR: return "OpenPBR (Advanced PBR)";
            default: return "Unknown Material Type";
        }
    }
};