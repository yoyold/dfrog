#include <gtest/gtest.h>

#include <string>

#include "dfrog/version.hpp"
#include "dfrog/version_info.hpp"

TEST(Version, BannerStartsWithProjectName) {
    const std::string banner{dfrog::version_banner()};
    EXPECT_EQ(banner.rfind("dfrog ", 0), 0u);
}

TEST(Version, BannerContainsConfiguredVersion) {
    const std::string banner{dfrog::version_banner()};
    EXPECT_NE(banner.find(DFROG_VERSION_STRING), std::string::npos);
}

TEST(Version, MajorMinorPatchNonNegative) {
    EXPECT_GE(dfrog::kVersionMajor, 0);
    EXPECT_GE(dfrog::kVersionMinor, 0);
    EXPECT_GE(dfrog::kVersionPatch, 0);
}
