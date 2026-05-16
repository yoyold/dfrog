#include <CLI/CLI.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "dfrog/version.hpp"
#include "dfrog/version_info.hpp"

int main(int argc, char** argv) {
    CLI::App app{"dfrog — Deterministic Fault-Resilient Operations Guardian"};
    app.set_version_flag("--version,-V", std::string{dfrog::version_banner()});

    std::string config_path{"/etc/dfrog/config.yaml"};
    app.add_option("-c,--config", config_path, "Path to dfrog YAML config")
        ->envname("DFROG_CONFIG");

    bool validate_only{false};
    app.add_flag("--validate-only", validate_only, "Validate config and exit.");

    CLI11_PARSE(app, argc, argv);

    std::cout << dfrog::version_banner() << '\n';
    std::cout << "config: " << config_path;
    if (validate_only) {
        std::cout << " (validate-only)";
    }
    std::cout << '\n';
    return EXIT_SUCCESS;
}
