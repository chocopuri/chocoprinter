#pragma once

#include "command_parser.hpp"

#include <queue>

namespace choco
{
    class command_sequencer
    {
        std::queue<command> command_queue;                        // コマンドキュー
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

        void receive_command_from_serial()
        {
            while (serial.available())
            {
                const String single_command_line = serial.readStringUntil('\n');
                if (const auto command = choco::parse_command({ single_command_line.c_str() }))
                {
                    command_queue.push(*command);    // コマンドをキューに追加
                    const auto m = "[o] Command received: " + choco::command_to_string(*command);
                    serial.println(m.c_str());    // パースにバグが無いかの確認もしたいので、single_command_lineではなくパース後のcommandを使って出力
                }
                else
                {
                    serial.println("[x] Invalid command: " + single_command_line);
                }
            }
        }

        template <typename E>
        void execute(E executer)
        {
            // 現在実行中のコマンドがなければ、受信した順にコマンドを取り出す
            if (not current_command && not command_queue.empty())
            {
                current_command = command_queue.front();
                command_queue.pop();

                const auto m = "[o] Command started: " + choco::command_to_string(*current_command);
                serial.println(m.c_str());
            }

            // std::variant なので、訪問者パターンでコマンドを実行する
            const bool is_finish = std::visit(executer, *current_command);
            
            // 訪問者の関数がtrueを返したら次のコマンドへ
            if (is_finish)
            {
                current_command = std::nullopt;    // コマンド実行完了

                const auto m = "[o] Command finished: " + choco::command_to_string(*current_command);
                serial.println(m.c_str());
            }
        }

        void clear_command()
        {
            command_queue = {};
            current_command = {};
        }
    };
}    // namespace choco
