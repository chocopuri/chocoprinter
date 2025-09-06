#pragma once

#include <vector>
#include "../common/type.hpp"

struct TrapezoidBlock
{
    float distance;

    float speed_max;
    float speed_begin;
    float speed_end;

    float time_acc;
    float time_const;
    float time_dec;

    friend std::ostream& operator<<(std::ostream& os, const TrapezoidBlock& self);
};

/// @brief 台形制御の加速時間、等速時間、減速時間を求める
/// @param begin
/// @param end
/// @param speed_begin 0~
/// @param speed_end 0~
/// @param speed_limit 0~
/// @param acc 0~
/// @param dec 0~
/// @return
TrapezoidBlock make_trapezoid_block(float begin, float end, float speed_begin, float speed_end, float speed_limit, float acc, float dec);

struct Node
{
    Vec3 waypoint;
    float choco;
};

std::vector<Node> make_path(const std::vector<Node>& /*positions*/)
{
    return {};
}
