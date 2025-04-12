//
//    チョコプリンター機構用ファームウエア
//
//    Copyright (c) 2025 okawa yusuke
//
//    ボード : Raspberry Pi Pico (https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json)
//


#include "choco_printer.hpp"
#include "loop_cycle_controller.hpp"

static choco::choco_printer printer{
    choco::gantry{
        choco::homing_available_motor{
            choco::stepper_motor{ 2, 3, 4 },
            choco::limit_switch{ 5 },
        },
        choco::homing_available_motor{
            choco::stepper_motor{ 6, 7, 8 },
            choco::limit_switch{ 9 },
        },
    },
    choco::head{
        choco::homing_available_motor{
            choco::stepper_motor{ 8, 9, 10 },
            choco::limit_switch{ 11 },
        },
        choco::solenoid_valve{ 11 },
        choco::solenoid_valve{ 12 },
    },
    choco::limit_switch{ 13 },
    Serial,
};

static choco::loop_cycle_controller loop_ctrl{ 5 };

void setup()
{
    printer.init();
}

void loop()
{
    printer.update();
    loop_ctrl.update();
}
