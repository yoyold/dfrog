#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "dfrog/daemon.hpp"

namespace dfrog {

TEST(Daemon, ValidateOnlyReturnsZeroImmediately) {
    DaemonConfig cfg;
    cfg.validate_only = true;
    Daemon d{std::move(cfg)};
    EXPECT_EQ(d.run(), 0);
}

TEST(Daemon, RunReturnsZeroWhenShutdownRequestedFirst) {
    Daemon d{DaemonConfig{}};
    d.request_shutdown();
    EXPECT_EQ(d.run(), 0);
}

TEST(Daemon, RunReturnsZeroWhenShutdownRequestedFromAnotherThread) {
    Daemon d{DaemonConfig{}};
    std::thread shutter([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        d.request_shutdown();
    });
    EXPECT_EQ(d.run(), 0);
    shutter.join();
}

}  // namespace dfrog
