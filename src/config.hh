#pragma once

#include <string>

class Config
{
public:
    Config();

    const std::string &db_path() const { return db_path_; }
    const std::string &base_url() const { return base_url_; }
    int port() const { return port_; }

private:
    std::string db_path_;
    std::string base_url_;
    int port_;

    void load_from_env();
};