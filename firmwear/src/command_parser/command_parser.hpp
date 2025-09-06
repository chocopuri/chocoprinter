#pragma once

#include <variant>
#include <optional>
#include <iostream>
#include <vector>

#include "../common/type.hpp"

struct CommandHome
{
    friend bool operator==(const CommandHome& l, const CommandHome& r);
    friend std::ostream& operator<<(std::ostream& os, const CommandHome& self);
};


struct CommandMove
{
    Color color;
    Vec3 pos;
    float speed;
    bool is_inject;

    friend bool operator==(const CommandMove& l, const CommandMove& r);
    friend std::ostream& operator<<(std::ostream& os, const CommandMove& self);
};


using Command = std::variant<CommandHome, CommandMove>;
std::ostream& operator<<(std::ostream& os, const Command& self);

std::optional<Command> parse_command(const std::string& command_text);

inline std::vector<std::string> split_newline(const std::string& input)
{
    std::vector<std::string> dest;
    size_t start = 0, pos;
    while ((pos = input.find('\n', start)) != std::string::npos)
    {
        dest.push_back(input.substr(start, pos - start));
        start = pos + 1;
    }
    dest.push_back(input.substr(start));
    return dest;
}

inline std::optional<std::vector<Command>> parse_commands(const std::string& command_text)
{
    std::vector<Command> dest{};

    std::vector<std::string> splitted_commands = split_newline(command_text);
    
    for (auto&& command : splitted_commands)
    {
        if (command.size() == 0)
            continue;
            
        if (const auto parsed_command = parse_command(command))
        {
            dest.push_back(*parsed_command);
        }
        else
        {
            return std::nullopt;
        }
    }

    return dest;
}
