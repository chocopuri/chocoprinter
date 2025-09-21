#pragma once

#include "limit_switch.hpp"
#include "stepper_motor.hpp"

struct HomingConfig
{
    // リミットスイッチへ近づく時
    float approach_switch_speed;           // [rev/s] sign
    float approach_switch_acceleration;    // [rev/s^2] abs

    // スイッチから離れる時
    float leave_switch_distance;        // [rev] sign
    float leave_switch_speed;           // [rev/s] abs
    float leave_switch_acceleration;    // [rev/s^2] abs
};

/// @brief 原点どり機能を有し、他のモータと動作タイミングを同期できるステッピングモータ
class StepperMotorHomeableSynchronizable
{
    StepperSyncGroup& sync_group;
    SteppingMotor driver;
    LimitSwitch limit_switch;

    int homing_sequence = 0;    // 0: 原点どりがまだ -1: 原点どり完了 1~原点どり中

    HomingConfig config;

public:
    /// @brief
    /// @param sync_group
    /// @param driver
    /// @param limit_switch
    StepperMotorHomeableSynchronizable(StepperSyncGroup& sync_group,
                                       SteppingMotor&& driver,
                                       LimitSwitch&& limit_switch,
                                       HomingConfig&& config)
        : sync_group{ sync_group }
        , driver{ std::move(driver) }
        , limit_switch{ std::move(limit_switch) }
        , config{ config }
    {
    }

    void begin()
    {
        driver.attach_sync_group(sync_group);
        limit_switch.begin();
    }

    // 原点どり終了で true を返す
    bool homing_update()
    {
        switch (homing_sequence)
        {
        case -1:    // すでに原点どり済  //だが再度原点どりするケース
            return true;
            
        case 0:    // 起動時
            homing_sequence = 1;
            driver.set_acceleration(config.approach_switch_acceleration);
            driver.set_max_speed(abs(config.approach_switch_speed));
            driver.set_speed(config.approach_switch_speed);    // 進行

        // リミットスイッチに向かって進行
        case 1:
            driver.run_speed();
            if (limit_switch.is_pressed())
            {
                ++homing_sequence;
                driver.set_acceleration(config.leave_switch_acceleration);
                driver.set_max_speed(config.leave_switch_speed);
                driver.set_relative_target_position(config.leave_switch_distance);    // 少し離れる
            }
            else
            {
                return false;
            }

        // リミットスイッチから少し離れる
        case 2:
            if (driver.run_to_position())
            {
                return false;    // 移動中
            }
            else
            {
                driver.set_current_position(0);    // 離れた点を原点とする
                homing_sequence = -1;
                return true;
            }

        default:
            return false;    // unreachable
        }
    }

    /// @brief 目標値を設定 実際の動作はsync_group参照先のオブジェクト経由で行う
    /// @param absolute_position_rev 何回転するか
    void set_target_position(float absolute_position_rev, float acceleration = -1, float max_speed = -1)
    {
        if (homing_sequence != -1)
            return;    // 原点どり中、または原点どり未実施
        
        if (acceleration > 0)
            driver.set_acceleration(acceleration);
        else
            driver.set_acceleration(config.leave_switch_acceleration);

        if (max_speed > 0)
            driver.set_max_speed(max_speed);
        else
            driver.set_max_speed(config.leave_switch_speed);
            
        driver.set_absolute_target_position_sync(absolute_position_rev);
    }

};