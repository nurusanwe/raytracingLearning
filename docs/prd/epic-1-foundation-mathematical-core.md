# Epic 1 Foundation & Mathematical Core

**Expanded Goal:** Establish the essential project infrastructure and development environment while implementing the core mathematical foundation of pathtracing through single-ray Lambert BRDF calculation. This epic delivers both the technical foundation necessary for all future development and a working demonstration of the fundamental light transport physics, providing immediate educational value and validation of core concepts before architectural complexity.

## Story 1.1 Project Setup and Build System
As a **graphics programming learner**,  
I want **a properly configured CMake build system with Apple Silicon optimization support**,  
so that **I have a solid development foundation that can scale from simple calculations to complex rendering features**.

### Acceptance Criteria
1. CMake build system successfully compiles C++20/C++23 code on Apple Silicon with appropriate optimization flags
2. Project structure includes clear separation for source, headers, tests, and documentation with educational organization
3. Git repository is initialized with .gitignore appropriate for C++ development and learning milestone branching strategy
4. README includes build instructions, learning objectives, and architectural decision rationale for future reference
5. Basic unit testing framework is integrated and demonstrates mathematical validation capability

## Story 1.2 Core Mathematical Foundation
As a **graphics programming learner**,  
I want **custom linear algebra implementation with Vector3, Point3, and Ray classes**,  
so that **I understand the fundamental mathematics before using external libraries and can validate calculations manually**.

### Acceptance Criteria
1. Vector3 class implements dot product, cross product, normalization, and basic arithmetic with inline mathematical comments
2. Point3 class provides point operations and vector relationships with clear geometric interpretation
3. Ray class encapsulates origin, direction, and parameter t with proper mathematical representation
4. All mathematical operations include educational comments explaining the physics and geometry concepts
5. Unit tests validate mathematical correctness against known values and edge cases

## Story 1.3 Single-Ray Lambert BRDF Implementation
As a **graphics programming learner**,  
I want **complete single-ray pathtracing calculation from camera through Lambert surface to console output**,  
so that **I can observe and understand every step of the light transport equation in practice**.

### Acceptance Criteria
1. Single hard-coded ray from camera origin to specific sphere intersection point with explicit coordinate values
2. Ray-sphere intersection uses proper quadratic equation solution with discriminant checking and root validation
3. Lambert BRDF implementation follows real OpenPBR energy conservation mathematics (n·l) with single point light
4. Complete light transport calculation shows energy flow from light source through surface interaction to camera
5. Console output displays calculated color values AND intermediate mathematical results for debugging and learning
6. Every variable has descriptive names and educational comments explaining its role in the rendering equation

## Story 1.4 Mathematical Validation and Documentation
As a **graphics programming learner**,  
I want **comprehensive validation of all mathematical calculations with educational documentation**,  
so that **I can verify correctness and reference the implementation for future learning**.

### Acceptance Criteria
1. Manual calculation verification demonstrates that code produces expected results for known input values
2. Edge case handling for ray-sphere intersection (grazing rays, no intersection, behind camera) with clear error messages
3. Energy conservation validation shows that Lambert BRDF properly conserves light energy according to physics
4. Inline code documentation explains every mathematical step with references to rendering equation theory
5. Implementation decision log documents why specific approaches were chosen over alternatives for learning value
