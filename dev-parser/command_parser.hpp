#pragma once

#include <memory>
#include <variant>
#include <optional>
#include <sstream>

#include "type.hpp"
#include "command_receiver.hpp"

struct CommandHome
{
    friend bool operator==(const CommandHome& l, const CommandHome& r) { return true; }
};

struct CommandMove
{
    Color color;
    Vec3 pos;
    float speed;
    bool is_inject;

    friend bool operator==(const CommandMove& l, const CommandMove& r)
    {
        return l.color == r.color && l.pos == r.pos && l.speed == r.speed && l.is_inject == r.is_inject;
    }
};

using Command = std::variant<CommandHome, CommandMove>;

template <typename T>
std::optional<T> read(std::istream& is)
{
    T obj;
    is >> obj;
    return obj;
}

auto parse_command(const std::string& command_text)
    -> std::optional<Command>
{
    std::istringstream iss{ command_text };
    auto commnad_type_str = read<std::string>(iss);
    if (not commnad_type_str)
        return std::nullopt;
    
    if (*commnad_type_str == "move")
    {
        auto color_str = read<std::string>(iss);
        auto pos = read<Vec3>(iss);
        auto speed = read<float>(iss);
        auto inject_or_stop_str = read<std::string>(iss);

        if (not color_str || not pos || not speed || not inject_or_stop_str)
            return std::nullopt;
                
        Color color;
        if (*color_str == "black")
            color = Color::black;
        else if (*color_str == "white")
            color = Color::white;
        else
            return std::nullopt;
        
        bool is_inject;
        if (*inject_or_stop_str == "inject")
            is_inject = true;
        else if (*inject_or_stop_str == "stop")
            is_inject = false;
        else
            return std::nullopt;

        return CommandMove{ 
            .color = color, 
            .pos = *pos,
            .speed = *speed, 
            .is_inject = is_inject,
        };
    }
    
    if (*commnad_type_str == "home")
    {
        return CommandHome{};
    }

    return std::nullopt;    
}
