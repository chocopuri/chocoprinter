//
//    XY軸移動用ガントリー
//
//    Copyright (c) 2025 okawa yusuke
//


#pragma once

#include "stepper_motor.hpp"

namespace choco
{

    /// @brief XY軸移動用ガントリー
    class gantry
    {

        homing_available_motor motor_x;
        homing_available_motor motor_y;

    public:
        gantry(homing_available_motor&& motor_x,
               homing_available_motor&& motor_y)
            : motor_x{ std::move(motor_x) }
            , motor_y{ std::move(motor_y) }
        {
        }


        /// @brief 初期化
        void init()
        {
            motor_x.init();
            motor_y.init();
        }

        /// @brief 原点取り loop 内で呼び出す
        /// @return 原点取りが終了した場合 true 移動中は false
        bool homing()
        {
            return motor_x.homing(0, 1, 500, 0.5, 0.5) &&
                   motor_y.homing(0, 1, 500, 0.5, 0.5);
        }

        /// @brief 目標位置へ移動
        /// @param xy 目標位置 [mm]
        /// @param speed 移動速度 [mm/s]
        /// @return true: 移動完了 false: 移動中
        bool move_to(const vector2d& xy, double speed)
        {
            // 線形補完
            auto x = xy.x;
            auto y = xy.y;

            auto x_speed = speed * (x / (x + y));
            auto y_speed = speed * (y / (x + y));

            return motor_x.move_to(x, x_speed) &&
                   motor_y.move_to(y, y_speed);
        }
    };
}    // namespace choco
