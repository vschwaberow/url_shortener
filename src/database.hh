#pragma once

#include <string>
#include <optional>
#include <sqlite3.h>
#include <memory>

class Database
{
public:
    explicit Database(const std::string &db_path);
    ~Database();

    void insert(const std::string &short_code, const std::string &long_url);
    std::optional<std::string> get(const std::string &short_code);
    bool exists(const std::string &short_code);

private:
    std::unique_ptr<sqlite3, decltype(&sqlite3_close)> db_;

    void execute(const std::string &sql);
};
