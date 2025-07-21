#pragma once

#include <istream>

enum class Color
{
    black,
    white,
};

struct Vec3
{
    float x, y, z;

    friend std::istream& operator>>(std::istream& is, Vec3& self)
    {
        return is >> self.x >> self.y >> self.z;
    }

    friend bool operator==(const Vec3& l, const Vec3& r)
    {
        return l.x == r.x && l.y == r.y && l.z == r.z;
    }
};
