# 3D Pathtracer Learning Project Product Requirements Document (PRD)

## Goals and Background Context

### Goals

- Build deep understanding of rendering engine architecture through hands-on implementation from pathtracing fundamentals to production-quality hybrid renderer
- Master integration of industry-standard tools (MaterialX, USD, OpenPBR) in practical graphics programming context
- Develop Apple Silicon optimization expertise while maintaining extensible cross-platform architecture foundation
- Create modular C++ rendering engine that serves as comprehensive learning laboratory for graphics programming concepts
- Establish production-quality code patterns and architectural thinking through incremental complexity management

### Background Context

This project addresses the critical gap between theoretical understanding of rendering concepts and practical implementation skills. Most graphics learning resources force a choice between oversimplified tutorials that can't scale or diving into complex existing codebases that obscure the learning process. This pathtracer provides a structured learn-by-building approach, starting with ultra-minimal single-ray calculations using real OpenPBR mathematics and progressively scaling to full-featured rendering with MaterialX integration and eventual hybrid pathtracing/rasterization capabilities.

The learning approach prioritizes architectural understanding from day one, ensuring that each feature addition maintains production-quality patterns while serving clear educational purposes. By building on Apple Silicon with explicit optimization learning, the project develops platform-specific expertise while maintaining modular design for future cross-platform expansion.

### Change Log

| Date | Version | Description | Author |
|------|---------|-------------|--------|
| 2025-08-10 | 1.0 | Initial PRD creation from Project Brief | PM Agent |

## Requirements

### Functional

1. FR1: System shall calculate ray-sphere intersection using proper quadratic equation solution with discriminant checking and root validation
2. FR2: System shall implement physically accurate Lambert BRDF using real OpenPBR energy conservation mathematics (n·l formulation)
3. FR3: System shall perform complete light transport calculation from single point light source through surface interaction to camera
4. FR4: System shall generate multi-resolution image output (512x512 to 4K+) with proper pixel sampling and anti-aliasing
5. FR5: System shall support multiple primitive types (spheres, planes, triangular meshes) with spatial organization
6. FR6: System shall integrate MaterialX node graph evaluation with real-time parameter adjustment capabilities
7. FR7: System shall load USD scene files with geometry, materials, lighting, and animation data parsing
8. FR8: System shall implement complete OpenPBR material model including metallic, dielectric, coating, emission, and subsurface scattering layers
9. FR9: System shall provide interactive camera controls with rotate, translate, zoom, field-of-view, and depth-of-field effects
10. FR10: System shall support multiple light types (point, directional, area lights) with environment map integration

### Non Functional

1. NFR1: Apple Silicon optimization using NEON vectorization and Metal Performance Shaders integration while maintaining cross-platform extensibility
2. NFR2: System architecture shall support both pathtracing and future rasterization backends through modular plugin system
3. NFR3: All mathematical calculations shall maintain physically accurate precision suitable for production-quality rendering
4. NFR4: Performance monitoring shall provide frame timing, ray count statistics, and bottleneck identification tools
5. NFR5: Code quality shall serve as exemplar for educational graphics programming with comprehensive inline documentation
6. NFR6: Memory management shall follow cache-friendly patterns optimized for Apple Silicon memory hierarchy
7. NFR7: Build system shall use CMake for industry-standard cross-platform development workflows

## User Interface Design Goals

### Overall UX Vision
Graphics programming education prioritizes mathematical transparency over polished user experience. The interface should expose rendering pipeline calculations and provide immediate visual feedback for parameter changes. Think "developer tools aesthetic" with comprehensive debugging visualization rather than consumer application polish.

### Key Interaction Paradigms
- **Mathematical Transparency:** Every UI element should reveal the underlying calculations and allow inspection of intermediate values
- **Real-time Parameter Manipulation:** Sliders and controls that provide immediate visual feedback for BRDF parameters, lighting conditions, and camera settings
- **Educational Debugging:** Visual tools that illuminate ray behavior, energy calculations, and algorithmic decisions for learning reinforcement
- **Progressive Complexity:** Interface complexity grows with implemented features, starting with console output and expanding to graphical controls

