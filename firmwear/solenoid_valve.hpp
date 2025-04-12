//
//    ソレノイドバルブ
//
//    Copyright (c) 2025 okawa yusuke
//


#pragma once

#include "type.hpp"

namespace choco
{

    /// @brief ソレノイドバルブ制御クラス
    class solenoid_valve
    {

        i8 pin;

    public:
        solenoid_valve(i8 pin)
            : pin{ pin }
        {
        }

        void init()
        {
            pinMode(pin, OUTPUT);
            close();
        }

        void open()
        {
            digitalWrite(pin, HIGH);
        }

        void close()
        {
            digitalWrite(pin, LOW);
        }
    };

}    // namespace choco