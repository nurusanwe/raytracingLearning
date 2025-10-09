#pragma once
#include <iostream>
#include <string>
#include <sstream>

// Verbosity levels for controlling educational output
// Silent: Only critical errors
// Normal: Standard output without educational details
// Verbose: Full educational breakdowns (original behavior)
// Debug: Everything including debug info
enum class VerbosityLevel {
    Silent = 0,    // Only errors
    Normal = 1,    // Normal output, no educational details
    Verbose = 2,   // Full educational output (original default)
    Debug = 3      // Debug info + all educational output
};

// Log message types with different display requirements
enum class LogType {
    Error,         // Always shown (unless silent)
    Warning,       // Shown in Normal and above
    Info,          // Shown in Normal and above
    Educational,   // Shown in Verbose and above (mathematical breakdowns, explanations)
    Debug          // Shown in Debug only
};

// Global logger for controlling verbosity throughout the raytracer
// Educational focus: allows toggling between learning mode (verbose) and production mode (quiet)
// Static design: single global verbosity setting accessible from all components
class Logger {
private:
    static VerbosityLevel current_level;

public:
    // Set global verbosity level
    static void set_level(VerbosityLevel level) {
        current_level = level;
    }

    static VerbosityLevel get_level() {
        return current_level;
    }

    // Check if a log type should be shown at current verbosity level
    static bool should_log(LogType type) {
        switch (type) {
            case LogType::Error:
                return current_level >= VerbosityLevel::Silent;
            case LogType::Warning:
            case LogType::Info:
                return current_level >= VerbosityLevel::Normal;
            case LogType::Educational:
                return current_level >= VerbosityLevel::Verbose;
            case LogType::Debug:
                return current_level >= VerbosityLevel::Debug;
            default:
                return false;
        }
    }

    // Core logging function
    static void log(LogType type, const std::string& message) {
        if (should_log(type)) {
            std::cout << message << std::endl;
        }
    }

    // Convenience functions for different log types
    static void error(const std::string& msg) {
        if (should_log(LogType::Error)) {
            std::cout << "ERROR: " << msg << std::endl;
        }
    }

    static void warning(const std::string& msg) {
        if (should_log(LogType::Warning)) {
            std::cout << "WARNING: " << msg << std::endl;
        }
    }

    static void info(const std::string& msg) {
        if (should_log(LogType::Info)) {
            std::cout << msg << std::endl;
        }
    }

    static void educational(const std::string& msg) {
        if (should_log(LogType::Educational)) {
            std::cout << msg << std::endl;
        }
    }

    static void debug(const std::string& msg) {
        if (should_log(LogType::Debug)) {
            std::cout << "DEBUG: " << msg << std::endl;
        }
    }

    // Helper functions to check current verbosity state
    static bool is_verbose() {
        return current_level >= VerbosityLevel::Verbose;
    }

    static bool is_debug() {
        return current_level >= VerbosityLevel::Debug;
    }

    static bool is_quiet() {
        return current_level <= VerbosityLevel::Normal;
    }

    static bool is_silent() {
        return current_level == VerbosityLevel::Silent;
    }
};

// Initialize static member - default to Verbose for educational mode
inline VerbosityLevel Logger::current_level = VerbosityLevel::Verbose;
