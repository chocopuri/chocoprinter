#pragma once

#include <variant>
#include <optional>
#include <iostream>
#include <vector>

#include "../common/type.hpp"

struct CommandHomeGantry
{
    friend bool operator==(const CommandHomeGantry& l, const CommandHomeGantry& r);
    friend std::ostream& operator<<(std::ostream& os, const CommandHomeGantry& self);
};

// エアーだけホーミング
struct CommandHomeAir {
    friend bool operator==(const CommandHomeAir& l, const CommandHomeAir& r);
    friend std::ostream& operator<<(std::ostream& os, const CommandHomeAir& self);
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

struct CommandAir
{
    Color color;
    float volume_ml;

    friend bool operator==(const CommandAir& l, const CommandAir& r);
    friend std::ostream& operator<<(std::ostream& os, const CommandAir& self);
};


using Command = std::variant<CommandHomeGantry, CommandHomeAir, CommandMove, CommandAir>;
std::ostream& operator<<(std::ostream& os, const Command& self);
bool operator==(const Command& l, const Command& r);

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
