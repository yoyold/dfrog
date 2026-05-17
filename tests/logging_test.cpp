#include <gtest/gtest.h>

#include <regex>
#include <string>

#include "dfrog/logging.hpp"

namespace dfrog::log {

TEST(MakeLogLine, MinimalNoAttrs) {
    const std::string line = detail::make_log_line(
        "2026-05-16T18:30:00.123Z",
        Level::Info,
        "daemon.start",
        {});
    EXPECT_EQ(
        line,
        R"({"ts":"2026-05-16T18:30:00.123Z","lvl":"info","event":"daemon.start"})");
}

TEST(MakeLogLine, IncludesAttrsWhenPresent) {
    const std::string line = detail::make_log_line(
        "2026-05-16T18:30:00.123Z",
        Level::Warn,
        "config.reload",
        {{"path", "/etc/dfrog/config.yaml"}});
    EXPECT_EQ(
        line,
        R"({"ts":"2026-05-16T18:30:00.123Z","lvl":"warn",)"
        R"("event":"config.reload","attrs":{"path":"/etc/dfrog/config.yaml"}})");
}

TEST(MakeLogLine, MultipleAttrsCommaSeparated) {
    const std::string line = detail::make_log_line(
        "2026-05-16T18:30:00.123Z",
        Level::Error,
        "check.failed",
        {{"check", "cpu"}, {"severity", "critical"}, {"value", "99.8"}});
    EXPECT_EQ(
        line,
        R"({"ts":"2026-05-16T18:30:00.123Z","lvl":"error","event":"check.failed",)"
        R"("attrs":{"check":"cpu","severity":"critical","value":"99.8"}})");
}

TEST(MakeLogLine, EscapesQuotesAndBackslashes) {
    const std::string line = detail::make_log_line(
        "2026-05-16T18:30:00.123Z",
        Level::Info,
        R"(a"b\c)",
        {{"k", R"("v")"}});
    EXPECT_NE(line.find(R"("event":"a\"b\\c")"), std::string::npos) << "actual: " << line;
    EXPECT_NE(line.find(R"("k":"\"v\"")"), std::string::npos) << "actual: " << line;
}

TEST(MakeLogLine, EscapesControlCharacters) {
    const std::string event = std::string{"a\nb\tc"};
    const std::string val = std::string{"x\rz"};
    const std::string line = detail::make_log_line(
        "2026-05-16T18:30:00.123Z",
        Level::Info,
        event,
        {{"k", val}});
    EXPECT_NE(line.find(R"("event":"a\nb\tc")"), std::string::npos) << "actual: " << line;
    EXPECT_NE(line.find(R"("k":"x\rz")"), std::string::npos) << "actual: " << line;
}

TEST(MakeLogLine, EscapesLowControlAsUnicodeEscape) {
    std::string event;
    event.push_back('a');
    event.push_back('\x01');
    event.push_back('b');
    const std::string line = detail::make_log_line(
        "2026-05-16T18:30:00.123Z",
        Level::Info,
        event,
        {});

    std::string expected;
    expected.append("\"event\":\"a");
    expected.push_back('\\');
    expected.append("u0001b\"");
    EXPECT_NE(line.find(expected), std::string::npos) << "actual: " << line;
}

TEST(IsoTimestampUtcNow, MatchesIsoFormat) {
    const std::string ts = detail::iso_timestamp_utc_now();
    const std::regex pattern{R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{3}Z$)"};
    EXPECT_TRUE(std::regex_match(ts, pattern)) << "timestamp was: " << ts;
}

TEST(MinLevel, SetAndGetRoundTrip) {
    const Level original = min_level();
    set_min_level(Level::Debug);
    EXPECT_EQ(min_level(), Level::Debug);
    set_min_level(Level::Error);
    EXPECT_EQ(min_level(), Level::Error);
    set_min_level(original);
}

}  // namespace dfrog::log
