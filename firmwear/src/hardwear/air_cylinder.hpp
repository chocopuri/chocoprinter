#pragma once

#include "stepper_motor_homeable_synchronizable.hpp"

class AirCylinder
{
    StepperMotorHomeableSynchronizable motor;
    float ml_per_rev;

public:
    /// @brief
    /// @param motor
    /// @param ml_per_rev 回転数から空気量への変換係数 [mL/rev]
    AirCylinder(StepperMotorHomeableSynchronizable&& motor, float ml_per_rev, float limit_ml)
        : motor{ std::move(motor) }
        , ml_per_rev{ ml_per_rev }
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

    void set_air_volume(float volume_ml)
    {
        // motor.set_target_position(volume_ml / ml_per_rev);
        motor.set_target_position(50);
    }
};
