//
//    ステッピングモーター
//
//    Copyright (c) 2025 okawa yusuke
//


#pragma once

#include "type.hpp"

namespace choco
{

    /// @brief ステッピングモータ
    /// @note モータードライバと通信する
    class stepper_motor
    {
        i8 pin_step;
        i8 pin_dir;
        i8 pin_enable;

        direction dir;    // 正転か逆転か

        i64 current_pos = 0;

    public:
        stepper_motor(i8 pin_step, i8 pin_dir, i8 pin_enable, direction dir = direction::forward)
            : pin_step(pin_step)
            , pin_dir(pin_dir)
            , pin_enable(pin_enable)
            , dir(dir)
        {
        }

        /// @brief 初期化
        void init()
        {
        }

        /// @brief 現在位置を原点にする
        void set_home()
        {
        }

        /// @brief 任意の角度に移動する
        /// @param angle 目標角度 [rad]
        /// @param speed 回転角速度 [rad/s]
        /// @return true: 移動完了 false: 移動中
        bool move_to(f64 angle, f64 speed = 0.0)
        {
            return true;
        }

        /// @brief 任意の角速度で回す
        /// @param speed 回転角速度 [rad/s]
        void rotate(i64 speed)
        {
        }

        /// @brief デバッグ出力
        void dump() const
        {
        }
    };


    /// @brief リミットスイッチ
    class limit_switch
    {
        i8 pin;

    public:
        limit_switch(i8 pin)
            : pin{ pin }
        {
        }

        void init()
        {
            pinMode(pin, INPUT_PULLUP);
        }

        bool is_pressed() const
        {
            return digitalRead(pin) == LOW;
        }
    };


    /// @brief 原点取り可能なモータ
    /// @note リミットスイッチとモーターの組み合わせ
    class homing_available_motor
    {
        stepper_motor motor;
        limit_switch limit_sw;

        direction home_dir;    // 原点の方向

        i32 homing_sequence = 0;    // 原点取りの状態を保持する変数
        u32 last_retreat_ms = 0;

    public:
        homing_available_motor(stepper_motor&& motor, limit_switch&& limit_sw, direction home_dir = direction::forward)
            : motor{ std::move(motor) }
            , limit_sw{ std::move(limit_sw) }
            , home_dir{ home_dir }
        {
        }

        void init()
        {
            motor.init();
            limit_sw.init();
        }


        /// @brief 原点取り loop 内で呼び出す
        /// @param limit_switch_angle リミットスイッチと接触した位置の角度 [rad] (リミットスイッチの位置が原点とは限らないので)
        /// @param first_approach_speed 一度目に原点に近づく時の移動速度 [rad/s]
        /// @param retreat_time_ms 原点到達後、原点から離れる時間 [ms]
        /// @param retreat_speed 原点から離れる速度 [rad/s]
        /// @param second_approach_speed 二度目に原点に近づく時の移動速度 [rad/s]
        /// @return 原点取りが終了した場合 true 移動中は false
        bool homing(
            f64 limit_switch_angle,
            f64 first_approach_speed,
            u32 retreat_time_ms,
            f64 retreat_speed,
            f64 second_approach_speed)
        {
            switch (homing_sequence)
            {
            case 0:
                motor.rotate(first_approach_speed * direction_to_sign(home_dir));    // 原点に向かって移動
                if (limit_sw.is_pressed())
                {
                    homing_sequence++;    // 原点に到達したら次へ
                }
                break;

            case 1:
                last_retreat_ms = millis();
                homing_sequence++;

            case 2:
                motor.rotate(first_approach_speed * -direction_to_sign(home_dir));    // 原点から離れる方向に移動
                if (millis() - last_retreat_ms > retreat_time_ms)                     // 指定時間経過後、次へ
                {
                    homing_sequence++;
                }

            case 3:
                motor.rotate(second_approach_speed * direction_to_sign(home_dir));    // 原点に再度移動
                if (limit_sw.is_pressed())
                {
                    motor.set_home();
                    homing_sequence = 0;
                    return true;    // 原点取り終了
                }
                break;
            }
            return false;    // 原点取り中
        }


        /// @brief 任意の角度に移動する
        /// @param angle 目標角度 [rad]
        /// @param speed 回転角速度 [rad/s]
        /// @return true: 移動完了 false: 移動中
        bool move_to(f64 angle, f64 speed)
        {
            return motor.move_to(angle, speed);
        }
    };
}    // namespace choco
