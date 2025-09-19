//
//    ファームウエア
//
//    ボード
//
//    ライブラリ
//       https://github.com/waspinator/AccelStepper
//


#include "src/network/wifi.hpp"
#include "src/network/mdns.hpp"
#include "src/network/http_server.hpp"
#include "src/command_parser/command_parser.hpp"
#include "src/executor/executor.hpp"
#include "src/file/file.hpp"
#include "src/hardwear/stepper_motor_homeable_synchronizable.hpp"
#include "src/hardwear/z_axis.hpp"
#include "src/hardwear/air_cylinder.hpp"


// #include "env.hpp"

// static Executor<Command> executor;
struct A{};
struct B{};
struct C{};
static Executor<std::variant<A, B, C>> executor;

static StepperSyncGroup stepper_group;    // 動作を同期させるやつ

// 調整済み
ZAxis z {
    StepperMotorHomeableSynchronizable {
        stepper_group,
        SteppingMotor {
            AccelStepper { AccelStepper::DRIVER, 26, 22 },
            Direction::forward,
            200 * 8,  // [pulse/rev]
        },
        LimitSwitch { 4 },
        HomingConfig {
            .approach_switch_speed = -0.5,
            .approach_switch_acceleration = 10,
            .leave_switch_distance = 0.93,
            .leave_switch_speed = 0.5,
            .leave_switch_acceleration = 10,
        },
    },
    14 * M_PI,  // mm/rev
    40,  // white_zero_pos_mm
    40,  // black_zero_pos_mm
    80,  // z_limit_mm
};

// AirCylinder left_air_cylinder {
//     StepperMotorHomeableSynchronizable {
//         stepper_group,
//         SteppingMotor {
//             AccelStepper{ AccelStepper::DRIVER, 13, 12 },
//             Direction::forward,
//             200 * 8,  // [pulse/rev]
//         },
//         LimitSwitch{ 5 },
//         HomingConfig {
//             .approach_switch_speed = -2,
//             .approach_switch_acceleration = 10,
//             .leave_switch_distance = 3,
//             .leave_switch_speed = 2,
//             .leave_switch_acceleration = 10,
//         },
//     },
//     1 / 3.5,  // mL/rev (ねじ山間隔1mm, 3.5mmで1mL)
//     20,
// };

// 調整済み
// static StepperMotorHomeableSynchronizable x_axsis{
//     stepper_group,
//     SteppingMotor {
//         AccelStepper{ AccelStepper::DRIVER, 14, 15 },
//         Direction::forward,
//         200 * 8,  // [pulse/rev]
//     },
//     LimitSwitch{ 2 },
//     HomingConfig {
//         .approach_switch_speed = 2,
//         .approach_switch_acceleration = 10,
//         .leave_switch_distance = -3,
//         .leave_switch_speed = 2,
//         .leave_switch_acceleration = 10,
//     },
// };
static StepperMotorHomeableSynchronizable y_axsis{
    stepper_group,
    SteppingMotor {
        AccelStepper{ AccelStepper::DRIVER, 21, 20 },
        Direction::forward,
        200 * 8,  // [pulse/rev]
    },
    LimitSwitch{ 3 },
    HomingConfig {
        .approach_switch_speed = 2,
        .approach_switch_acceleration = 10,
        .leave_switch_distance = -0.1,
        .leave_switch_speed = 2,
        .leave_switch_acceleration = 10,
    },
};
  // AccelStepper{ AccelStepper::DRIVER, 13, 12 },  // エアL step dir ok
  // AccelStepper{ AccelStepper::DRIVER, 0, 1 },    // エアR step dir ok
//   AccelStepper{ AccelStepper::DRIVER, 14, 15 },  // X step dir ok
//   AccelStepper{ AccelStepper::DRIVER, 26, 22 },  // Z step dir ok
  // AccelStepper{ AccelStepper::DRIVER, 21, 20 },  // Y step dir ok
// static StepperMotorHomeableSynchronizable right_air_cylinder{
//     stepper_group,
//     SteppingMotor {
//         AccelStepper{ AccelStepper::DRIVER, 0, 1 },
//         Direction::forward,
//         200 * 8,  // [pulse/rev]
//     },
//     LimitSwitch{ 6 },
//     HomingConfig {
//         .approach_switch_speed = -2,
//         .approach_switch_acceleration = 10,
//         .leave_switch_distance = 3,
//         .leave_switch_speed = 2,
//         .leave_switch_acceleration = 10,
//     },
// };

