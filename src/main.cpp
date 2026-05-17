#include <CLI/CLI.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "dfrog/daemon.hpp"
#include "dfrog/version.hpp"
#include "dfrog/version_info.hpp"

int main(int argc, char** argv) {
    CLI::App app{"dfrog — Deterministic Fault-Resilient Operations Guardian"};
    app.set_version_flag("--version,-V", std::string{dfrog::version_banner()});

    dfrog::DaemonConfig config;
    app.add_option("-c,--config", config.config_path, "Path to dfrog YAML config")
        ->envname("DFROG_CONFIG");
    app.add_flag("--validate-only", config.validate_only, "Validate config and exit.");

    CLI11_PARSE(app, argc, argv);

    std::cout << dfrog::version_banner() << '\n';

    dfrog::Daemon daemon{std::move(config)};
    return daemon.run();
}
