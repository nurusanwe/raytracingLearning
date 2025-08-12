# Components

Based on the refined hybrid educational architecture, the system components follow the epic progression from simple single-file implementation to modular production-quality architecture. Each component serves both educational transparency and performance objectives through evolutionary development.

## Epic 1: Single-File Educational Component

**Responsibility:** Complete ray tracing demonstration in maximum educational transparency

**Key Interfaces:**
- `Epic1::trace_single_ray()` - Hard-coded educational example with full mathematical breakdown
- `Epic1::trace_ray_with_parameters()` - Parameterized version for student exploration

**Dependencies:** None - completely self-contained for educational clarity

**Technology Stack:** Pure C++ with educational console output, no external dependencies

## Ray Tracing Engine Core

**Responsibility:** Core mathematical ray tracing operations that students implement and evolve across all epics

**Key Interfaces:**
- `Ray` class - Light ray representation with educational debugging
- `Material::evaluate_brdf()` - BRDF evaluation (Lambert → Cook-Torrance → OpenPBR progression)
- `Scene::intersect()` - Ray-scene intersection testing
- `Camera::generate_ray()` - Primary ray generation from camera

**Dependencies:** EducationalMode controller, Vector3 mathematical foundation

**Technology Stack:** C++ with progressive optimization (scalar → SIMD → multi-threaded)

## Mathematical Foundation Component

**Responsibility:** Core mathematical operations (Vector3, Matrix4x4) with educational transparency and optimization progression

**Key Interfaces:**
- `Vector3::dot()`, `Vector3::cross()`, `Vector3::normalized()` - Basic vector operations
- Mathematical validation and educational explanation methods

**Dependencies:** EducationalMode controller for optional transparency features

**Technology Stack:** Custom C++ implementation progressing from educational verbosity to NEON-optimized SIMD

## Educational Control System

**Responsibility:** Unified control of educational features across all components

**Key Interfaces:**
- `EducationalMode::set_level()` - Single control point for educational transparency
- `EducationalMode::is_explanation_enabled()` - Consistent educational feature querying
- Educational memory management and cleanup

**Dependencies:** None - provides services to all other components

**Technology Stack:** C++ with thread-safe atomic operations for educational state management
