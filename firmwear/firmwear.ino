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


static AirCylinder black_air_cylinder{
    StepperMotorHomeableSynchronizable{
        // right
        stepper_group,
        SteppingMotor{
            AccelStepper{ AccelStepper::DRIVER, 0, 1 },
            Direction::forward,
            200 * 8,    // [pulse/rev]
        },
        LimitSwitch{ 6 },
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

    const auto command_receive_handler = [](std::string_view command_text) -> HttpResponse
    {
        const auto parsed_command = parse_commands(std::string{ command_text });

        if (not parsed_command)
            return { 400, "application/json", R"({ "status": "Failed to parse command." })" };

        executor.replace_instructions({});    // 既存の命令をクリア


        // ホーミングするときに吸気するためチョコレートが吸い込まれてしまう。なので動作直前ではなくチョコがセッティングされていないときに原点どりをしておく。
        executor.push_instruction(CommandHomeAir{});

        for (auto&& cmd : *parsed_command)
        {
            std::visit(Overload{
                           [&](CommandHomeGantry)
                           {
                               executor.push_instruction(CommandHomeGantry{});
                           },
                           [&](const CommandMove& cmd_move)
                           {
                               static bool is_prev_inject = false;

                               // 射出開始時にエアーを出し管内の圧を上げる (チョコが出るまでにラグがあるため)
                               if (cmd_move.is_inject && not is_prev_inject)
                               {
                                   executor.push_instruction(CommandAir{ cmd_move.color, 4, 2 });
                               }

                               executor.push_instruction(cmd_move);

                               // 停止時にエアーを少し吸って管内の圧を下げる (チョコが垂れるのを防止するため)
                               if (not cmd_move.is_inject && is_prev_inject)
                               {
                                   executor.push_instruction(CommandAir{ cmd_move.color, -5, 8 });
                               }

                               is_prev_inject = cmd_move.is_inject;
                           },
                           [](CommandAir) {},    // パース段階で弾いているのでここには来ない
                           [](CommandHomeAir) {},
                       },
                       cmd);
        }

        executor.restart();

        return { 200, "application/json", R"({ "status": "OK" })" };

        // clang format のバグ対策
    };

    http_server_add_post_handler("/command", command_receive_handler);

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
        [](CommandHomeGantry) -> bool
        {
            const bool x_finished = x_axis.homing_update();
            const bool y_finished = y_axis.homing_update();
            const bool z_finished = z_axis.homing_update();

            return x_finished && y_finished && z_finished;
        },
        [](CommandHomeAir) -> bool
        {
            const bool white_finished = white_air_cylinder.homing_update();
            const bool black_finished = black_air_cylinder.homing_update();

            return white_finished && black_finished;
        },
        [](const CommandMove& cmd_move) -> bool
        {
            // std::ostringstream oss;
            // oss << cmd_move;
            // auto s = oss.str();
            // Serial.println(s.c_str());

            static bool is_first_call = true;

            y_axis.set_target_position(cmd_move.pos.y);

            static Vec3 last_pos{};
            const auto diff = cmd_move.pos - last_pos;

            const auto move_length = diff.length();

            if (is_first_call)
            {
                last_pos = cmd_move.pos;
            }

            if (cmd_move.color == Color::black)
            {
                x_axis.set_target_position(cmd_move.pos.x);
                z_axis.set_black_position(cmd_move.pos.z);

                if (cmd_move.is_inject && is_first_call)
                    black_air_cylinder.set_relative_air_volume(move_length / 60, 4);
            }
            else
            {
                x_axis.set_target_position(cmd_move.pos.x);
                z_axis.set_white_position(cmd_move.pos.z);
                
                if (cmd_move.is_inject && is_first_call)
                    white_air_cylinder.set_relative_air_volume(move_length / 60, 4);
            }

            is_first_call = false;

            if (const auto finished = stepper_group.run())
            {
                is_first_call = true;
                return true;
            }
            else
            {
                return false;
            }
        },
        [](CommandAir cmd_air) -> bool
        {
            static bool is_first_call = true;

            if (is_first_call)
            {
                if (cmd_air.color == Color::black)
                {
                    black_air_cylinder.set_relative_air_volume(cmd_air.volume_ml, cmd_air.speed);
                }
                else
                {
                    white_air_cylinder.set_relative_air_volume(cmd_air.volume_ml, cmd_air.speed);
                }

                is_first_call = false;
            }

            if (const auto finished = stepper_group.run())
            {
                is_first_call = true;
                return true;
            }
            else
            {
                return false;
            }
        },
    });
}