### Core Screens and Views
- **Console Output Display:** Primary interface showing calculated color values and mathematical intermediate results
- **Parameter Control Panel:** Real-time adjustment of material properties, lighting parameters, and rendering settings
- **Ray Visualization Window:** Debug view showing ray paths, intersection points, and energy calculations
- **Performance Monitoring Dashboard:** Display of frame timing, ray count statistics, and optimization metrics
- **Scene Configuration Interface:** Controls for camera positioning, object placement, and lighting setup

### Accessibility: None
Educational development tool focused on graphics programming learning rather than general user accessibility

### Branding
Developer-focused aesthetic emphasizing function over form. Clean typography for mathematical formulas, high-contrast colors for debugging visualization, and professional developer tool appearance. Think Xcode/Visual Studio aesthetic rather than consumer graphics application.

### Target Device and Platforms: Web Responsive
Interface should work across development environments while optimizing for Apple Silicon development workflow

## Technical Assumptions

### Repository Structure: Monorepo
Single repository containing all development phases from MVP through production features to maintain learning continuity and enable easy comparison between implementation milestones.

### Service Architecture
**Modular Monolith evolving to Plugin-based Architecture** - Begin with single-file implementation for mathematical transparency, then gradually extract modules based on actual usage patterns rather than theoretical abstractions. Final architecture supports hot-swappable rendering backends (pathtracing, future rasterization) and material systems.

### Testing Requirements
**Full Testing Pyramid with Educational Focus** - Custom assertion framework for mathematical validation expanding to comprehensive unit testing suite. Include visual regression testing for rendering accuracy and performance benchmarking tools specific to graphics workloads.

### Additional Technical Assumptions and Requests

**Development Platform Decisions:**
- **Primary Platform:** macOS with Apple Silicon (M-series) for development and optimization learning
- **Language:** Modern C++ (C++20/C++23) for performance, industry relevance, and learning transfer value
- **Build System:** CMake for cross-platform compatibility and industry-standard workflows
- **Development Environment:** Xcode with Metal framework access for GPU acceleration learning

**Industry Integration Requirements:**
- **Material Standards Progression:** Physics-First educational journey through material theory:
  - Phase 1: Hand-coded Lambert BRDF (MVP learning foundation)
  - Phase 2: Cook-Torrance microfacet theory implementation (core physics understanding)
  - Phase 3: OpenPBR material model implementation (industry-leading accuracy)
  - Phase 4: MaterialX node graph evaluation framework (Academy Software Foundation standard)
- **USD:** Pixar Universal Scene Description SDK for scene loading with geometry, materials, and animation data
- **Image I/O:** OpenEXR for HDR image handling with PNG/JPEG support for standard workflows
- **Math Library:** Custom linear algebra implementation initially (learning focus), GLM for validation

**Performance and Optimization Assumptions:**
- **SIMD Support:** NEON vectorization for Apple Silicon with abstraction layer for future AVX/SSE support
- **Memory Management:** Manual memory management with smart pointers, emphasizing cache-friendly patterns for Apple Silicon memory hierarchy
- **GPU Integration:** Metal Performance Shaders preparation for hybrid CPU/GPU computation phases
- **Multi-threading:** Work-stealing queue architecture for efficient CPU parallelization

**Cross-Platform Extension Requirements:**
- **Graphics API Abstraction:** Metal foundation with extension points for DirectX 12 and Vulkan backends
- **Architecture Flexibility:** Abstract SIMD operations to support both NEON (ARM) and AVX/SSE (x86-64)
- **Platform Optimization:** Maintain platform-specific optimizations without code duplication

## Epic List

### Core Educational Journey (Required)

### Epic 1: Foundation & Mathematical Core
Establish project infrastructure, build system, and implement single-ray Lambert BRDF calculation with console output - delivering both foundational architecture and working mathematical demonstration.

### Epic 2: Visual Foundation & Multi-Ray Rendering
Extend from single-ray to full image generation with camera system, multiple primitive support, and basic scene management - delivering visible rendering results.

### Epic 3: Cook-Torrance Microfacet Theory Implementation
Implement Cook-Torrance microfacet BRDF theory to provide physically accurate specular reflection and understand the mathematical foundation of modern material models - delivering core physics understanding necessary for advanced materials.

### Epic 4: OpenPBR Material System 🎓 **Educational Completion Point**
Implement OpenPBR material model, add advanced lighting techniques, and implement performance monitoring - delivering cutting-edge material accuracy and **complete ray tracing education mastery**.

**Upon completing Epic 4, students have achieved full ray tracing competency and can teach others.**

