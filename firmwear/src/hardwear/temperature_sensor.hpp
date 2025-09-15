#pragma once

class TemperatureSensor
{
    int analog_pin;

public:
    TemperatureSensor(int analog_pin)
        : analog_pin{ analog_pin }
    {
    }

    float read_temperature() const
    {
        const int raw = analogRead(analog_pin);
        const float voltage = raw * 3.3 / 1023.0;
        return voltage / 8.25 * 100 + 20;
    }
};


// static TemperatureSensor s0{ 27 };
// static TemperatureSensor s1{ 28 };

// void setup() {
//   Serial.begin( 9600 );
// }

// void loop() {
//   Serial.print(s0.read_temperature());
//   Serial.print('\t');
//   Serial.println(s1.read_temperature());
//   delay( 100 ); 
// }