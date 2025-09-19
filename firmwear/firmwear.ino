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
#include "src/hardwear/axis.hpp"
#include "src/hardwear/air_cylinder.hpp"

#include <sstream>

#include "env.hpp"

static Executor<Command> executor;
// struct A
// {
// };
// struct B
// {
// };
// struct C
// {
// };
// static Executor<std::variant<A, B, C>> executor;

static StepperSyncGroup stepper_group;    // 動作を同期させるやつ

// 調整済み
ZAxis z_axis{
    StepperMotorHomeableSynchronizable{
        stepper_group,
        SteppingMotor{
            AccelStepper{ AccelStepper::DRIVER, 26, 22 },
            Direction::forward,
            200 * 8,    // [pulse/rev]
        },
        LimitSwitch{ 4 },
        HomingConfig{
            .approach_switch_speed = -0.5,
            .approach_switch_acceleration = 10,
            .leave_switch_distance = 0.93,
            .leave_switch_speed = 0.5,
            .leave_switch_acceleration = 10,
        },
    },
    14 * M_PI,    // mm/rev
    40,           // white_zero_pos_mm
    40,           // black_zero_pos_mm
    80,           // z_limit_mm
};

// AirCylinder left_air_cylinder{
//     StepperMotorHomeableSynchronizable{
//         stepper_group,
//         SteppingMotor{
//             AccelStepper{ AccelStepper::DRIVER, 13, 12 },
//             Direction::forward,
//             200 * 8,    // [pulse/rev]
//         },
//         LimitSwitch{ 5 },
//         HomingConfig{
//             .approach_switch_speed = -2,
//             .approach_switch_acceleration = 10,
//             .leave_switch_distance = 3,
//             .leave_switch_speed = 2,
//             .leave_switch_acceleration = 10,
//         },
//     },
//     1 / 3.5,    // mL/rev (ねじ山間隔1mm, 3.5mmで1mL)
//     20,
// };

// 調整済み
XAxis x_axis{
    StepperMotorHomeableSynchronizable{
        stepper_group,
        SteppingMotor{
            AccelStepper{ AccelStepper::DRIVER, 14, 15 },
            Direction::forward,
            200 * 8,    // [pulse/rev]
        },
        LimitSwitch{ 2 },
        HomingConfig{
            .approach_switch_speed = 2,
            .approach_switch_acceleration = 10,
            .leave_switch_distance = -3,
            .leave_switch_speed = 2,
            .leave_switch_acceleration = 10,
        },
    },
    14 * M_PI,    // mm/rev
    130,          // x_limit_mm
};

YAxis y_axis{
    StepperMotorHomeableSynchronizable{
        stepper_group,
        SteppingMotor{
            AccelStepper{ AccelStepper::DRIVER, 21, 20 },
            Direction::backward,
            200 * 8,    // [pulse/rev]
        },
        LimitSwitch{ 3 },
        HomingConfig{
            .approach_switch_speed = -2,
            .approach_switch_acceleration = 10,
            .leave_switch_distance = 0.1,
            .leave_switch_speed = 2,
            .leave_switch_acceleration = 10,
        },
    },
    14 * M_PI,    // mm/rev
    130,          // y_limit_mm
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

    wifi_begin(env::access_points);

    mdns_begin("pico");    // http://pico.local

    http_server_begin();

    http_server_add_post_handler("/command", [](std::string_view sv) -> HttpResponse
                                 {
                                    if (const auto parsed = parse_commands(std::string{ sv }))
                                    {
                                        executor.replace_instructions(*parsed);
                                        return { 200, "application/json", R"({ "status": "OK" })" };
                                    }
                                    else
                                        return { 400, "application/json", R"({ "status": "Failed to parse command." })" }; });

    // for (auto&& file : control_webpage)
    // {
    //     http_server_add_get_handler("/" + file.get_filename(), [&file]() -> HttpResponse
    //                                 {
    //                                     if (const auto all_line_opt = file.read_all_line())
    //                                         return { 200, "text/html", *all_line_opt };
    //                                     else
    //                                         return { 500, "text/html", "server internal error. \n file open failed." }; });
    // }
    // executor.replace_instructions({
    //     *parse_command("home"),
    //     *parse_command("move black 0 0 0 0 inject"),
    //     *parse_command("move black 0 100 0 1 inject"),
    //     *parse_command("move white 100 100 0 1 inject"),
    //     *parse_command("move white 100 0 0 1 inject"),
    //     *parse_command("move black 0 0 0 0 inject"),
    //     *parse_command("move black 100 100 0 1 inject"),
    //     *parse_command("move white 10.1 20.1 30.1 1.1 inject"),
    // });
    // left_air_cylinder.begin();

    x_axis.begin();
    y_axis.begin();
    z_axis.begin();
}

void loop()
{
    executor.execute(Overload{
        [](CommandHome) -> bool
        {
            Serial.println("homing");

            const bool x_finished = x_axis.homing_update();
            const bool y_finished = y_axis.homing_update();
            const bool z_finished = z_axis.homing_update();

            return x_finished && y_finished && z_finished;
        },
        [](const CommandMove& cmd_move) -> bool
        {
            std::ostringstream oss;
            oss << cmd_move;
            auto s = oss.str();
            Serial.println(s.c_str());

            x_axis.set_target_position(cmd_move.pos.x);
            y_axis.set_target_position(cmd_move.pos.y);
            if (cmd_move.color == Color::black)
                z_axis.set_black_position(cmd_move.pos.z);
            else
                z_axis.set_white_position(cmd_move.pos.z);

            return stepper_group.run();
        },
    });
    
    http_server_update();
    mdns_update();

    // digitalWrite(LED_BUILTIN, millis() % 500 > 300);
}
