//
//    コマンド解析器
//
//    Copyright (c) 2025 okawa yusuke
//


#include "command_parser.hpp"

#include <sstream>
#include <istream>

namespace choco
{

    template <class... Ts>
    struct overload : Ts...
    {
        using Ts::operator()...;
    };
    template <class... Ts>
    overload(Ts...) -> overload<Ts...>;

    /// @brief T 型の値をストリームから読み込む
    /// @param stream ストリーム
    template <typename T>
    static std::optional<T> read(std::istream& stream)
    {
        T value;
        if (stream >> value)
            return value;
        else
            return std::nullopt;
    }

    /// @brief コマンドを解析する
    /// @param string コマンド文字列
    /// @return プログラムで扱いやすい形のコマンド
    std::optional<command> parse_command(const std::string& string)
    {
        std::istringstream iss{ string };

        const auto command = read<std::string>(iss);
        if (not command)
            return std::nullopt;

        // 出現回数が多そうな順に判定する
        if (*command == "move")
        {
            const auto read_color = [&iss]() -> std::optional<color_kind>
            {
                if (const auto color_string = read<std::string>(iss))
                    return string_to_color_kind(*color_string);
                else
                    return std::nullopt;
            };

            const auto read_emit_state = [&iss]() -> std::optional<emit_state_kind>
            {
                if (const auto state_string = read<std::string>(iss))
                    return string_to_emit_state_kind(*state_string);
                else
                    return std::nullopt;
            };

            const auto color = read_color();
            if (not color)
                return std::nullopt;

            const auto position = read<vector3d>(iss);
            if (not position)
                return std::nullopt;

            const auto speed = read<f64>(iss);
            if (not speed)
                return std::nullopt;

            const auto emit_state = read_emit_state();
            if (not emit_state)
                return std::nullopt;

            return command_type::move{
                .color = *color,
                .position = *position,
                .speed = *speed,
                .emit_state = *emit_state,
            };
        }

        if (*command == "home")
        {
            return command_type::home{};
        }

        if (*command == "pause")
        {
            return command_type::pause{};
        }

        if (*command == "dump")
        {
            const auto read_dump_kind = [&iss]() -> std::optional<dump_kind>
            {
                if (const auto dump_kind_string = read<std::string>(iss))
                    return string_to_dump_kind(*dump_kind_string);
                else
                    return std::nullopt;
            };

            if (const auto dump_kind = read_dump_kind())
                return command_type::dump{ *dump_kind };
            else
                return std::nullopt;
        }

        return std::nullopt;
    }


    static std::string f64_to_string(f64 value, i32 precision = 2)
    {
        char buf[64];
        std::snprintf(buf, sizeof buf, "%.*f", (int)precision, value);
        return buf;
    }

    /// @brief コマンドを文字列に変換する デバッグ用
    /// @param command コマンド
    /// @return 文字列
    std::string command_to_string(const command& command)
    {
        auto visitor = overload{
            [](command_type::pause) -> std::string
            { return "pause"; },
            [](command_type::home) -> std::string
            { return "home"; },
            [](const command_type::move& command) -> std::string
            {
                return "move" + color_kind_to_string(command.color) + ' ' + command.position.to_string() + ' ' + f64_to_string(command.speed) + ' ' + emit_state_kind_to_string(command.emit_state);
            },
            [](command_type::dump) -> std::string
            { return "dump"; },
        };
        return std::visit(visitor, command);
    }


}    // namespace choco
