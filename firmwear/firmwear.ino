#include "printer.hpp"
#include "loop_cycle_controller.hpp"

static choco::printer printer{
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

static choco::loop_cycle_controller loop_ctrl{ 5 };    // 制御周期調整器

void setup()
{
    printer.init();
}

void loop()
{
    printer.update();
    loop_ctrl.update();
}
