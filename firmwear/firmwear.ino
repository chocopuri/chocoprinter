//
//    チョコレート3Dプリンタ ファームウエア
//
//    依存ボード：
//       https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
//
//    依存ライブラリ：
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

static StepperSyncGroup stepper_group;    // 動作を同期させるやつ

static ZAxis z_axis{
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

static XAxis x_axis{
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

static YAxis y_axis{
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


static AirCylinder white_air_cylinder{
    StepperMotorHomeableSynchronizable{
        stepper_group,
        SteppingMotor{
            AccelStepper{ AccelStepper::DRIVER, 13, 12 },
            Direction::forward,
            200 * 8,    // [pulse/rev]
        },
        LimitSwitch{ 5 },
        HomingConfig{
            .approach_switch_speed = -1.5,
            .approach_switch_acceleration = 10,
            .leave_switch_distance = 1,
            .leave_switch_speed = 2,
            .leave_switch_acceleration = 10,
        },
    },
    1 / 3.5,    // mL/rev (ねじ山間隔1mm, 3.5mmで1mL)
    20,
};


static StepperMotorHomeableSynchronizable black_air_cylinder{ // right
    stepper_group,
    SteppingMotor {
        AccelStepper{ AccelStepper::DRIVER, 0, 1 },
        Direction::forward,
        200 * 8,  // [pulse/rev]
    },
    LimitSwitch{ 6 },
    HomingConfig {
        .approach_switch_speed = -1.5,
        .approach_switch_acceleration = 10,
        .leave_switch_distance = 1,
        .leave_switch_speed = 2,
        .leave_switch_acceleration = 10,
    },
};

// static file control_webpage[]{
//     file{ "index.html" },
//     file{ "style.css" },
//     file{ "script.js" },
// };

static file access_point_file{ "access_point.csv" };

std::vector<AccessPoint> load_access_points()
{
    std::vector<AccessPoint> aps{};

    if (const auto all_line_opt = access_point_file.read_all_line())
    {
        std::istringstream iss{ *all_line_opt };
        std::string line;

        while (std::getline(iss, line))
        {
            const auto comma_pos = line.find(',');
            if (comma_pos == std::string::npos)
                continue;

            const auto ssid = line.substr(0, comma_pos);
            const auto pass = line.substr(comma_pos + 1);

            if (ssid.size() == 0 || pass.size() == 0)
                continue;

            aps.push_back(AccessPoint{ ssid.c_str(), pass.c_str() });
        }
    }

    return aps;
}

void setup()
{
    sd_card_begin();

    // for (auto&& file : control_webpage)
    // {
    //     if (file.exists())
    //         Serial.print("[ OK ] file exists: ");
    //     else
    //         Serial.print("[ NG ] file not exists: ");

    //     Serial.println(file.get_filename().c_str());
    // }

    wifi_begin(env::access_points);
    // wifi_begin(load_access_points());

    mdns_begin("pico");    // http://pico.local

    http_server_begin();

    http_server_add_post_handler("/command", [](std::string_view command_text) -> HttpResponse
                                 {
                                    if (const auto parsed = parse_commands(std::string{ command_text }))
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
}

void loop()
{
    http_server_update();
    mdns_update();
    wifi_update();
}

void setup1()
{
    white_air_cylinder.begin();
    black_air_cylinder.begin();

    x_axis.begin();
    y_axis.begin();
    z_axis.begin();
}

void loop1()
{
    
    executor.execute(Overload{
        [](CommandHome) -> bool
        {
            // Serial.println("homing");

            const bool x_finished = x_axis.homing_update();
            const bool y_finished = y_axis.homing_update();
            const bool z_finished = z_axis.homing_update();
            const bool white_finished = white_air_cylinder.homing_update();
            const bool black_finished = black_air_cylinder.homing_update();

            return x_finished && y_finished && z_finished && white_finished && black_finished;
        },
        [](const CommandMove& cmd_move) -> bool
        {
            // std::ostringstream oss;
            // oss << cmd_move;
            // auto s = oss.str();
            // Serial.println(s.c_str());

            x_axis.set_target_position(cmd_move.pos.x);
            y_axis.set_target_position(cmd_move.pos.y);

            const auto move_lenght = cmd_move.pos.length();

            if (cmd_move.color == Color::black)
            {
                z_axis.set_black_position(cmd_move.pos.z);
                if (cmd_move.is_inject)
                    white_air_cylinder.set_air_volume(move_lenght / 20);
                else
                    white_air_cylinder.set_air_volume(0);
            }
            else
            {
                z_axis.set_white_position(cmd_move.pos.z);
                if (cmd_move.is_inject)
                    white_air_cylinder.set_air_volume(move_lenght / 20);
                else
                    white_air_cylinder.set_air_volume(0);
            }

            return stepper_group.run();
        },
    });

}
