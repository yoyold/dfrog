#include "dfrog/logging.hpp"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <mutex>

namespace dfrog::log {

namespace {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::atomic<Level> g_min_level{Level::Info};
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::mutex g_write_mu;

constexpr std::string_view level_name(Level l) noexcept {
    switch (l) {
        case Level::Debug:
            return "debug";
        case Level::Info:
            return "info";
        case Level::Warn:
            return "warn";
        case Level::Error:
            return "error";
    }
    return "info";
}

constexpr char kHexDigits[] = "0123456789abcdef";

void append_escaped(std::string& out, std::string_view s) {
    for (char c : s) {
        switch (c) {
            case '"':
                out += "\\\"";
                break;
            case '\\':
                out += "\\\\";
                break;
            case '\b':
                out += "\\b";
                break;
            case '\f':
                out += "\\f";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            default: {
                const auto byte = static_cast<unsigned char>(c);
                if (byte < 0x20) {
                    out += "\\u00";
                    out += kHexDigits[(byte >> 4U) & 0x0FU];
                    out += kHexDigits[byte & 0x0FU];
                } else {
                    out += c;
                }
            }
        }
    }
}

}  // namespace

namespace detail {

std::string iso_timestamp_utc_now() {
    using std::chrono::duration_cast;
    using std::chrono::floor;
    using std::chrono::milliseconds;
    using std::chrono::seconds;
    using std::chrono::system_clock;

    const auto now = system_clock::now();
    const auto secs = floor<seconds>(now);
    const auto ms = duration_cast<milliseconds>(now - secs).count();

    const std::time_t t = system_clock::to_time_t(secs);
    std::tm tm_buf{};
    gmtime_r(&t, &tm_buf);

    char buf[40];
    // NOLINTNEXTLINE(cert-err33-c)
    std::snprintf(
        buf,
        sizeof(buf),
        "%04d-%02d-%02dT%02d:%02d:%02d.%03lldZ",
        tm_buf.tm_year + 1900,
        tm_buf.tm_mon + 1,
        tm_buf.tm_mday,
        tm_buf.tm_hour,
        tm_buf.tm_min,
        tm_buf.tm_sec,
        static_cast<long long>(ms));
    return std::string{buf};
}

std::string make_log_line(
    std::string_view iso_timestamp,
    Level lvl,
    std::string_view event,
    std::initializer_list<Attr> attrs) {
    std::string out;
    out.reserve(128 + (32 * attrs.size()));
    out += "{\"ts\":\"";
    append_escaped(out, iso_timestamp);
    out += "\",\"lvl\":\"";
    out += level_name(lvl);
    out += "\",\"event\":\"";
    append_escaped(out, event);
    out += '"';
    if (attrs.size() > 0) {
        out += ",\"attrs\":{";
        bool first = true;
        for (const auto& a : attrs) {
            if (!first) {
                out += ',';
            }
            first = false;
            out += '"';
            append_escaped(out, a.key);
            out += "\":\"";
            append_escaped(out, a.value);
            out += '"';
        }
        out += '}';
    }
    out += '}';
    return out;
}

std::string make_log_line(
    Level lvl,
    std::string_view event,
    std::initializer_list<Attr> attrs) {
    return make_log_line(iso_timestamp_utc_now(), lvl, event, attrs);
}

}  // namespace detail

void set_min_level(Level lvl) noexcept {
    g_min_level.store(lvl, std::memory_order_release);
}

Level min_level() noexcept {
    return g_min_level.load(std::memory_order_acquire);
}

void log(Level lvl, std::string_view event, std::initializer_list<Attr> attrs) {
    if (static_cast<int>(lvl) < static_cast<int>(min_level())) {
        return;
    }
    const std::string line = detail::make_log_line(lvl, event, attrs);
    const std::lock_guard<std::mutex> lk(g_write_mu);
    // NOLINTNEXTLINE(cert-err33-c)
    std::fwrite(line.data(), 1, line.size(), stdout);
    // NOLINTNEXTLINE(cert-err33-c)
    std::fputc('\n', stdout);
}

void debug(std::string_view event, std::initializer_list<Attr> attrs) {
    log(Level::Debug, event, attrs);
}

void info(std::string_view event, std::initializer_list<Attr> attrs) {
    log(Level::Info, event, attrs);
}

void warn(std::string_view event, std::initializer_list<Attr> attrs) {
    log(Level::Warn, event, attrs);
}

void error(std::string_view event, std::initializer_list<Attr> attrs) {
    log(Level::Error, event, attrs);
}

}  // namespace dfrog::log
