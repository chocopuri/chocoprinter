#pragma once

#include <variant>
#include <optional>
#include <iostream>

#include "common/type.hpp"

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
