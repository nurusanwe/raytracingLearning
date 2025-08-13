#include <iostream>

// Cross-platform preprocessor directives
#ifdef PLATFORM_APPLE
    #include <TargetConditionals.h>
    constexpr const char* PLATFORM_NAME = "Apple";
#elif defined(PLATFORM_WINDOWS)
    constexpr const char* PLATFORM_NAME = "Windows";
#elif defined(PLATFORM_LINUX)
    constexpr const char* PLATFORM_NAME = "Linux";
#else
    constexpr const char* PLATFORM_NAME = "Unknown";
#endif

int main() {
    std::cout << "=== Educational Ray Tracer - Epic 1 Foundation ===" << std::endl;
    std::cout << "Platform: " << PLATFORM_NAME << std::endl;
    std::cout << "C++ Standard: " << __cplusplus << std::endl;
    
    #ifdef PLATFORM_APPLE
        #if TARGET_CPU_ARM64
            std::cout << "Apple Silicon optimization: Enabled" << std::endl;
        #else
            std::cout << "Apple Intel optimization: Enabled" << std::endl;
        #endif
    #endif
    
    std::cout << "Build system verification: SUCCESS" << std::endl;
    std::cout << "Ready for Epic 1 mathematical foundation development." << std::endl;
    
    return 0;
}