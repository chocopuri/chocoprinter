//
//    型定義
//
//    Copyright (c) 2025 okawa yusuke
//


#pragma once

#include <cstdint>
#include <iostream>
#include <optional>

namespace choco
{

    using i64 = int64_t;
    using i32 = int32_t;
    using i16 = int16_t;
    using i8 = int8_t;

    using u64 = uint64_t;
    using u32 = uint32_t;
    using u16 = uint16_t;
    using u8 = uint8_t;

    using f64 = double;
    using f32 = float;

    struct vector2d
    {
        f64 x;
        f64 y;

        friend std::istream& operator>>(std::istream& is, vector2d& v)
        {
            return is >> v.x >> v.y;
        }

        std::string to_string() const
        {
            return "{ " + std::to_string(x) + ' ' + std::to_string(y) + " }";
        }
    };

    struct vector3d
    {
        f64 x;
        f64 y;
        f64 z;

        friend std::istream& operator>>(std::istream& is, vector3d& v)
        {
            return is >> v.x >> v.y >> v.z;
        }

        std::string to_string() const
        {
            return "{ " + std::to_string(x) + ' ' + std::to_string(y) + ' ' + std::to_string(z) + " }";
        }
    };


    enum class color_kind
    {
        white,
        black,
    };

    /// @brief std::string から構築
    /// @param color_string
    /// @return
    inline std::optional<color_kind> string_to_color_kind(const std::string& color_string)
    {
        if (color_string == "white")
            return color_kind::white;
        if (color_string == "black")
            return color_kind::black;
        return std::nullopt;
    }

    inline std::string color_kind_to_string(color_kind color)
    {
        switch (color)
        {
        case color_kind::white: return "white";
        case color_kind::black: return "black";
        default: return "";
        }
    }


    /// @brief チョコを射出するか停止するか
    enum class emit_state_kind
    {
        emit,
        stop,
    };

    /// @brief std::string から構築
    /// @param state_string
    /// @return
    inline std::optional<emit_state_kind> string_to_emit_state_kind(const std::string& state_string)
    {
        if (state_string == "emit")
            return emit_state_kind::emit;
        if (state_string == "stop")
            return emit_state_kind::stop;
        return std::nullopt;
    }

    inline std::string emit_state_kind_to_string(emit_state_kind emit_state)
    {
        switch (emit_state)
        {
        case emit_state_kind::emit: return "emit";
        case emit_state_kind::stop: return "stop";
        default: return "";
        }
    }


    /// @brief デバッグ出力の形式
    enum class dump_kind
    {
        all,
        current,
    };

    /// @brief std::string から構築
    /// @param state_string
    /// @return
    inline std::optional<dump_kind> string_to_dump_kind(const std::string& dump_kind_string)
    {
        if (dump_kind_string == "all")
            return dump_kind::all;
        if (dump_kind_string == "current")
            return dump_kind::current;
        return std::nullopt;
    }

    enum class direction
    {
        forward,
        backward,
    };

    inline direction reverse_direction(direction dir)
    {
        return (dir == direction::forward) ? direction::backward : direction::forward;
    }

    inline i32 direction_to_sign(direction dir)
    {
        return (dir == direction::forward) ? 1 : -1;
    }

}    // namespace choco
