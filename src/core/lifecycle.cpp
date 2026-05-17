#include "dfrog/lifecycle.hpp"

namespace dfrog {

std::string_view state_name(State s) noexcept {
    switch (s) {
        case State::Booting:
            return "booting";
        case State::Ready:
            return "ready";
        case State::Degraded:
            return "degraded";
        case State::Draining:
            return "draining";
        case State::Stopped:
            return "stopped";
    }
    return "unknown";
}

bool is_valid_transition(State from, State to) noexcept {
    if (from == to) {
        return false;
    }
    switch (from) {
        case State::Booting:
            return to == State::Ready || to == State::Stopped;
        case State::Ready:
            return to == State::Degraded || to == State::Draining;
        case State::Degraded:
            return to == State::Ready || to == State::Draining;
        case State::Draining:
            return to == State::Stopped;
        case State::Stopped:
            return false;
    }
    return false;
}

State Lifecycle::get() const noexcept {
    return state_.load(std::memory_order_acquire);
}

bool Lifecycle::try_transition(State next) noexcept {
    State current = state_.load(std::memory_order_acquire);
    while (is_valid_transition(current, next)) {
        const bool swapped = state_.compare_exchange_weak(
            current, next, std::memory_order_acq_rel, std::memory_order_acquire);
        if (swapped) {
            return true;
        }
    }
    return false;
}

}  // namespace dfrog
