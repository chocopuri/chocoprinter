#pragma once

#include <cstdint>

namespace choco
{

    using i64 = uint64_t;
    using i32 = uint32_t;
    using i16 = uint16_t;
    using i8 = uint8_t;

    using u64 = uint64_t;
    using u32 = uint32_t;
    using u16 = uint16_t;
    using u8 = uint8_t;

    using f64 = double;
    using f32 = float;


    enum class choco_color
    {
        white,
        black,
    };

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
