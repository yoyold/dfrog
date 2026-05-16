cmake_minimum_required(VERSION 3.25)

# Read project version from a single source of truth.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" DFROG_VERSION_RAW LIMIT_COUNT 1)
string(STRIP "${DFROG_VERSION_RAW}" DFROG_VERSION)

project(dfrog
    VERSION ${DFROG_VERSION}
    DESCRIPTION "Deterministic Fault-Resilient Operations Guardian"
    HOMEPAGE_URL "https://github.com/your-org/dfrog"
    LANGUAGES CXX
)

# Out-of-tree builds only.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not allowed. Run cmake from a separate build directory, "
        "e.g. `cmake --preset dev`.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Always emit compile_commands.json for clangd / clang-tidy.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Debug if no build type was set (for single-config generators).
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Warnings)
include(Sanitizers)

# Options.
option(DFROG_BUILD_TESTS "Build the dfrog test suite" ON)
option(DFROG_ENABLE_ASAN "Enable AddressSanitizer + UBSan in dev builds" OFF)
option(DFROG_ENABLE_TSAN "Enable ThreadSanitizer in dev builds" OFF)
option(DFROG_WERROR "Promote warnings to errors" ON)

# Generated version header (single source of truth: VERSION file).
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/dfrog/version.hpp"
    @ONLY
)

add_subdirectory(src)

if(DFROG_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
