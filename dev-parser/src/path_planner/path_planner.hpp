#pragma once

#include "common/type.hpp"

struct SinglePath
{
    float distance;

    float speed_limit;

    float time_acc;
    float time_const;
    float time_dec;
};

SinglePath make_time_block(float begin, float end, float speed_begin, float speed_end, float speed_limit, float acc_limit)
{
    
}
