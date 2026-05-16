#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>

#include "dfrog/lifecycle.hpp"

namespace dfrog {

TEST(Lifecycle, StartsInBooting) {
    Lifecycle l;
    EXPECT_EQ(l.get(), State::Booting);
}

TEST(StateName, AllStatesRenderable) {
    EXPECT_EQ(state_name(State::Booting), "booting");
    EXPECT_EQ(state_name(State::Ready), "ready");
    EXPECT_EQ(state_name(State::Degraded), "degraded");
    EXPECT_EQ(state_name(State::Draining), "draining");
    EXPECT_EQ(state_name(State::Stopped), "stopped");
}

TEST(IsValidTransition, BootingTransitions) {
    EXPECT_TRUE(is_valid_transition(State::Booting, State::Ready));
    EXPECT_TRUE(is_valid_transition(State::Booting, State::Stopped));
    EXPECT_FALSE(is_valid_transition(State::Booting, State::Degraded));
    EXPECT_FALSE(is_valid_transition(State::Booting, State::Draining));
    EXPECT_FALSE(is_valid_transition(State::Booting, State::Booting));
}

TEST(IsValidTransition, ReadyTransitions) {
    EXPECT_TRUE(is_valid_transition(State::Ready, State::Degraded));
    EXPECT_TRUE(is_valid_transition(State::Ready, State::Draining));
    EXPECT_FALSE(is_valid_transition(State::Ready, State::Booting));
    EXPECT_FALSE(is_valid_transition(State::Ready, State::Stopped));
    EXPECT_FALSE(is_valid_transition(State::Ready, State::Ready));
}

TEST(IsValidTransition, DegradedCanRecoverOrDrain) {
    EXPECT_TRUE(is_valid_transition(State::Degraded, State::Ready));
    EXPECT_TRUE(is_valid_transition(State::Degraded, State::Draining));
    EXPECT_FALSE(is_valid_transition(State::Degraded, State::Stopped));
    EXPECT_FALSE(is_valid_transition(State::Degraded, State::Booting));
}

TEST(IsValidTransition, DrainingOnlyToStopped) {
    EXPECT_TRUE(is_valid_transition(State::Draining, State::Stopped));
    EXPECT_FALSE(is_valid_transition(State::Draining, State::Ready));
    EXPECT_FALSE(is_valid_transition(State::Draining, State::Degraded));
    EXPECT_FALSE(is_valid_transition(State::Draining, State::Booting));
}

TEST(IsValidTransition, StoppedIsTerminal) {
    EXPECT_FALSE(is_valid_transition(State::Stopped, State::Booting));
    EXPECT_FALSE(is_valid_transition(State::Stopped, State::Ready));
    EXPECT_FALSE(is_valid_transition(State::Stopped, State::Degraded));
    EXPECT_FALSE(is_valid_transition(State::Stopped, State::Draining));
}

TEST(LifecycleTryTransition, ValidTransitionApplies) {
    Lifecycle l;
    ASSERT_EQ(l.get(), State::Booting);
    EXPECT_TRUE(l.try_transition(State::Ready));
    EXPECT_EQ(l.get(), State::Ready);
}

TEST(LifecycleTryTransition, InvalidTransitionLeavesStateUnchanged) {
    Lifecycle l;
    ASSERT_EQ(l.get(), State::Booting);
    EXPECT_FALSE(l.try_transition(State::Degraded));
    EXPECT_EQ(l.get(), State::Booting);
}

TEST(LifecycleTryTransition, FullHappyPath) {
    Lifecycle l;
    ASSERT_TRUE(l.try_transition(State::Ready));
    ASSERT_TRUE(l.try_transition(State::Degraded));
    ASSERT_TRUE(l.try_transition(State::Ready));
    ASSERT_TRUE(l.try_transition(State::Draining));
    ASSERT_TRUE(l.try_transition(State::Stopped));
    EXPECT_EQ(l.get(), State::Stopped);
    EXPECT_FALSE(l.try_transition(State::Ready));
    EXPECT_EQ(l.get(), State::Stopped);
}

TEST(LifecycleTryTransition, ConcurrentTransitionsHaveExactlyOneWinner) {
    Lifecycle l;
    ASSERT_TRUE(l.try_transition(State::Ready));

    constexpr int kThreads = 16;
    std::atomic<int> winners{0};
    std::vector<std::thread> ts;
    ts.reserve(kThreads);
    for (int i = 0; i < kThreads; ++i) {
        ts.emplace_back([&] {
            if (l.try_transition(State::Draining)) {
                winners.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }
    for (auto& t : ts) {
        t.join();
    }
    EXPECT_EQ(winners.load(), 1);
    EXPECT_EQ(l.get(), State::Draining);
}

}  // namespace dfrog
