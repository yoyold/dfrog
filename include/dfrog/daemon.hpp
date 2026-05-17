#pragma once

#include <memory>
#include <string>

namespace dfrog {

struct DaemonConfig {
    std::string config_path = "/etc/dfrog/config.yaml";
    bool validate_only = false;
};

// Owns the event loop, the signal handlers, and the process lifecycle.
// Construct once per process; run() blocks until a shutdown is requested
// (SIGTERM / SIGINT, or an explicit call to request_shutdown()).
class Daemon {
public:
    explicit Daemon(DaemonConfig config);
    ~Daemon();

    Daemon(const Daemon&) = delete;
    Daemon& operator=(const Daemon&) = delete;
    Daemon(Daemon&&) = delete;
    Daemon& operator=(Daemon&&) = delete;

    // Block on the event loop. Returns the process exit code (0 on graceful
    // shutdown).
    [[nodiscard]] int run();

    // Thread-safe. May be called before or during run(). Pre-run calls cause
    // the subsequent run() to return immediately.
    void request_shutdown();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace dfrog
