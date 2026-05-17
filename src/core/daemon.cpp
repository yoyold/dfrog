#include "dfrog/daemon.hpp"

// Asio's executor equality operators trip GCC 13's -Wnull-dereference under
// -O3 once they are inlined. The headers are included as -isystem but that
// classification is lost by the time the analyzer runs after inlining, so
// scope-suppress the diagnostic just across the asio includes.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#endif
#include <asio/io_context.hpp>
#include <asio/signal_set.hpp>
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#include <csignal>
#include <string_view>
#include <utility>

#include "dfrog/lifecycle.hpp"
#include "dfrog/logging.hpp"

namespace dfrog {

namespace {

constexpr std::string_view signal_name(int sig) noexcept {
    switch (sig) {
        case SIGTERM:
            return "SIGTERM";
        case SIGINT:
            return "SIGINT";
        default:
            return "unknown";
    }
}

}  // namespace

struct Daemon::Impl {
    explicit Impl(DaemonConfig cfg) : config(std::move(cfg)), signals(io, SIGTERM, SIGINT) {
    }

    void install_signal_handler() {
        signals.async_wait([this](const asio::error_code& ec, int sig) {
            if (ec) {
                return;
            }
            log::info("daemon.signal", {{"signal", signal_name(sig)}});
            this->stop();
        });
    }

    void stop() {
        if (lifecycle.try_transition(State::Draining)) {
            log::info("daemon.draining", {});
        }
        io.stop();
    }

    int run() {
        log::info(
            "daemon.starting",
            {{"config", config.config_path},
             {"validate_only", config.validate_only ? "true" : "false"}});

        if (config.validate_only) {
            // No config layer yet — validate-only is currently a no-op success.
            log::info("daemon.validate_only", {{"result", "ok"}});
            return 0;
        }

        install_signal_handler();

        if (!lifecycle.try_transition(State::Ready)) {
            log::error("daemon.startup_failed", {{"reason", "cannot reach Ready"}});
            return 1;
        }
        log::info("daemon.ready", {});

        io.run();

        // Defensive: io may have been stopped before the signal handler ran
        // (e.g. via request_shutdown() before run()), so the state may still
        // be Ready. Always walk through Draining on the way to Stopped.
        lifecycle.try_transition(State::Draining);
        lifecycle.try_transition(State::Stopped);
        log::info("daemon.stopped", {});
        return 0;
    }

    DaemonConfig config;
    asio::io_context io;
    asio::signal_set signals;
    Lifecycle lifecycle;
};

Daemon::Daemon(DaemonConfig config) : impl_(std::make_unique<Impl>(std::move(config))) {
}

Daemon::~Daemon() = default;

int Daemon::run() {
    return impl_->run();
}

void Daemon::request_shutdown() {
    impl_->stop();
}

}  // namespace dfrog
