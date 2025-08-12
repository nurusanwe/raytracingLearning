# Technical Assumptions

## Repository Structure: Monorepo
Single repository containing all development phases from MVP through production features to maintain learning continuity and enable easy comparison between implementation milestones.

## Service Architecture
**Modular Monolith evolving to Plugin-based Architecture** - Begin with single-file implementation for mathematical transparency, then gradually extract modules based on actual usage patterns rather than theoretical abstractions. Final architecture supports hot-swappable rendering backends (pathtracing, future rasterization) and material systems.

## Testing Requirements
**Full Testing Pyramid with Educational Focus** - Custom assertion framework for mathematical validation expanding to comprehensive unit testing suite. Include visual regression testing for rendering accuracy and performance benchmarking tools specific to graphics workloads.

## Additional Technical Assumptions and Requests

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