---

### Advanced Optional Modules (For Motivated Learners)

### Epic 5: Industry Pipeline Exploration ⚠️ **Optional Advanced**
**Reframed Goal:** Conceptual exploration of MaterialX and USD rather than full implementation complexity. Simplified integration examples and workflow understanding to provide industry exposure without overwhelming educational objectives.

### Epic 6: Apple Silicon Optimization Workshop ⚠️ **Optional Advanced**
**Reframed Goal:** Performance learning laboratory focusing on SIMD vectorization learning with educational performance comparison rather than production optimization. Students understand optimization principles through hands-on measurement.

## Epic 1 Foundation & Mathematical Core

**Expanded Goal:** Establish the essential project infrastructure and development environment while implementing the core mathematical foundation of pathtracing through single-ray Lambert BRDF calculation. This epic delivers both the technical foundation necessary for all future development and a working demonstration of the fundamental light transport physics, providing immediate educational value and validation of core concepts before architectural complexity.

### Story 1.1 Project Setup and Build System
As a **graphics programming learner**,  
I want **a properly configured CMake build system with Apple Silicon optimization support**,  
so that **I have a solid development foundation that can scale from simple calculations to complex rendering features**.

#### Acceptance Criteria
1. CMake build system successfully compiles C++20/C++23 code on Apple Silicon with appropriate optimization flags
2. Project structure includes clear separation for source, headers, tests, and documentation with educational organization
3. Git repository is initialized with .gitignore appropriate for C++ development and learning milestone branching strategy
4. README includes build instructions, learning objectives, and architectural decision rationale for future reference
5. Basic unit testing framework is integrated and demonstrates mathematical validation capability

### Story 1.2 Core Mathematical Foundation
As a **graphics programming learner**,  
I want **custom linear algebra implementation with Vector3, Point3, and Ray classes**,  
so that **I understand the fundamental mathematics before using external libraries and can validate calculations manually**.

#### Acceptance Criteria
1. Vector3 class implements dot product, cross product, normalization, and basic arithmetic with inline mathematical comments
2. Point3 class provides point operations and vector relationships with clear geometric interpretation
3. Ray class encapsulates origin, direction, and parameter t with proper mathematical representation
4. All mathematical operations include educational comments explaining the physics and geometry concepts
5. Unit tests validate mathematical correctness against known values and edge cases

### Story 1.3 Single-Ray Lambert BRDF Implementation
As a **graphics programming learner**,  
I want **complete single-ray pathtracing calculation from camera through Lambert surface to console output**,  
so that **I can observe and understand every step of the light transport equation in practice**.

#### Acceptance Criteria
1. Single hard-coded ray from camera origin to specific sphere intersection point with explicit coordinate values
2. Ray-sphere intersection uses proper quadratic equation solution with discriminant checking and root validation
3. Lambert BRDF implementation follows real OpenPBR energy conservation mathematics (n·l) with single point light
4. Complete light transport calculation shows energy flow from light source through surface interaction to camera
5. Console output displays calculated color values AND intermediate mathematical results for debugging and learning
6. Every variable has descriptive names and educational comments explaining its role in the rendering equation

### Story 1.4 Mathematical Validation and Documentation
As a **graphics programming learner**,  
I want **comprehensive validation of all mathematical calculations with educational documentation**,  
so that **I can verify correctness and reference the implementation for future learning**.

#### Acceptance Criteria
1. Manual calculation verification demonstrates that code produces expected results for known input values
2. Edge case handling for ray-sphere intersection (grazing rays, no intersection, behind camera) with clear error messages
3. Energy conservation validation shows that Lambert BRDF properly conserves light energy according to physics
4. Inline code documentation explains every mathematical step with references to rendering equation theory
5. Implementation decision log documents why specific approaches were chosen over alternatives for learning value

## Epic 2 Visual Foundation & Multi-Ray Rendering

**Expanded Goal:** Transform the single-ray mathematical foundation into a complete visual rendering system by implementing multi-ray image generation, camera system, and scene management. This epic delivers the first visible rendered images while maintaining the educational transparency established in Epic 1, enabling visual validation of mathematical concepts and providing the foundation for all future visual features.

### Story 2.1 Image Generation and Pixel Sampling
As a **graphics programming learner**,  
I want **multi-ray rendering that generates complete images from camera-to-pixel coordinate transformation**,  
so that **I can see visual results of the mathematical calculations and understand how individual rays combine into images**.

