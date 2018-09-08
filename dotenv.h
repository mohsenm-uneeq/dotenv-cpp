#pragma once

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>

class dotenv
{
public:
    const static unsigned char Overwrite = 1 << 0;

    static const int OptionsDefault = Overwrite;
    static const int OptionsNone = 0;

    static void init(const char* filename = ".env");
    static void init(int flags, const char* filename = ".env");

private:
    static void do_init(int flags, const char* filename);
    static std::string strip_quotes(const std::string& str);
};

void dotenv::init(const char* filename)
{
    dotenv::do_init(OptionsDefault, filename);
}

void dotenv::init(int flags, const char* filename)
{
    dotenv::do_init(flags, filename);
}

void dotenv::do_init(int flags, const char* filename)
{
    std::ifstream file;
    std::string line;

    const std::regex pattern("[a-zA-Z_][a-zA-Z0-9_]*=.*");

    file.open(filename);

    if (file)
    {
        unsigned int i = 1;

        while (getline(file, line))
        {
            if (!std::regex_match(line, pattern)) {
                std::cout << "dotenv: Ignoring ill-formed assignment on line "
                          << i << ": '"
                          << line << "'" << std::endl;
            } else {
                const auto pos = line.find("=");
                const auto name = line.substr(0, pos);
                const auto val = strip_quotes(line.substr(pos + 1));
                setenv(name.c_str(), val.c_str(), flags & dotenv::Overwrite);
            }

            ++i;
        }
    }
}

std::string dotenv::strip_quotes(const std::string& str)
{
    const std::size_t len = str.length();

    if (len < 2)
        return str;

    const char first = str[0];
    const char last = str[len - 1];

    if (first == last && ('"' == first || '\'' == first))
        return str.substr(1, len - 2);

    return str;
}
