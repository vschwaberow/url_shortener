
#pragma once

#include <string>
#include <fmt/format.h>

inline std::string escape_string(const std::string &str)
{
    std::string result;
    result.reserve(str.size());
    for (char c : str)
    {
        if (c == '\'')
        {
            result += "''";
        }
        else
        {
            result += c;
        }
    }
    return result;
}
