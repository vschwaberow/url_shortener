#include "database.hh"
#include "utils.hh"
#include <stdexcept>

Database::Database(const std::string &db_path)
    : db_(nullptr, sqlite3_close)
{
    sqlite3 *raw_db;
    if (sqlite3_open(db_path.c_str(), &raw_db) != SQLITE_OK)
    {
        throw std::runtime_error("Failed to open database");
    }
    db_.reset(raw_db);

    execute("CREATE TABLE IF NOT EXISTS urls (short_code TEXT PRIMARY KEY, long_url TEXT NOT NULL)");
}

Database::~Database() = default;

void Database::insert(const std::string &short_code, const std::string &long_url)
{
    std::string sql = fmt::format(
        "INSERT INTO urls (short_code, long_url) VALUES ('{}', '{}')",
        escape_string(short_code), escape_string(long_url));
    execute(sql);
}

std::optional<std::string> Database::get(const std::string &short_code)
{
    std::string sql = fmt::format(
        "SELECT long_url FROM urls WHERE short_code = '{}'",
        escape_string(short_code));

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_.get(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error("Failed to prepare statement");
    }

    std::optional<std::string> result;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        result = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);
    return result;
}

bool Database::exists(const std::string &short_code)
{
    std::string sql = fmt::format(
        "SELECT 1 FROM urls WHERE short_code = '{}'",
        escape_string(short_code));

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_.get(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error("Failed to prepare statement");
    }

    bool result = sqlite3_step(stmt) == SQLITE_ROW;
    sqlite3_finalize(stmt);
    return result;
}

void Database::execute(const std::string &sql)
{
    char *error_message = nullptr;
    if (sqlite3_exec(db_.get(), sql.c_str(), nullptr, nullptr, &error_message) != SQLITE_OK)
    {
        std::string error(error_message);
        sqlite3_free(error_message);
        throw std::runtime_error("SQL error: " + error);
    }
}