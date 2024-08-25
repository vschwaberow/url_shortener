#include "config.hh"
#include <cstdlib>
#include <stdexcept>

Config::Config()
{
    load_from_env();
}

void Config::load_from_env()
{
    const char *db_path = std::getenv("URL_SHORTENER_DB_PATH");
    const char *base_url = std::getenv("URL_SHORTENER_BASE_URL");
    const char *port_str = std::getenv("URL_SHORTENER_PORT");

    if (!db_path || !base_url || !port_str)
    {
        throw std::runtime_error("Missing required environment variables");
    }

    db_path_ = db_path;
    base_url_ = base_url;
    port_ = std::stoi(port_str);
}