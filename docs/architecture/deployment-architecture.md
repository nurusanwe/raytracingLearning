# Deployment Architecture

Based on the simplified single-codebase educational architecture, deployment focuses on **local development environments** rather than traditional web application deployment patterns.

## Deployment Strategy

**Educational Application Deployment:**
- **Platform:** Native C++ executable for local development and learning
- **Build Command:** `./tools/build_simple.sh` (single command for all platforms)
- **Output Directory:** `build/` directory with `raytracer` executable
- **Distribution Method:** Git repository cloning with educational progression through branches

**Development Environment Requirements:**
- **Primary Platform:** macOS with Apple Silicon for optimization learning (Epic 6)
- **Secondary Platform:** macOS Intel and Linux for cross-platform compatibility validation
- **Minimum Requirements:** C++20 compiler, basic OpenGL support, git
- **Educational Dependencies:** Dear ImGui (bundled), STB libraries (single-header)
