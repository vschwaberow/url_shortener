#include "../src/database.hh"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Database operations", "[database]")
{
    Database db(":memory:");

    SECTION("Insert and retrieve URL")
    {
        std::string short_code = "abc123";
        std::string long_url = "https://example.com";

        db.insert(short_code, long_url);

        auto retrieved_url = db.get(short_code);
        REQUIRE(retrieved_url.has_value());
        REQUIRE(*retrieved_url == long_url);
    }

    SECTION("Check URL existence")
    {
        std::string short_code = "def456";
        std::string long_url = "https://example.org";

        REQUIRE_FALSE(db.exists(short_code));

        db.insert(short_code, long_url);

        REQUIRE(db.exists(short_code));
    }

    SECTION("Retrieve non-existent URL")
    {
        auto retrieved_url = db.get("nonexistent");
        REQUIRE_FALSE(retrieved_url.has_value());
    }
}