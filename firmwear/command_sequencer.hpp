//
//    コマンドを順に実行するクラス
//
//    Copyright (c) 2025 okawa yusuke
//


#pragma once

#include "command_parser.hpp"

#include <queue>

namespace choco
{
    class command_sequencer
    {
        std::deque<command> command_queue;                        // コマンドキュー
        std::optional<command> current_command = std::nullopt;    // 現在実行中のコマンド

        HardwareSerial& serial;    // シリアルポート

    public:
        command_sequencer(HardwareSerial& serial)
            : serial{ serial }
        {
        }

        void init()
        {
            serial.begin(115200);
            serial.setTimeout(20);
        }

        template <typename E>
        void execute_command(E executer)
        {
            while (serial.available())
            {
                const String single_command_line = serial.readStringUntil('\n');

                const auto command = choco::parse_command({ single_command_line.c_str() });
                if (not command)
                {
                    using namespace std::string_literals;
                    log("[x] invalid command : "s + single_command_line.c_str());
                    continue;
                }

                log("[o] command received: " + choco::command_to_string(*command));  // パースにバグが無いかの確認もしたいので、single_command_lineではなくパース後のcommandを使って出力
                
                std::visit([this, &executer, &command](auto&& cmd){
                    using T = std::decay_t<decltype(cmd)>;

                    if constexpr (std::is_base_of_v<command_type::immediate_tag, T>)
                    {
                        // 即実行命令 (cmd の型が command_type::immediate を継承している場合)
                        log("[o] immediate command started : " + choco::command_to_string(cmd));
                        executer(cmd);
                        log("[o] immediate command finished: " + choco::command_to_string(cmd));
                    }
                    else
                    {
                        command_queue.push_back(*command);    // コマンドをキューに追加
                    }
                }, *command);
            }

            // 現在実行中のコマンドがなければ、受信した順にコマンドを取り出す
            if (not current_command && not command_queue.empty())
            {
                current_command = command_queue.front();
                command_queue.pop_front();

                log("[o] command started : " + choco::command_to_string(*current_command));
            }

            if (current_command)
            {
                // std::variant なので、訪問者パターンでコマンドを実行する
                const bool is_finish = std::visit(executer, *current_command);
                
                // 訪問者の関数がtrueを返したら次のコマンドへ
                if (is_finish)
                {
                    log("[o] command finished: " + choco::command_to_string(*current_command));
                    current_command = std::nullopt;    // コマンド実行完了
                }
            }
        }

        /// @brief 全消去
        void clear_command()
        {
            command_queue = {};
            current_command = {};
        }

        /// @brief デバッグ出力
        void dump_all() const
        {
            for (auto&& command : command_queue)
                log(choco::command_to_string(command));
        }

        /// @brief デバッグ出力
        void dump_current() const
        {
            if (current_command)
                log(choco::command_to_string(*current_command));
            else
                log("command is not running");
        }

    private:
        void log(const std::string& message) const
        {
            serial.println(message.c_str());
        }
    };
}    // namespace choco
