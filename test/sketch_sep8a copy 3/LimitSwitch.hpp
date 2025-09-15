#pragma once

class LimitSwitch
{
  int pin;

public:
  LimitSwitch(int pin)
    : pin{ pin }
  {}

  void begin()
  {
    pinMode(pin, INPUT_PULLUP);
  }

  bool is_pressed() const
  {
    return digitalRead(pin) == LOW;
  }
};
