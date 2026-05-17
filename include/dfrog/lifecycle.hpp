#pragma once

#include <atomic>
#include <string_view>

namespace dfrog {

enum class State {
    Booting,
    Ready,
    Degraded,
    Draining,
    Stopped,
};

[[nodiscard]] std::string_view state_name(State s) noexcept;

[[nodiscard]] bool is_valid_transition(State from, State to) noexcept;

class Lifecycle {
public:
    Lifecycle() noexcept = default;

    Lifecycle(const Lifecycle&) = delete;
    Lifecycle& operator=(const Lifecycle&) = delete;
    Lifecycle(Lifecycle&&) = delete;
    Lifecycle& operator=(Lifecycle&&) = delete;
    ~Lifecycle() = default;

    [[nodiscard]] State get() const noexcept;

    bool try_transition(State next) noexcept;

private:
    std::atomic<State> state_{State::Booting};
};

}  // namespace dfrog
