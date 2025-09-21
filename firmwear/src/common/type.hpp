#pragma once

#include <istream>
#include <cmath>

enum class Color
{
    black,
    white,
};

inline std::ostream& operator<<(std::ostream& os, Color self)
{
    return os << "{ " << (self == Color::black ? "black" : "white") << " }"; 
}

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

    friend std::ostream& operator<<(std::ostream& os, const Vec3& self)
    {
        return os << "{ " << self.x << " " << self.y << " " << self.z << " }"; 
    }

    float length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }
};

enum class Direction
{
    forward,
    backward,
};

inline int direction_to_sign(Direction dir)
{
    switch (dir)
    {
    case Direction::forward:
        return 1;
    case Direction::backward:
        return -1;
    default:
        return 0; // unreachable
    }
}
