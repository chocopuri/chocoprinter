//
//   コマンドパーサーテスト
//

#ifndef ARDUINO

#    include <cassert>

#    include "command_parser.hpp"

void expect_same(const std::string& command_str, const Command& expect)
{
    if (const auto parsed_command = parse_command(command_str))
    {
        if (expect == *parsed_command)
            return;

        std::cout << "[parser fail] input: \"" << command_str
                  << "\" expect: " << expect << std::endl;

        assert(false);
    }

    std::cout << "[parser fail] input: \"" << command_str
              << "\" expect: " << expect << std::endl;

    assert(false);
}

int main()
{
    // true case
    expect_same("home", CommandHome{});

    expect_same("move black 0 0 0 0 inject",
                CommandMove{ Color::black, Vec3{ 0, 0, 0 }, 0, true });

    expect_same("move black 10 20 30 1 inject",
                CommandMove{ Color::black, Vec3{ 10, 20, 30 }, 1, true });

    expect_same("move black 10 20 30 1 stop",
                CommandMove{ Color::black, Vec3{ 10, 20, 30 }, 1, false });

    expect_same("move white 10.1 20.1 30.1 1.1 inject",
                CommandMove{ Color::white, Vec3{ 10.1, 20.1, 30.1 }, 1.1, true });

    // false case
    assert(not parse_command("homee"));                             // typo
    assert(not parse_command("move pink 10 20 30 1 inject"));       // pink
    assert(not parse_command("move white 10 20 30 0 0 inject"));    // too many args
    assert(not parse_command("move white 10 20 30 0 hoge"));        // hoge

    if (const auto commands = parse_commands("home \r\n move white 10.1 20.1 30.1 1.1 inject \r\n move black 10 20 30 1 stop \r\n"))
    {
        // for (auto&& command : *commands)
        // {
        //     std::cout << command << std::endl;
        // }
    }
    else
    {
        assert(false);
    }

    std::cout << "passed" << std::endl;
}

#endif
