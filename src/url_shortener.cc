#include "url_shortener.hh"
#include "utils.hh"
#include <blake3.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <algorithm>
#include <chrono>

URLShortener::URLShortener(const Config &config)
    : db_(config.db_path()), config_(config) {}

std::string URLShortener::shorten(const std::string &long_url)
{
    std::string short_code = generate_short_code(long_url);
    db_.insert(short_code, long_url);
    return short_code;
}

std::optional<std::string> URLShortener::expand(const std::string &short_code)
{
    return db_.get(short_code);
}

std::string URLShortener::generate_short_code(const std::string &long_url)
{
    constexpr size_t code_length = 8;

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    std::string input = long_url + std::to_string(timestamp);

    std::array<uint8_t, BLAKE3_OUT_LEN> hash;
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, input.data(), input.size());
    blake3_hasher_finalize(&hasher, hash.data(), BLAKE3_OUT_LEN);

    return base62_encode(hash).substr(0, code_length);
}

std::string URLShortener::base62_encode(const std::array<uint8_t, 32> &hash)
{
    constexpr char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    constexpr size_t charset_size = sizeof(charset) - 1;

    std::string result;
    result.reserve(43);

    size_t value = 0;
    int bits = 0;

    for (uint8_t byte : hash)
    {
        value = (value << 8) | byte;
        bits += 8;

        while (bits >= 6)
        {
            bits -= 6;
            result.push_back(charset[(value >> bits) & 0x3F]);
        }
    }

    if (bits > 0)
    {
        result.push_back(charset[(value << (6 - bits)) & 0x3F]);
    }

    return result;
}