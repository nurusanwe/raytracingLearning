# Epic 3: Cook-Torrance Microfacet Theory Implementation

**Expanded Goal:** Implement Cook-Torrance microfacet BRDF theory to provide physically accurate specular reflection and understand the mathematical foundation of modern material models. This epic transitions from simple Lambert diffuse to rigorous microfacet theory while maintaining educational transparency, delivering the core physics understanding necessary for OpenPBR implementation.

## Story 3.1 Pure Cook-Torrance BRDF Implementation
As a **graphics programming learner**,  
I want **complete Cook-Torrance microfacet BRDF with proper normal distribution, geometry, and Fresnel terms**,  
so that **I can understand the mathematical foundation of physically based rendering**.

### Acceptance Criteria
1. Normal Distribution Function (D) implements GGX/Trowbridge-Reitz distribution with roughness parameter and educational mathematical breakdown
2. Geometry Function (G) implements Smith masking-shadowing model with proper self-shadowing calculations
3. Fresnel Function (F) provides accurate dielectric and conductor reflection with complex refractive index support
4. Complete Cook-Torrance evaluation combines D, G, F terms with proper normalization and energy conservation
5. Console output shows detailed mathematical breakdown of each BRDF component for learning validation

## Story 3.2 Advanced Lighting and Multiple Lights
As a **graphics programming learner**,  
I want **multiple light types including directional, point, and area lights**,  
so that **I can understand different lighting models and their impact on microfacet material appearance**.

### Acceptance Criteria
1. Light base class supports point, directional, and simple area light implementations with proper falloff
2. Multiple light sampling correctly accumulates Cook-Torrance contributions from all light sources in scene
3. Shadow ray testing prevents light contribution from occluded sources with educational ray counting
4. Light importance sampling demonstrates basic Monte Carlo integration techniques for specular highlights
5. Lighting debug visualization shows individual light contributions and specular highlight behavior

## Story 3.3 Interactive Material Parameter Controls
As a **graphics programming learner**,  
I want **real-time Cook-Torrance parameter adjustment through command-line interface**,  
so that **I can experiment with microfacet properties and observe immediate visual feedback**.

### Acceptance Criteria
1. Command-line interface supports real-time adjustment of roughness, metallic, and base color parameters
2. Material parameter changes trigger immediate re-render with Cook-Torrance evaluation timing feedback
3. Parameter validation ensures physically plausible ranges with educational explanations of microfacet theory
4. Preset material library includes common Cook-Torrance configurations (metal, dielectric, rough, smooth)
5. Material state serialization allows saving and loading of microfacet configurations for experimentation

## Story 3.4 Fresnel Equations and Layered Materials Foundation
As a **graphics programming learner**,  
I want **complete Fresnel equation implementation and basic material layering**,  
so that **I can understand physically accurate reflection and prepare for OpenPBR complexity**.

### Acceptance Criteria
1. Fresnel equations support both dielectric (glass, plastic) and conductor (metal) materials with proper complex IOR
2. Basic material layering demonstrates diffuse base with specular coating using proper energy conservation
3. Material examples demonstrate glass, metal, plastic, and ceramic rendering with accurate Fresnel behavior
4. Fresnel angle dependency visualization shows how reflection changes with viewing angle
5. Material validation suite ensures mathematical correctness against known Cook-Torrance reference implementations
