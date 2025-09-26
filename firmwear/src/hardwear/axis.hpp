#pragma once

#include "stepper_motor_homeable_synchronizable.hpp"

class XAxis
{
    StepperMotorHomeableSynchronizable motor;
    const float mm_per_rev;
    const float x_limit_mm;

public:

    /// @brief 
    /// @param motor 
    /// @param mm_per_rev 回転数から移動距離への変換係数 [mm/rev]
    XAxis(StepperMotorHomeableSynchronizable&& motor, float mm_per_rev, float x_limit_mm)
        : motor{ std::move(motor) }
        , mm_per_rev{ mm_per_rev }
        , x_limit_mm{ x_limit_mm }
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

    void reset_homing()
    {
        motor.reset_homing();
    }

    void set_black_position(float x_mm, float speed)
    {
        set_target_position(x_mm + 65, speed);
    }

    void set_white_position(float x_mm, float speed)
    {
        set_target_position(x_mm, speed);
    }

private:
    
    void set_target_position(float x_mm, float speed)
    {
        motor.set_target_position(constrain(x_mm, 0, x_limit_mm) / mm_per_rev, -1, speed);
    }
};


class YAxis
{
    StepperMotorHomeableSynchronizable motor;
    const float mm_per_rev;
    const float x_limit_mm;

public:

    /// @brief 
    /// @param motor 
    /// @param mm_per_rev 回転数から移動距離への変換係数 [mm/rev]
    YAxis(StepperMotorHomeableSynchronizable&& motor, float mm_per_rev, float x_limit_mm)
        : motor{ std::move(motor) }
        , mm_per_rev{ mm_per_rev }
        , x_limit_mm{ x_limit_mm }
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

    void reset_homing()
    {
        motor.reset_homing();
    }
    
    void set_target_position(float x_mm, float speed)
    {
        motor.set_target_position(constrain(x_mm, 0, x_limit_mm) / mm_per_rev, -1, speed);
    }
};


class ZAxis
{
    StepperMotorHomeableSynchronizable motor;
    const float mm_per_rev;
    const float white_zero_pos_mm;   // 白色面の原点位置 [mm]
    const float black_zero_pos_mm;   // 黒色面の原点位置 [mm]
    const float z_limit_mm;          // Z軸の最大移動距離 [mm]

public:

    /// @brief 
    /// @param motor 
    /// @param mm_per_rev 回転数から移動距離への変換係数 [mm/rev]
    ZAxis(StepperMotorHomeableSynchronizable&& motor, float mm_per_rev, float white_zero_pos_mm, float black_zero_pos_mm, float z_limit_mm)
        : motor{ std::move(motor) }
        , mm_per_rev{ mm_per_rev }
        , white_zero_pos_mm{ white_zero_pos_mm }
        , black_zero_pos_mm{ black_zero_pos_mm }
        , z_limit_mm{ z_limit_mm }
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

    void reset_homing()
    {
        motor.reset_homing();
    }

    void set_black_position(float z_mm, float speed)
    {
        motor.set_target_position((constrain(z_mm, 0, z_limit_mm) - black_zero_pos_mm) / mm_per_rev, -1, speed);
    }

    void set_white_position(float z_mm, float speed)
    {
        motor.set_target_position((white_zero_pos_mm - constrain(z_mm, 0, z_limit_mm)) / mm_per_rev, -1, speed);
    }
};
