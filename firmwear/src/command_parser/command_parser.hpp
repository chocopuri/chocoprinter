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
    float speed; // [mL/s]
    
    friend bool operator==(const CommandAir& l, const CommandAir& r);
    friend std::ostream& operator<<(std::ostream& os, const CommandAir& self);
};


using Command = std::variant<CommandHomeGantry, CommandHomeAir, CommandMove, CommandAir>;
std::ostream& operator<<(std::ostream& os, const Command& self);
bool operator==(const Command& l, const Command& r);

/// @brief コマンドを一列のみパースする
/// @param command_text
/// @return パース後のコマンド列、パースに失敗した場合は std::nullopt
std::optional<Command> parse_command(const std::string& command_text);

/// @brief 複数行のコマンドをパースする
/// @param command_text
/// @return パース後のコマンド列、パースに失敗した場合は std::nullopt
std::optional<std::vector<Command>> parse_commands(const std::string& command_text);
