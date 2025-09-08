#pragma once

#include "stepper_motor_with_homing.hpp"

class AirCylinder
{
    StepperMotorWithHoming motor;
    float ml_per_rev;

public:

    /// @brief 
    /// @param ml_per_rev 一回転当たりに射出される空気の量 [mL/rev]
    AirCylinder(float ml_per_rev)
        : ml_per_rev{ ml_per_rev }
    {}

    void begin()
    {
        motor.begin();
    }

    void set_air_volume_ml(float volume_ml)
    {
        motor.set_target_position_rev(volume_ml * ml_per_rev);
    }
    
    bool homing_update()
    {
        return motor.homing_update();
    }
};
