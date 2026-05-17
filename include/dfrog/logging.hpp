#pragma once

#include <initializer_list>
#include <string>
#include <string_view>

namespace dfrog::log {

enum class Level {
    Debug,
    Info,
    Warn,
    Error,
};

struct Attr {
    std::string_view key;
    std::string_view value;
};

void set_min_level(Level lvl) noexcept;
[[nodiscard]] Level min_level() noexcept;

void log(Level lvl, std::string_view event, std::initializer_list<Attr> attrs = {});

void debug(std::string_view event, std::initializer_list<Attr> attrs = {});
void info(std::string_view event, std::initializer_list<Attr> attrs = {});
void warn(std::string_view event, std::initializer_list<Attr> attrs = {});
void error(std::string_view event, std::initializer_list<Attr> attrs = {});

namespace detail {

// Exposed for unit tests of the JSON serialization. The first overload
// accepts an explicit timestamp so tests can pin determinism; production
// callers use the second overload which reads the wall clock.
[[nodiscard]] std::string make_log_line(
    std::string_view iso_timestamp,
    Level lvl,
    std::string_view event,
    std::initializer_list<Attr> attrs);

[[nodiscard]] std::string make_log_line(
    Level lvl,
    std::string_view event,
    std::initializer_list<Attr> attrs);

[[nodiscard]] std::string iso_timestamp_utc_now();

}  // namespace detail

}  // namespace dfrog::log