#### Acceptance Criteria
1. Camera-to-pixel coordinate transformation correctly maps image coordinates to 3D ray directions with educational comments
2. Multi-ray sampling generates one ray per pixel initially, with clear extension points for anti-aliasing
3. Image class manages pixel data with proper color clamping and gamma correction for display
4. PNG image output demonstrates successful rendering of simple sphere scene with visible Lambert shading
5. Console output shows ray count statistics and basic performance timing for educational insight

### Story 2.2 Camera System and Controls
As a **graphics programming learner**,  
I want **interactive camera positioning with field-of-view and look-at controls**,  
so that **I can explore scenes from different perspectives and understand camera mathematics in practice**.

#### Acceptance Criteria
1. Camera class encapsulates position, target, up vector, field-of-view, and image aspect ratio with clear mathematical representation
2. Look-at matrix generation follows standard computer graphics mathematics with educational documentation
3. Field-of-view to focal length conversion demonstrates practical camera optics calculations
4. Command-line parameter support allows easy camera adjustment without recompilation for experimentation
5. Camera ray generation produces correct world-space rays for any camera configuration with validation tests

### Story 2.3 Multi-Primitive Scene Management
As a **graphics programming learner**,  
I want **scenes with multiple spheres and basic spatial organization**,  
so that **I can understand ray-object intersection algorithms and scene traversal patterns**.

#### Acceptance Criteria
1. Scene class manages multiple sphere objects with efficient intersection testing against ray lists
2. Sphere class extends to support position, radius, and material properties with proper encapsulation
3. Ray-scene intersection finds closest hit point across all objects with proper depth testing
4. Scene loading from simple text format demonstrates data-driven scene configuration capability
5. Intersection testing includes educational timing and hit-count statistics for performance understanding

### Story 2.4 Multi-Resolution and Performance Foundation
As a **graphics programming learner**,  
I want **support for different image resolutions with basic performance monitoring**,  
so that **I can understand the relationship between image complexity and rendering performance**.

#### Acceptance Criteria
1. Command-line resolution specification supports common sizes (256x256, 512x512, 1024x1024, 2048x2048)
2. Performance timing tracks ray generation, intersection testing, and shading calculation phases separately
3. Memory usage monitoring shows relationship between image size and memory consumption
4. Progress reporting provides rendering feedback for larger images with educational timing breakdowns
5. Resolution scaling maintains correct aspect ratios and camera field-of-view relationships

## Epic 3: Cook-Torrance Microfacet Theory Implementation

**Expanded Goal:** Implement Cook-Torrance microfacet BRDF theory to provide physically accurate specular reflection and understand the mathematical foundation of modern material models. This epic transitions from simple Lambert diffuse to rigorous microfacet theory while maintaining educational transparency, delivering the core physics understanding necessary for OpenPBR implementation.

### Story 3.1 Pure Cook-Torrance BRDF Implementation
As a **graphics programming learner**,  
I want **complete Cook-Torrance microfacet BRDF with proper normal distribution, geometry, and Fresnel terms**,  
so that **I can understand the mathematical foundation of physically based rendering**.

#### Acceptance Criteria
1. Normal Distribution Function (D) implements GGX/Trowbridge-Reitz distribution with roughness parameter and educational mathematical breakdown
2. Geometry Function (G) implements Smith masking-shadowing model with proper self-shadowing calculations
3. Fresnel Function (F) provides accurate dielectric and conductor reflection with complex refractive index support
4. Complete Cook-Torrance evaluation combines D, G, F terms with proper normalization and energy conservation
5. Console output shows detailed mathematical breakdown of each BRDF component for learning validation

### Story 3.2 Advanced Lighting and Multiple Lights
As a **graphics programming learner**,  
I want **multiple light types including directional, point, and area lights**,  
so that **I can understand different lighting models and their impact on microfacet material appearance**.

#### Acceptance Criteria
1. Light base class supports point, directional, and simple area light implementations with proper falloff
2. Multiple light sampling correctly accumulates Cook-Torrance contributions from all light sources in scene
3. Shadow ray testing prevents light contribution from occluded sources with educational ray counting
4. Light importance sampling demonstrates basic Monte Carlo integration techniques for specular highlights
5. Lighting debug visualization shows individual light contributions and specular highlight behavior

