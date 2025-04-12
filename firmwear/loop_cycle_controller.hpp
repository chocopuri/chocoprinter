#pragma once

#include "typedef.hpp"

namespace choco
{

    class loop_cycle_controller
    {
        u64 last_update_ms = 0;    // 最後にループした時間 [ms]
        u64 loop_time_ms = 10;     // ループ周期 [ms]

    public:
        loop_cycle_controller(u64 loop_time_ms)
            : loop_time_ms{ loop_time_ms }
        {
        }

        void update()
        {
            while (millis() - last_update_ms > loop_time_ms)
                ;
            last_update_ms += loop_time_ms;
        }
    };
}    // namespace choco
