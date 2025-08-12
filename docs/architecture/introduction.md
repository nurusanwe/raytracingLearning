# Introduction

This document outlines the complete system architecture for **3D Pathtracer Learning Project**, a native C++ educational rendering engine that bridges mathematical transparency with production-quality patterns. Unlike traditional fullstack web applications, this "fullstack" encompasses the complete rendering pipeline: backend ray tracing engine + frontend interactive interface + industry-standard content pipeline integration.

**Critical Architectural Principle:** **Education-First with Production Patterns** - Every architectural decision must demonstrably serve learning objectives while establishing transferable industry patterns. When conflicts arise, educational transparency takes precedence in early epics, with production quality becoming dominant in later phases.

## Architectural Decision Framework

To address the inherent tension between educational and production requirements, this architecture establishes clear decision criteria:

**Epic 1-2 (Foundation):** Prioritize mathematical transparency and build system simplicity. Accept some performance overhead for educational clarity.

**Epic 3-4 (Industry Standards):** Balance educational visibility with industry pattern adoption. Introduce abstractions gradually with clear before/after comparisons.

**Epic 5-6 (Production Quality):** Prioritize production patterns while maintaining educational documentation of optimization decisions.

## Platform Strategy

**Primary Platform:** macOS Apple Silicon for optimization learning
**Cross-Platform Foundation:** Abstract SIMD operations and rendering backends from Epic 1 using preprocessor directives and function pointers, not complex abstraction hierarchies.

**Risk Mitigation:** Build minimal cross-platform compatibility early (console interface works everywhere) rather than waiting until Epic 6 for platform abstraction.

## Starter Template Decision

**Status:** Greenfield C++ project with **educational-specific build system**

**Key Insight:** Rather than following traditional CMake patterns, the build system will prioritize:
1. **Single Command Setup:** `./build.sh` works on any system with minimal dependencies
2. **Educational Visibility:** Build process shows exactly what's happening (no hidden CMake complexity)  
3. **Incremental Complexity:** Build requirements grow with epic progression, not front-loaded

**Risk Mitigation:** Prototype build system and Dear ImGui integration in Epic 1 to validate architectural assumptions before committing to later epic dependencies.

## Change Log

| Date | Version | Description | Author |
|------|---------|-------------|--------|
| 2025-08-10 | 1.0 | Initial architecture with risk mitigation strategies | Winston (Architect) |
