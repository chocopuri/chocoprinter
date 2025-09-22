#pragma once

#include "stepper_motor_homeable_synchronizable.hpp"

class AirCylinder
{
    StepperMotorHomeableSynchronizable motor;
    float ml_per_rev;
    float limit_ml;
    float current_volume_ml = 0;

public:
    /// @brief
    /// @param motor
    /// @param ml_per_rev 回転数から空気量への変換係数 [mL/rev]
    AirCylinder(StepperMotorHomeableSynchronizable&& motor, float ml_per_rev, float limit_ml)
        : motor{ std::move(motor) }
        , ml_per_rev{ ml_per_rev }
        , limit_ml{ limit_ml }
    {
    }

    void begin()
    {
        motor.begin();
    }
    
    bool homing_update()
    {
        return motor.homing_update();
    }

    /// @brief 絶対位置移動
    /// @param volume_ml 移動する空気量 [mL]
    void set_absolute_air_volume(float volume_ml)
    {
        if (0 <= volume_ml && volume_ml <= limit_ml)
        {
            motor.set_target_position(volume_ml / ml_per_rev, 5, 5);
            current_volume_ml = volume_ml;
        }
    }

    /// @brief 相対位置移動
    void set_relative_air_volume(float volume_ml)
    {
        set_absolute_air_volume(current_volume_ml + volume_ml);
    }
};
