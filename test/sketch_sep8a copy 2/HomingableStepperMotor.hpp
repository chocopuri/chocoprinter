#pragma once


#include <AccelStepper.h>
#include <MultiStepper.h>

#include "LimitSwitch.hpp"


class HomingableStepperMotor {
  AccelStepper driver;
  LimitSwitch limit_switch;

  int homing_sequesnce = 0;  // 0: 原点どりがまだ -1: 原点どり完了 1~原点どり中

public:
  HomingableStepperMotor(AccelStepper&& driver, LimitSwitch&& limit_switch)
    : driver{ std::move(driver) }, limit_switch{ std::move(limit_switch) } {
      
    driver.setAcceleration(3000);
    driver.setMaxSpeed(10000);
    }

  void join_stepper_group(MultiStepper& group) {
    group.addStepper(driver);
  }

  void begin() {
    limit_switch.begin();
  }

  // 原点どり終了で true を返す
  bool homing_update() {
    Serial.println(homing_sequesnce);
    switch (homing_sequesnce) {
      case -1:  // すでに原点どり済だが再度原点どりするケース
      case 0:   // 起動時
        homing_sequesnce = 1;

      // リミットスイッチに向かって進行
      case 1:
        driver.setSpeed(-5000);
        driver.runSpeed();
        if (limit_switch.is_pressed()) {
          ++homing_sequesnce;
        } else {
          return false;
        }

      // リミットスイッチから少し離れる
      case 2:
        driver.setAcceleration(10000);
        driver.setMaxSpeed(5000);
        driver.move(3000);
        if (driver.run() == false) {
          driver.setCurrentPosition(0);  // 離れた点を原点とする
          homing_sequesnce = -1;
          return true;
        }
        else
        {
          return false;
        }

      default:
        return false;  // unreachable
    }
  }

  void update() {
    if (homing_sequesnce != -1)
      return;
  }
};