### Story 3.3 Interactive Material Parameter Controls
As a **graphics programming learner**,  
I want **real-time Cook-Torrance parameter adjustment through command-line interface**,  
so that **I can experiment with microfacet properties and observe immediate visual feedback**.

#### Acceptance Criteria
1. Command-line interface supports real-time adjustment of roughness, metallic, and base color parameters
2. Material parameter changes trigger immediate re-render with Cook-Torrance evaluation timing feedback
3. Parameter validation ensures physically plausible ranges with educational explanations of microfacet theory
4. Preset material library includes common Cook-Torrance configurations (metal, dielectric, rough, smooth)
5. Material state serialization allows saving and loading of microfacet configurations for experimentation

### Story 3.4 Fresnel Equations and Layered Materials Foundation
As a **graphics programming learner**,  
I want **complete Fresnel equation implementation and basic material layering**,  
so that **I can understand physically accurate reflection and prepare for OpenPBR complexity**.

#### Acceptance Criteria
1. Fresnel equations support both dielectric (glass, plastic) and conductor (metal) materials with proper complex IOR
2. Basic material layering demonstrates diffuse base with specular coating using proper energy conservation
3. Material examples demonstrate glass, metal, plastic, and ceramic rendering with accurate Fresnel behavior
4. Fresnel angle dependency visualization shows how reflection changes with viewing angle
5. Material validation suite ensures mathematical correctness against known Cook-Torrance reference implementations

## Epic 4: OpenPBR Material System 🎓 **Educational Completion Point**

**Expanded Goal:** Implement the complete OpenPBR material model to achieve cutting-edge material accuracy and **complete ray tracing education mastery**. This epic builds on Cook-Torrance understanding to deliver industry-leading material complexity while maintaining educational transparency, providing comprehensive multi-lobe BRDF support and advanced lighting techniques.

**Educational Graduation Milestone:** Upon completing Epic 4, students have achieved full ray tracing competency with production-quality skills and can teach ray tracing concepts to others.

### Story 4.1 OpenPBR Base Material Implementation
As a **graphics programming learner**,  
I want **complete OpenPBR base material layer with proper energy conservation and layering**,  
so that **I can understand the most advanced physically accurate material model in the industry**.

#### Acceptance Criteria
1. OpenPBR base material implements diffuse, specular, metallic workflow with proper energy conservation mathematics
2. Base color, metallic, roughness, and specular parameters follow OpenPBR specification exactly with validation
3. Material layering system properly handles energy distribution between diffuse and specular components
4. OpenPBR validation suite ensures compliance with official specification test cases
5. Educational documentation explains OpenPBR improvements over Cook-Torrance and Standard Surface approaches

### Story 4.2 OpenPBR Advanced Material Layers
As a **graphics programming learner**,  
I want **OpenPBR coating, emission, and subsurface scattering layers**,  
so that **I can render complex materials with multiple physically accurate interaction types**.

#### Acceptance Criteria
1. Coating layer implements proper clearcoat with independent roughness, IOR, and tinting parameters
2. Emission layer provides physically accurate light emission with proper energy units and color temperature
3. Subsurface scattering implements basic subsurface approximation with scattering distance and color parameters
4. Layer interaction follows OpenPBR energy conservation rules with proper masking and blending
5. Complex material examples demonstrate automotive paint, skin, wax, and translucent materials

### Story 4.3 Advanced Sampling and Performance Monitoring
As a **graphics programming learner**,  
I want **importance sampling for OpenPBR materials with comprehensive performance monitoring**,  
so that **I can understand Monte Carlo techniques and optimization strategies for complex materials**.

#### Acceptance Criteria
1. Importance sampling implementation reduces noise for specular highlights and coating layers
2. Multiple importance sampling combines light and BRDF sampling strategies effectively
3. Performance monitoring tracks BRDF evaluation cost, sampling efficiency, and convergence rates
4. Adaptive sampling demonstrates quality-driven rendering with educational performance analysis
5. Sampling visualization shows sample distribution and convergence behavior for learning

### Story 4.4 Interactive OpenPBR Material Editor
As a **graphics programming learner**,  
I want **comprehensive material editing interface supporting all OpenPBR parameters**,  
so that **I can experiment with industry-standard material authoring workflows**.

