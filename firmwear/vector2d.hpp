#pragma once

#include "typedef.hpp"

#include <iostream>

namespace choco
{

    struct vector2d
    {
        f64 x;
        f64 y;

        friend std::istream& operator>>(std::istream& is, vector2d& v)
        {
            return is >> v.x >> v.y;
        }
    };
    
} // namespace choco
