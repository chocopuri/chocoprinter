#pragma once


#include <AccelStepper.h>
#include <MultiStepper.h>

#include "limit_switch.hpp"

/// @brief 原点どり機能を有し、他のモータと動作タイミングを同期できるステッピングモータ
class StepperMotorHomeableSynchronizable
{
    MultiStepper& sync_group;
    AccelStepper driver;
    LimitSwitch limit_switch;

    int homing_sequence = 0;    // 0: 原点どりがまだ -1: 原点どり完了 1~原点どり中
    int pulse_per_rev;

public:

    /// @brief 
    /// @param sync_group 
    /// @param driver 
    /// @param limit_switch 
    /// @param pulse_per_rev 一回転するのに必要なパルス数
    StepperMotorHomeableSynchronizable(MultiStepper& sync_group, AccelStepper&& driver, LimitSwitch&& limit_switch, int pulse_per_rev)
        : sync_group{ sync_group }
        , driver{ std::move(driver) }
        , limit_switch{ std::move(limit_switch) }
        , pulse_per_rev{ pulse_per_rev }
    {
        // driver.setAcceleration(3000);
        // driver.setMaxSpeed(10000);
    }

    void begin()
    {
        sync_group.addStepper(driver);
        limit_switch.begin();
    }

    // 原点どり終了で true を返す
    bool homing_update()
    {
        // Serial.println(homing_sequence);
        switch (homing_sequence)
        {
        case -1:    // すでに原点どり済だが再度原点どりするケース
        case 0:     // 起動時
            homing_sequence = 1;

        // リミットスイッチに向かって進行
        case 1:
            driver.setSpeed(-5000);
            driver.runSpeed();
            if (limit_switch.is_pressed())
            {
                ++homing_sequence;
            }
            else
            {
                return false;
            }

        // リミットスイッチから少し離れる
        case 2:
            driver.setAcceleration(10000);
            driver.setMaxSpeed(5000);
            driver.move(3000);
            if (driver.run())
            {
                return false;  // 移動中
            }
            else
            {
                driver.setCurrentPosition(0);    // 離れた点を原点とする
                homing_sequence = -1;
                return true;
            }

        default:
            return false;    // unreachable
        }
    }

    /// @brief 目標値を設定 実際の動作はsync_group参照先のオブジェクト経由で行う
    /// @param absolute_position_rev 何回転するか
    void set_target_position_rev(float absolute_position_rev)
    {
        if (homing_sequence == -1)
        {
            driver.moveTo(absolute_position_rev * pulse_per_rev);
        }
    }
};