// static StepperMotorHomeableSynchronizable left_air_cylinder{
//     stepper_group,
//     SteppingMotor {
//         AccelStepper{ AccelStepper::DRIVER, 13, 12 },
//         Direction::forward,
//         200 * 8,  // [pulse/rev]
//     },
//     LimitSwitch{ 5 },
//     HomingConfig {
//         .approach_switch_speed = -2,
//         .approach_switch_acceleration = 10,
//         .leave_switch_distance = 3,
//         .leave_switch_speed = 2,
//         .leave_switch_acceleration = 10,
//     },
// };

// static StepperMotorHomeableSynchronizable z_axsis{
//     stepper_group,
//     SteppingMotor {
//         AccelStepper{ AccelStepper::DRIVER, 26, 22 },
//         Direction::forward,
//         200 * 8,  // [pulse/rev]
//     },
//     LimitSwitch{ 4 },
//     HomingConfig {
//         .approach_switch_speed = -0.5,
//         .approach_switch_acceleration = 10,
//         .leave_switch_distance = 0.93,
//         .leave_switch_speed = 0.5,
//         .leave_switch_acceleration = 10,
//     },
// };

// static AirCylinder left_air_cylinder{
//     StepperMotorHomeableSynchronizable{
//         stepper_group,
//         AccelStepper{ AccelStepper::DRIVER, 0, 1 },
//         LimitSwitch{ 2 },
//         2000,    // [pulse/rev]
//     },
//     0.25,    // [mL/rev]
// };

// static file control_webpage[]{
//     file{ "index.html" },
//     file{ "style.css" },
//     file{ "script.js" },
// };

void setup()
{
    delay(1000);
    // pinMode(LED_BUILTIN, OUTPUT);

    // sd_card_begin();

    // for (auto&& file : control_webpage)
    // {
    //     if (file.exists())
    //         Serial.print("[ OK ] file exists: ");
    //     else
    //         Serial.print("[ NG ] file not exists: ");

    //     Serial.println(file.get_filename().c_str());
    // }

    // // wifi_begin(env::access_points);

    // mdns_begin("pico");    // http://pico.local

    // http_server_begin();

    // http_server_add_post_handler("/command", [](std::string_view sv) -> HttpResponse
    //                              {
    //                                 if (const auto parsed = parse_commands(std::string{ sv }))
    //                                 {
    //                                     executor.replace_instructions(*parsed);
    //                                     return { 200, "application/json", R"({ "status": "OK" })" };
    //                                 }
    //                                 else
    //                                     return { 400, "application/json", R"({ "status": "Failed to parse command." })" }; });

    // for (auto&& file : control_webpage)
    // {
    //     http_server_add_get_handler("/" + file.get_filename(), [&file]() -> HttpResponse
    //                                 {
    //                                     if (const auto all_line_opt = file.read_all_line())
    //                                         return { 200, "text/html", *all_line_opt };
    //                                     else
    //                                         return { 500, "text/html", "server internal error. \n file open failed." }; });
    // }
    executor.replace_instructions({ A{}, B{}, C{} });
    // left_air_cylinder.begin();
    y_axsis.begin();
    z.begin();
}

void loop()
{
    // executor.execute(Overload{
    //     [](CommandHome) -> bool
    //     {
    //         return left_air_cylinder.homing_update() && stepper.homing_update();
    //     },
    //     [](CommandMove) -> bool
    //     {
    //         std::cout << "CommandMove" << std::endl;
    //         return false;
    //     },
    // });
    executor.execute(Overload{
        [](A) -> bool
        {
            Serial.println("A");
            return y_axsis.homing_update();
            // return true;
        },
        [](B) -> bool { return z.homing_update(); },
        [](C) -> bool
        {
            Serial.println("B");

            z.set_black_position(0);
            // z.set_white_position(0);

            y_axsis.set_target_position(5);
            
            // left_air_cylinder.set_air_volume(10);


            Serial.print(stepper_group.run() ? "run" : "end");
            return false;
        },
    });

    // http_server_update();
    // mdns_update();

    // digitalWrite(LED_BUILTIN, millis() % 500 > 300);
}
