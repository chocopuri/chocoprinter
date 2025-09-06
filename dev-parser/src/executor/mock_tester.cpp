//
//   コマンドパーサーテスト
//

#ifndef ARDUINO

#include <variant>
#include <iostream>
#include <cassert>

#include "executor.hpp"

int main()
{
    struct A{};
    struct B{};
    struct C{};

    using Command = std::variant<A, B, C>;

    {
        Executor<Command> e;

        e.replace_instructions({
            A{},
            B{},
            C{},
        });

        e.execute(Overload{
            [](A) -> bool { std::cout << "A" << std::endl; return true; },  // exec
            [](B) -> bool { std::cout << "B" << std::endl; return false; },
            [](C) -> bool { std::cout << "C" << std::endl; return false; },
        });

        e.execute(Overload{
            [](A) -> bool { std::cout << "A" << std::endl; return false; },
            [](B) -> bool { std::cout << "B" << std::endl; return true; },  // exec
            [](C) -> bool { std::cout << "C" << std::endl; return false; },
        });

        const bool is_finish = e.execute(Overload{
            [](A) -> bool { std::cout << "A" << std::endl; return false; },
            [](B) -> bool { std::cout << "B" << std::endl; return false; },
            [](C) -> bool { std::cout << "C" << std::endl; return true; },  // exec
        });

        assert(is_finish);
    }

    {
        Executor<Command> e;

        e.replace_instructions({
            A{},
            B{},
            C{},
        });

        e.execute(Overload{
            [](A) -> bool { std::cout << "A" << std::endl; return false; },  // exec
            [](B) -> bool { std::cout << "B" << std::endl; return false; },
            [](C) -> bool { std::cout << "C" << std::endl; return false; },
        });

        e.execute(Overload{
            [](A) -> bool { std::cout << "A" << std::endl; return true; },  // exec
            [](B) -> bool { std::cout << "B" << std::endl; return false; },
            [](C) -> bool { std::cout << "C" << std::endl; return false; },
        });

        const bool is_finish_1 = e.execute(Overload{
            [](A) -> bool { std::cout << "A" << std::endl; return false; },
            [](B) -> bool { std::cout << "B" << std::endl; return true; },  // exec
            [](C) -> bool { std::cout << "C" << std::endl; return false; },
        });

        assert(is_finish_1 == false);

        const bool is_finish_2 = e.execute(Overload{
            [](A) -> bool { std::cout << "A" << std::endl; return false; },
            [](B) -> bool { std::cout << "B" << std::endl; return false; },
            [](C) -> bool { std::cout << "C" << std::endl; return true; },  // exec
        });

        assert(is_finish_2);

        const bool is_finish_3 = e.execute(Overload{
            [](A) -> bool { std::cout << "no exec" << std::endl; return false; },
            [](B) -> bool { std::cout << "no exec" << std::endl; return false; },
            [](C) -> bool { std::cout << "no exec" << std::endl; return true; },
        });

        assert(is_finish_3);
    }

    std::cout << "passed" << std::endl;
}

#endif
