#include <sstream>

#include "command_parser.hpp"

bool operator==(const CommandHomeGantry&, const CommandHomeGantry&)
{
    return true;
}

std::ostream& operator<<(std::ostream& os, const CommandHomeGantry&)
{
    return os << "home";
}

bool operator==(const CommandMove& l, const CommandMove& r)
{
    return l.color == r.color && l.pos == r.pos && l.speed == r.speed && l.is_inject == r.is_inject;
}

std::ostream& operator<<(std::ostream& os, const CommandMove& self)
{
    return os << "move { " << self.color << " " << self.pos << " " << self.speed << " " << (self.is_inject ? "inject" : "stop") << " }";
}

bool operator==(const CommandAir& l, const CommandAir& r)
{
    return l.color == r.color && l.volume_ml == r.volume_ml;
}

std::ostream& operator<<(std::ostream& os, const CommandAir& self)
{
    return os << "air { " << self.color << " " << self.volume_ml << " }";
}


bool operator==(const CommandHomeAir& l, const CommandHomeAir& r)
{
    return true;
}

std::ostream& operator<<(std::ostream& os, const CommandHomeAir& self)
{
    return os << "homing_air";
}


std::ostream& operator<<(std::ostream& os, const Command& self)
{
    return std::visit([&os](auto&& value) -> std::ostream&
                      { return os << value; },
                      self);
}

bool operator==(const Command& l, const Command& r)
{
    if (l.index() != r.index())
        return false;

    return std::visit(
        [](auto&& left, auto&& right) -> bool
        {
            using L = std::decay_t<decltype(left)>;
            using R = std::decay_t<decltype(right)>;
            if constexpr (std::is_same_v<L, R>)
            {
                return left == right;
            }
            else
            {
                return false;
            }
        },
        l, r);
}

template <typename T>
std::optional<T> read(std::istream& is)
{
    T obj;
    is >> obj;
    return obj;
}

std::optional<Command> parse_command(const std::string& command_text)
{
    std::istringstream iss{ command_text };
    auto command_type_str = read<std::string>(iss);
    if (not command_type_str)
        return std::nullopt;

    if (*command_type_str == "move")
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

    if (*command_type_str == "home air")
    {
        return CommandHomeAir{};
    }

    if (*command_type_str == "home")
    {
        return CommandHomeGantry{};
    }

    return std::nullopt;
}


static std::vector<std::string> split_newline(const std::string& input)
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

std::optional<std::vector<Command>> parse_commands(const std::string& command_text)
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