#### Acceptance Criteria
1. Interactive parameter controls support all OpenPBR material layers with real-time preview
2. Material presets library includes comprehensive OpenPBR material examples from industry references
3. Parameter validation ensures OpenPBR specification compliance with educational explanations
4. Material export/import supports OpenPBR-compliant material file format for workflow learning
5. Side-by-side comparison tools demonstrate differences between Cook-Torrance and OpenPBR implementations

## Epic 5: MaterialX Integration & USD Pipeline

**Expanded Goal:** Integrate MaterialX node graph evaluation and USD scene loading with comprehensive material authoring workflow to deliver production pipeline compatibility. This epic transforms the standalone renderer into a production-ready tool capable of handling industry-standard content creation workflows while maintaining educational transparency.

### Story 5.1 MaterialX Node Graph Integration
As a **graphics programming learner**,
I want **MaterialX node graph evaluation supporting OpenPBR materials**,
so that **I can understand production material authoring workflows and node-based material creation**.

### Story 5.2 USD Scene Loading Pipeline
As a **graphics programming learner**,
I want **complete USD scene loading with geometry, materials, and lighting data**,
so that **I can render industry-standard scene files and understand production pipeline formats**.

### Story 5.3 Production Material Authoring Workflow  
As a **graphics programming learner**,
I want **comprehensive material authoring tools with MaterialX export capability**,
so that **I can create materials that integrate with professional content creation workflows**.

## Epic 6: Apple Silicon Optimization & Advanced Features

**Expanded Goal:** Implement NEON vectorization, Metal Performance Shaders integration, multi-threading, and advanced rendering techniques to deliver optimized production-quality performance while maintaining the educational foundation established in earlier epics.

### Story 6.1 NEON Vectorization and Apple Silicon Optimization
As a **graphics programming learner**,
I want **Apple Silicon NEON vectorization for ray tracing calculations**,
so that **I can understand platform-specific optimization while maintaining cross-platform design**.

### Story 6.2 Multi-threading and Performance Scaling
As a **graphics programming learner**,
I want **efficient CPU parallelization with work-stealing queues**,
so that **I can understand high-performance rendering architecture and scaling techniques**.

### Story 6.3 Advanced Rendering Features
As a **graphics programming learner**,
I want **advanced pathtracing features including importance sampling and denoising**,
so that **I can implement production-quality rendering techniques and optimization strategies**.

## Checklist Results Report

### Executive Summary
- **Overall PRD Completeness:** 85%
- **MVP Scope Appropriateness:** Just Right  
- **Readiness for Architecture Phase:** Ready
- **Most Critical Gaps:** Missing detailed acceptance criteria for Epic 5-6 stories, some non-functional requirements need refinement

### Category Analysis Results
- **Problem Definition & Context:** PASS - Excellent problem definition with comprehensive context
- **MVP Scope Definition:** PASS - Clear MVP boundaries, good progression through complexity  
- **User Experience Requirements:** PASS - Well-defined for educational tool, appropriate developer focus
- **Functional Requirements:** PASS - Comprehensive FR/NFR coverage with clear educational progression
- **Non-Functional Requirements:** PARTIAL - Performance requirements could be more specific
- **Epic & Story Structure:** PARTIAL - Epic 1-4 excellent, Epic 5-6 need detailed acceptance criteria
- **Technical Guidance:** PASS - Excellent technical assumptions, Physics-First approach well-defined
- **Cross-Functional Requirements:** PARTIAL - Integration requirements present but could be more detailed  
- **Clarity & Communication:** PASS - Excellent structure, clear educational focus throughout

### Key Strengths
- Physics-First material progression (Lambert → Cook-Torrance → OpenPBR → MaterialX) provides excellent educational foundation
- Incremental complexity management from single-ray to production-quality rendering
- Strong educational focus with mathematical transparency throughout
- Appropriate Apple Silicon optimization approach with cross-platform considerations
- Clear epic sequencing with logical dependencies and value delivery

### Final Validation: ✅ READY FOR ARCHITECT

The PRD provides comprehensive guidance for architectural design and implementation planning.

## Next Steps

### UX Expert Prompt
Review this PRD and create user experience architecture for the educational pathtracer interface, focusing on mathematical transparency and learning workflow optimization.

### Architect Prompt  
Review this PRD and create technical architecture for the modular pathtracer system, emphasizing educational transparency while supporting the progression from single-ray calculations to production-quality hybrid rendering.
