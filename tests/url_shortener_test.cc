#include "../src/url_shortener.hh"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("URLShortener shorten and expand", "[url_shortener]")
{
    Config config;
    URLShortener shortener(config);

    SECTION("Shorten and expand URL")
    {
        std::string long_url = "https://example.com";
        std::string short_code = shortener.shorten(long_url);

        REQUIRE(short_code.length() == 8);

        auto expanded_url = shortener.expand(short_code);
        REQUIRE(expanded_url.has_value());
        REQUIRE(*expanded_url == long_url);
    }

    SECTION("Expand non-existent URL")
    {
        auto expanded_url = shortener.expand("nonexistent");
        REQUIRE_FALSE(expanded_url.has_value());
    }
}