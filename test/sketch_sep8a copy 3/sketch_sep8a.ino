// MultiStepper.pde
// -*- mode: C++ -*-
// Use MultiStepper class to manage multiple steppers and make them all move to
// the same position at the same time for linear 2d (or 3d) motion.

#include <AccelStepper.h>
#include <MultiStepper.h>


AccelStepper steppers[]{
  // AccelStepper{ AccelStepper::DRIVER, 28, 27 },  // エアL step dir
  // AccelStepper{ AccelStepper::DRIVER, 0, 1 },    // エアR step dir
  // AccelStepper{ AccelStepper::DRIVER, 14, 15 },  // X step dir
  AccelStepper{ AccelStepper::DRIVER, 26, 22 },  // Z step dir
  // AccelStepper{ AccelStepper::DRIVER, 21, 20 },  // Y step dir
}; 

// Up to 10 steppers can be handled as a group by MultiStepper
// MultiStepper stepper_group;

void setup() {
  Serial.begin(9600);

  // for (auto&& stepper : steppers)
  // {
  //   stepper.setAcceleration(3000);
  //   stepper.setMaxSpeed(10000);
  // }

  // for (auto&& stepper : steppers) {
  //   stepper_group.addStepper(stepper);
  // }

  pinMode(2, INPUT_PULLUP);
}

// stepper1.runToNewPosition(5000);

void loop() {

  static int s;

  switch (s)
  {
  case 0:
    if (Serial.available())
    {
      auto str = Serial.readStringUntil('\n');
      if (str == "home")
      {
        ++s;
      }
    }
    break;
  
  case 1:
    steppers[0].setSpeed(-5000);
    steppers[0].runSpeed();
    if (not digitalRead(2))
    {
      ++s;
      steppers[0].setCurrentPosition(0);
    }
    break;
  
  case 2:
    steppers[0].setAcceleration(10000);
    steppers[0].setMaxSpeed(5000);
    steppers[0].moveTo(40000);
    if (steppers[0].run() == false) ++s;
    break;
  
  case 3:
    s = 0;
    break;
  
  }

  Serial.println(s);

  // stepper_group.runSpeedToPosition(); // Blocks until all are in position
  // {
  //   Serial.println("1");
  //   long positions[2];
  //   positions[0] = 5000;
  //   positions[1] = 50;
  //   stepper_group.moveTo(positions);
  //   delay(1000);
  // }

  // {
  //   Serial.println("2");
  //   long positions[2];
  //   positions[0] = -5000;
  //   positions[1] = 100;
  //   stepper_group.moveTo(positions);
  //   stepper_group.runSpeedToPosition();  // Blocks until all are in position
  //   delay(1000);
  // }
}

