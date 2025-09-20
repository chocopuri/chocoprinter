//
//   コマンドパーサーテスト
//

#ifndef ARDUINO


#    include <cassert>
#    include "command_parser.hpp"

Command parse_unwrap(const std::string& command_str)
{
    if (const auto command = parse_command(command_str))
        return *command;

    assert(false);
    
    return CommandHome{}; // unreachable
}

int main()
{
    // true case
    assert((parse_unwrap("home") == CommandHome{}));
    assert((parse_unwrap("move black  0  0  0 0 inject") == CommandMove{ Color::black, Vec3{  0,  0,  0 }, 0, true }));
    assert((parse_unwrap("move black 10 20 30 1 inject") == CommandMove{ Color::black, Vec3{ 10, 20, 30 }, 1, true }));
    assert((parse_unwrap("move black 10 20 30 1   stop") == CommandMove{ Color::black, Vec3{ 10, 20, 30 }, 1, false }));

    // false case
    assert(not parse_command(""));                                  // empty
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
