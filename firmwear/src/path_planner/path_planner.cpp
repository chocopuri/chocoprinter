#include "path_planner.hpp"

#include <cmath>

std::ostream& operator<<(std::ostream& os, const TrapezoidBlock& self)
{
    return os
            << "{ dist: " << self.distance
            << ", speed { max: " << self.speed_max
            << ", begin: " << self.speed_begin
            << ", end: " << self.speed_end
            << " }, time { acc: " << self.time_acc
            << ", const: " << self.time_const
            << ", dec: " << self.time_dec
            << " } }";
}

/// @brief 台形制御の加速時間、等速時間、減速時間を求める
/// @param begin
/// @param end
/// @param speed_begin 0~
/// @param speed_end 0~
/// @param speed_limit
/// @param acc 0~
/// @param dec 0~
/// @return
TrapezoidBlock make_trapezoid_block(float begin, float end, float speed_begin, float speed_end, float speed_limit, float acc, float dec)
{
    const float distance = end - begin;

    // 速度の変化形状が台形状になる場合と三角形状になる場合が考えられる。そこでひとまず台形になると仮定し時間を計算する。
    // 三角形状になる場合、求めた時間から移動距離を逆算すると期待する移動距離より大きくなるためそこで分岐を行う。

    // 加減速時間 (台形状と仮定)
    const float trap_time_acc = (speed_limit - speed_begin) / acc;
    const float trap_time_dec = (speed_limit - speed_end) / dec;

    // 加減速期間を基に移動距離を逆算 (台形状と仮定)
    const float trap_dist_acc = (speed_begin + speed_limit) * trap_time_acc / 2;
    const float trap_dist_dec = (speed_end + speed_limit) * trap_time_dec / 2;
    const float trap_dist = trap_dist_acc + trap_dist_dec;  // 三角状のときはこの距離が distance より大きくなる

    if (trap_dist < distance)
    {
        // 台形状
        const float dist_const = distance - trap_dist;
        const float time_const = dist_const / speed_limit;

        return {
            .distance = distance,

            .speed_max = speed_limit,
            .speed_begin = speed_begin,
            .speed_end = speed_end,

            .time_acc = trap_time_acc,
            .time_const = time_const,
            .time_dec = trap_time_dec,
        };
    }
    else
    {
        // 三角状 台形と仮定し時間を求めているためすべて再計算
        // 最高速度を求める式は次の連立方程式を解くことで求められる。変数 speed_limit を追加して立式した

        // | time_acc = (speed_max - speed_begin) / acc
        // { time_dec = (speed_max - speed_end) / dec
        // | distance = {(speed_begin + speed_limit) / 2 * time_acc} + {(speed_end + speed_limit) / 2 * time_dec}

        const float triangle_speed_max = std::sqrt((2 * distance * acc * dec + speed_begin * speed_begin * dec + speed_end * speed_end * acc) / (acc + dec));

        const float triangle_time_acc = (triangle_speed_max - speed_begin) / acc;
        const float triangle_time_const = 0;
        const float triangle_time_dec = (triangle_speed_max - speed_end) / dec;

        return {
            .distance = distance,

            .speed_max = triangle_speed_max,
            .speed_begin = speed_begin,
            .speed_end = speed_end,

            .time_acc = triangle_time_acc,
            .time_const = triangle_time_const,
            .time_dec = triangle_time_dec,
        };
    }
}
