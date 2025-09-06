//
//   台形制御テスト
//

#ifndef ARDUINO

#include <iostream>
#include <cmath>
#include <cassert>
#include <iomanip>

#include "path_planner.hpp"

static bool float_eq(float lhs, float rhs)
{
    const float EPSILON = 0.001;    // 許容誤差
    return std::fabs(lhs - rhs) < EPSILON;
}

static void test_distance_consistency(const TrapezoidBlock& path)
{
    float distance_acc = (path.speed_begin + path.speed_max) * path.time_acc / 2;
    float distance_const = path.speed_max * path.time_const;
    float distance_dec = (path.speed_max + path.speed_end) * path.time_dec / 2;
    float distance = distance_acc + distance_const + distance_dec;

    std::cout << "diff : " << path.distance - distance  << std::endl;

    if (float_eq(distance, path.distance))
        return;

    std::cout << "diff : " << path.distance - distance << ", path.distance: " << path.distance << ", distance : " << distance << std::endl;
    std::cout << path << std::endl;

    assert(false);
}

int main()
{
    std::cout << std::fixed << std::setprecision(8);

    test_distance_consistency(make_trapezoid_block(0, 100, 0, 0, 10, 1, 1));
    test_distance_consistency(make_trapezoid_block(0, 1000, 0, 0, 10, 1, 1));
    test_distance_consistency(make_trapezoid_block(0, 1000, 10, 30, 10, 5, 5));
    test_distance_consistency(make_trapezoid_block(1000, 2000, 10, 30, 10, 5, 5));
    test_distance_consistency(make_trapezoid_block(913, 1668, 50, 36, 95, 4, 2));

    for (int i = 0; i < 10000; ++i)
    {
        float begin = rand() % 1000 + 1;
        float end = rand() % 1000 + 1000 + 1;
        float speed_begin = rand() % 50 + 1;
        float speed_end = rand() % 50 + 1;
        float speed_limit = rand() % 100 + 1;
        float acc = rand() % 10 + 1;
        float dec = rand() % 10 + 1;
        std::cout << "make_trapezoid_block(" << begin << ", " << end << ", " << speed_begin << ", " << speed_end << ", " << speed_limit << ", " << acc << ", " << dec << ")" << std::endl;
        test_distance_consistency(make_trapezoid_block(begin, end, speed_begin, speed_end, speed_limit, acc, dec));
    }

    std::cout << "passed" << std::endl;
}

#endif
