#include "dfrog/version_info.hpp"

#include "dfrog/version.hpp"

namespace dfrog {

std::string_view version_banner() noexcept {
    return "dfrog " DFROG_VERSION_STRING;
}

}  // namespace dfrog
