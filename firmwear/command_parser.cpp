#include <type_traits>
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

        std::string command_type;
        iss >> command_type;
        if (iss.fail())
            return std::nullopt;

        // 出現回数が多そうな順に判定する
        if (command_type == "go")
        {
            if (const auto pos = read<vector2d>(iss))
                return command_type::go{ *pos };
            else
                return std::nullopt;
        }

        if (command_type == "choco")
        {
            const auto color_str = read<std::string>(iss);
            const auto z = read<double>(iss);
            const auto inject_str = read<std::string>(iss);

            if (not color_str || not z || not inject_str)
                return std::nullopt;

            choco_color color;
            if (*color_str == "white")
                color = choco_color::white;
            else if (*color_str == "black")
                color = choco_color::black;
            else
                return std::nullopt;

            bool inject;
            if (*inject_str == "inject")
                inject = true;
            else if (*inject_str == "stop")
                inject = false;
            else
                return std::nullopt;

            return command_type::choco{
                .z = *z,
                .inject = inject,
                .color = color,
            };
        }

        if (command_type == "speed")
        {
            if (const auto speed = read<double>(iss))
                return command_type::speed{ *speed };
            else
                return std::nullopt;
        }

        if (command_type == "home")
        {
            return command_type::home{};
        }

        if (command_type == "pause")
        {
            return command_type::pause{};
        }

        if (command_type == "clear")
        {
            return command_type::clear{};
        }

        if (command_type == "dump")
        {
            const auto dump_type = read<std::string>(iss);
            if (not dump_type)
                return std::nullopt;
            
            if (*dump_type == "all")
                return command_type::dump{ .type = command_type::dump::dump_type::all };
            else if (*dump_type == "current")
                return command_type::dump{ .type = command_type::dump::dump_type::current };
            else
                return std::nullopt;
        }

        return std::nullopt;
    }


    static std::string f64_to_string_with_precision(f64 value, i32 precision = 2)
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
        return std::visit(
            [](const auto& cmd) -> std::string
            {
                using T = std::decay_t<decltype(cmd)>;
                if constexpr (std::is_same_v<T, command_type::go>)
                    return "go " + f64_to_string_with_precision(cmd.x) + " " + f64_to_string_with_precision(cmd.y);
                if constexpr (std::is_same_v<T, command_type::speed>)
                    return "speed " + f64_to_string_with_precision(cmd);
                if constexpr (std::is_same_v<T, command_type::pause>)
                    return "pause";
                if constexpr (std::is_same_v<T, command_type::home>)
                    return "home";
                if constexpr (std::is_same_v<T, command_type::choco>)
                    return "choco " + std::string(cmd.color == choco_color::white ? "white " : "black ") + f64_to_string_with_precision(cmd.z) + " " + (cmd.inject ? "inject" : "stop");
                if constexpr (std::is_same_v<T, command_type::clear>)
                    return "clear";
                if constexpr (std::is_same_v<T, command_type::dump>)
                    return "dump " + std::string(cmd.type == command_type::dump::dump_type::all ? "all" : "current");
                return "[unknown]";
            },
            command);
    }


}    // namespace choco
