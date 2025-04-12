//
//    ガントリー上を移動するヘッド
//
//    Copyright (c) 2025 okawa yusuke
//


#pragma once

#include "stepper_motor.hpp"
#include "solenoid_valve.hpp"

namespace choco
{

    /// @brief ガントリー上を移動するヘッド
    class head
    {

        homing_available_motor motor_z;    // 上下移動用モータ (白チョコ、黒チョコ共通)

        solenoid_valve valve_white;    // 白チョコ用
        solenoid_valve valve_black;    // 黒チョコ用

    public:
        head(homing_available_motor&& motor_z,
             solenoid_valve&& valve_white,
             solenoid_valve&& valve_black)
            : motor_z{ std::move(motor_z) }
            , valve_white{ std::move(valve_white) }
            , valve_black{ std::move(valve_black) }
        {
        }


        /// @brief 初期化
        void init()
        {
            motor_z.init();
            valve_white.init();
            valve_black.init();
        }

        /// @brief 原点取り loop 内で呼び出す
        /// @return 原点取りが終了した場合 true 移動中は false
        /// @note true が返ってきた場合、原点取りが終了しているので、即本関数呼び出しを終了する必要がある。終了しないと再度原点取りを行うことになる。
        bool homing()
        {
            return motor_z.homing(0, 1, 500, 0.5, 0.5);
        }


        /// @brief 指定の色のチョコを射出する
        /// @param c 色
        /// @param enable true: 射出する false: 射出停止
        void inject(choco_color c, bool enable)
        {
            auto& valve = (c == choco_color::white) ? valve_white : valve_black;
            if (enable)
                valve.open();
            else
                valve.close();
        }


        /// @brief ノズルの先端を指定の位置に移動する
        /// @param c 色
        /// @param z Z座標 [mm]
        bool move_to(choco_color c, double z)
        {
            return true;
        }
    };

}    // namespace choco
