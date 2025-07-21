//
//   マイコンを使わないテスト
//
//   コマンドパーサーはC++の言語機能だけを使っているのでPC上でテストを行える
//

#include <cassert>
#include <iostream>

#include "command_parser.hpp"

bool test(const std::string& command_str, const Command& expect)
{
    if (const auto parsed_command = parse_command(command_str))
    {
        return expect == *parsed_command;
    }
    
    return false;
}

int main()
{
    assert(test("home", CommandHome{}));

    assert(test("move black  0    0    0   0   inject", CommandMove{ Color::black, Vec3{  0  ,  0  ,  0   }, 0  , true  }));
    assert(test("move black 10   20   30   1   inject", CommandMove{ Color::black, Vec3{ 10  , 20  , 30   }, 1  , true  }));
    assert(test("move black 10   20   30   1   stop",   CommandMove{ Color::black, Vec3{ 10  , 20  , 30   }, 1  , false }));
    assert(test("move white 10.1 20.1 30.1 1.1 inject", CommandMove{ Color::white, Vec3{ 10.1, 20.1, 30.1 }, 1.1, true  }));

    assert(not parse_command("homee"));                           // pypo
    assert(not parse_command("move pink 10 20 30 1 inject"));     // pink
    assert(not parse_command("move white 10 20 30 0 0 inject"));  // meny args
    assert(not parse_command("move white 10 20 30 0 hoge"));      // hoge

    std::cout << "passed" << std::endl;
}
