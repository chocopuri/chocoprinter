#include "src/network/wifi.hpp"
#include "src/network/mdns.hpp"
#include "src/network/http_server.hpp"
#include "src/command_parser/command_parser.hpp"
#include "src/executor/executor.hpp"
#include "src/file/file.hpp"
#include "src/hardwear/stepper_motor_homeable_synchronizable.hpp"

// #include "env.hpp"

static Executor<Command> executor;

static MultiStepper stepper_group;    // 動作を同期させるやつ

static StepperMotorHomeableSynchronizable stepper{
    stepper_group,
    AccelStepper{ AccelStepper::DRIVER, 0, 1 },
    LimitSwitch{ 2 },
    2000,    // [pulse/rev]
};

static AirCylinder left_air_cylinder{
    StepperMotorHomeableSynchronizable{
        stepper_group,
        AccelStepper{ AccelStepper::DRIVER, 0, 1 },
        LimitSwitch{ 2 },
        2000,    // [pulse/rev]
    },
    0.25,    // [mL/rev]
};

static file control_webpage[]{
    file{ "index.html" },
    file{ "style.css" },
    file{ "script.js" },
};

void setup()
{
    delay(1000);
    pinMode(LED_BUILTIN, OUTPUT);

    sd_card_begin();

    for (auto&& file : control_webpage)
    {
        if (file.exists())
            Serial.print("[ OK ] file exists: ");
        else
            Serial.print("[ NG ] file not exists: ");

        Serial.println(file.get_filename().c_str());
    }

    // wifi_begin(env::access_points);

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

    for (auto&& file : control_webpage)
    {
        http_server_add_get_handler("/" + file.get_filename(), [&file]() -> HttpResponse
                                    {
                                        if (const auto all_line_opt = file.read_all_line())
                                            return { 200, "text/html", *all_line_opt };
                                        else
                                            return { 500, "text/html", "server internal error. \n file open failed." }; });
    }

    stepper.begin();
}

void loop()
{
    executor.execute(Overload{
        [](CommandHome) -> bool
        {
            return left_air_cylinder.homing_update() && stepper.homing_update();
        },
        [](CommandMove) -> bool
        {
            std::cout << "CommandMove" << std::endl;
            return false;
        },
    });

    http_server_update();
    mdns_update();

    digitalWrite(LED_BUILTIN, millis() % 500 > 300);
}
