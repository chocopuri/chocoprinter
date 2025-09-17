#pragma once


#include <AccelStepper.h>
#include <MultiStepper.h>

#include "limit_switch.hpp"

#include "../common/type.hpp"

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

class SteppingMotor
{
    AccelStepper driver;
    Direction direction;
    int pulse_per_rev;

public:
    /// @brief 
    /// @param driver 
    /// @param direction 
    /// @param pulse_per_rev 一回転あたり何パルス必要か
    SteppingMotor(AccelStepper&& driver, Direction direction, int pulse_per_rev)
        : driver{ std::move(driver) }
        , direction{ direction }
        , pulse_per_rev{ pulse_per_rev }
    {
    }
    
    // 上限設定系
    void set_acceleration(float acceleration_rev)
    {
        driver.setAcceleration(acceleration_rev * pulse_per_rev);
    }
    void set_max_speed(float max_speed_rev)
    {
        driver.setMaxSpeed(max_speed_rev * pulse_per_rev);
    }

    // 位置制御系
    void set_current_position(float current_position_rev)
    {
        driver.setCurrentPosition(current_position_rev * pulse_per_rev * direction_to_sign(direction));
    }
    void set_relative_target_position(float relative_position_rev)
    {
        driver.move(relative_position_rev * pulse_per_rev * direction_to_sign(direction));
    }
    void set_absolute_target_position(float absolute_position_rev)
    {
        driver.moveTo(absolute_position_rev * pulse_per_rev * direction_to_sign(direction));
    }
    bool run_to_position()
    {
        return driver.run();
    }

    // 速度制御系
    void set_speed(float speed_rev)
    {
        driver.setSpeed(speed_rev * pulse_per_rev * direction_to_sign(direction));
    }
    void run_speed()
    {
        driver.runSpeed();
    }

    // MultiStepper 連携
    void join_multi_stepper(MultiStepper& group)
    {
        group.addStepper(driver);
    }
};

/// @brief 原点どり機能を有し、他のモータと動作タイミングを同期できるステッピングモータ
class StepperMotorHomeableSynchronizable
{
    MultiStepper& sync_group;
    SteppingMotor driver;
    LimitSwitch limit_switch;

    int homing_sequence = 0;    // 0: 原点どりがまだ -1: 原点どり完了 1~原点どり中

    HomingConfig config;

public:
    /// @brief
    /// @param sync_group
    /// @param driver
    /// @param limit_switch
    StepperMotorHomeableSynchronizable(MultiStepper& sync_group,
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
        driver.join_multi_stepper(sync_group);
        limit_switch.begin();
    }

    // 原点どり終了で true を返す
    bool homing_update()
    {
        Serial.println(homing_sequence);
        switch (homing_sequence)
        {
        // case -1:    // すでに原点どり済だが再度原点どりするケース
        case 0:     // 起動時
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
    void set_target_position(float absolute_position_rev)
    {
        if (homing_sequence == -1)
        {
            Serial.println(absolute_position_rev);
            driver.set_acceleration(config.approach_switch_acceleration);
            driver.set_max_speed(abs(config.approach_switch_speed));
            driver.set_absolute_target_position(absolute_position_rev);
        }
    }
};