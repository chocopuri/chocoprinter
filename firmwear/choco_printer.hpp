//
//    プリンター本体
//
//    Copyright (c) 2025 okawa yusuke
//


#pragma once

#include "head.hpp"
#include "gantry.hpp"
#include "command_parser.hpp"

#include <queue>

namespace choco
{
    class choco_printer
    {

        // ハードウエア
        choco::gantry gantry;              // ガントリ (X, Y 軸)
        choco::head head;                  // ヘッド (Z 軸, チョコ容器)
        choco::limit_switch unpause_sw;    // ポーズ解除スイッチ

        // コマンド
        std::queue<command> command_queue;                        // コマンドキュー
        std::optional<command> current_command = std::nullopt;    // 現在実行中のコマンド

        f64 speed = 0.0;    // 移動速度 [mm/s]

        HardwareSerial& serial;    // シリアルポート

    public:
        choco_printer(
            choco::gantry&& gantry,
            choco::head&& head,
            choco::limit_switch&& unpause_sw,
            HardwareSerial& serial)
            : gantry{ std::move(gantry) }
            , head{ std::move(head) }
            , unpause_sw{ std::move(unpause_sw) }
            , serial{ serial }
        {
        }

        void init()
        {
            gantry.init();
            head.init();
            unpause_sw.init();

            serial.begin(115200);
            serial.setTimeout(20);
        }

        void update()
        {
            while (serial.available())
            {
                const String single_command_line = serial.readStringUntil('\n');
                if (const auto command = choco::parse_command({ single_command_line.c_str() }))
                {
                    // コマンドをキューに追加
                    command_queue.push(*command);
                    const auto m = "[o] Command received: " + single_command_line;
                    serial.println(m.c_str());    // パースにバグが無いかの確認もしたいので、single_command_lineではなくパース後のcommandを使って出力
                }
                else
                {
                    serial.println("[x] Invalid command: " + single_command_line);
                }
            }

            // 送信された順にコマンドを取り出す
            if (not current_command && not command_queue.empty())
            {
                current_command = command_queue.front();
                command_queue.pop();

                const auto m = "[o] Command started: " + choco::command_to_string(*current_command);
                serial.println(m.c_str());
            }

            // std::variant は共用体なので、訪問者パターンでコマンドを実行する
            const bool is_finish = std::visit([this](auto&& cmd) -> bool
                                              { return this->execute_command(cmd); },
                                              *current_command);
            if (is_finish)
            {
                current_command = std::nullopt;    // コマンド実行完了

                const auto m = "[o] Command finished: " + choco::command_to_string(*current_command);
                serial.println(m.c_str());
            }
        }

    private:
        bool execute_command(const command_type::go& target_position)
        {
            return gantry.move_to(target_position, speed);
        }

        bool execute_command(const command_type::speed& speed)
        {
            this->speed = speed;
            return true;
        }

        bool execute_command(const command_type::pause&)
        {
            return unpause_sw.is_pressed();    // ポーズ解除スイッチが押されるまで待機
        }

        bool execute_command(const command_type::home&)
        {
            // 原点取り
            return gantry.homing() && head.homing();
        }

        bool execute_command(const command_type::choco& cmd)
        {
            head.inject(cmd.color, cmd.inject);       // チョコの射出
            return head.move_to(cmd.color, cmd.z);    // Z座標を移動
        }

        bool execute_command(const command_type::clear&)
        {
            command_queue = {};
            // current_command = std::nullopt;    // 現在実行中のコマンドをクリア
            return true;
        }
    };

}    // namespace choco
