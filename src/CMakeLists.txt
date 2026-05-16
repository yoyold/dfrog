find_package(CLI11 CONFIG REQUIRED)

# ---- Core library (so tests can link against the same code as the daemon) ----
add_library(dfrog_core STATIC
    core/version_info.cpp
)
target_include_directories(dfrog_core
    PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/generated"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
dfrog_set_warnings(dfrog_core)
dfrog_apply_sanitizers(dfrog_core)

# ---- Daemon binary ----
add_executable(dfrogd main.cpp)
target_link_libraries(dfrogd PRIVATE dfrog_core CLI11::CLI11)
dfrog_set_warnings(dfrogd)
dfrog_apply_sanitizers(dfrogd)

set_target_properties(dfrogd PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS dfrogd RUNTIME DESTINATION bin)
