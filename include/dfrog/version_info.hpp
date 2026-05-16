#pragma once

#include <string_view>

namespace dfrog {

// Human-readable one-line version banner (e.g. "dfrog 0.0.1").
[[nodiscard]] std::string_view version_banner() noexcept;

}  // namespace dfrog
