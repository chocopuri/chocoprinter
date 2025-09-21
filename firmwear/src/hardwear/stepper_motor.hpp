#pragma once

#include <AccelStepper.h>
#include <MultiStepper.h>


#include "../common/type.hpp"
class SteppingMotor;

class StepperSyncGroup
{
    MultiStepper group;
    long positions[MULTISTEPPER_MAX_STEPPERS];
    size_t stepper_count = 0;

public:
    StepperSyncGroup() = default;

    long* join_group(SteppingMotor& motor);

    bool run()
    {
        group.moveTo(positions);
        return not group.run();
    }
};


class SteppingMotor
{
    AccelStepper driver;
    Direction direction;
    int pulse_per_rev;
    float max_speed_rev = 0;

    long* sync_time_pos_ptr = nullptr;    // 動作を同期させるとき用の位置情報へのポインタ 実体は StepperSyncGroup が持つ

    friend class StepperSyncGroup;

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
        if (this->max_speed_rev != max_speed_rev)
        {
            this->max_speed_rev = max_speed_rev;
            driver.setMaxSpeed(max_speed_rev * pulse_per_rev);
        }
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

    // 同期位置制御 (実行は StepperSyncGroup 経由で)
    void attach_sync_group(StepperSyncGroup& sync_group)
    {
        sync_time_pos_ptr = sync_group.join_group(*this);
    }
    void set_absolute_target_position_sync(float absolute_position_rev)
    {
        *sync_time_pos_ptr = absolute_position_rev * pulse_per_rev * direction_to_sign(direction);
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
};


inline long* StepperSyncGroup::join_group(SteppingMotor& motor)
{
    group.addStepper(motor.driver);
    return &positions[stepper_count++];
}
