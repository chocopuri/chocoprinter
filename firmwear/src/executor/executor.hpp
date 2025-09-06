#pragma once

#include <vector>

template <class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template <class... Ts> Overload(Ts...) -> Overload<Ts...>;

template <typename Inst>
class Executor
{
    size_t current_inst_idx{};
    std::vector<Inst> instructions{};

public:
    Executor() = default;

    void replace_instructions(const std::vector<Inst>& new_instructions)
    {
        current_inst_idx = 0;
        instructions = new_instructions;
    }

    template <typename Visitor>
    bool execute(Visitor&& visitor)
    {
        if (not instructions.size())
        {
            return true;    // 命令がない -> 命令が完了していると同義
        }

        if (current_inst_idx == instructions.size())
            return true;

        const Inst& current_inst = instructions.at(current_inst_idx);

        const bool is_finish = std::visit(visitor, current_inst);

        if (is_finish)
        {
            if (current_inst_idx < instructions.size())
                ++current_inst_idx;

            if (current_inst_idx == instructions.size())
                return true;
        }

        return false;
    }
};
