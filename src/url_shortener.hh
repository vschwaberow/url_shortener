#pragma once

#include "database.hh"
#include "config.hh"
#include <string>
#include <optional>
#include <array>

class URLShortener
{
public:
    explicit URLShortener(const Config &config);

    std::string shorten(const std::string &long_url);
    std::optional<std::string> expand(const std::string &short_code);

private:
    Database db_;
    Config config_;

    std::string generate_short_code(const std::string &long_url);
    std::string base62_encode(const std::array<uint8_t, 32> &hash);
};