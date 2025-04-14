//
//    プリンター本体
//
//    Copyright (c) 2025 okawa yusuke
//


#pragma once

#include "head.hpp"
#include "gantry.hpp"
#include "command_sequencer.hpp"

namespace choco
{
    class choco_printer
    {

        // ハードウエア
        choco::gantry gantry;              // ガントリ (X, Y 軸)
        choco::head head;                  // ヘッド (Z 軸, チョコ容器)
        choco::limit_switch unpause_sw;    // ポーズ解除スイッチ

        choco::command_sequencer sequencer;

        f64 speed = 0.0;    // 移動速度 [mm/s]

    public:
        choco_printer(
            choco::gantry&& gantry,
            choco::head&& head,
            choco::limit_switch&& unpause_sw,
            choco::command_sequencer&& sequencer)
            : gantry{ std::move(gantry) }
            , head{ std::move(head) }
            , unpause_sw{ std::move(unpause_sw) }
            , sequencer{ std::move(sequencer) }
        {
        }

        void init()
        {
            gantry.init();
            head.init();
            unpause_sw.init();
            sequencer.init();
        }

        void update()
        {
            // 引数にコマンドの引数をのせてコールバックする
            // コマンドの種類ごとに異なるオーバーロード関数が呼ばれる
            // コールバック関数がtrueを返したら次のコマンドに移行する
            sequencer.execute_command([this](auto&& cmd)
                                      { return executer(cmd); });
        }

    private:
        bool executer(const command_type::go& target_position)
        {
            return gantry.move_to(target_position, speed);
        }

        bool executer(const command_type::speed& speed)
        {
            this->speed = speed;
            return true;
        }

        bool executer(const command_type::pause&)
        {
            return unpause_sw.is_pressed();    // ポーズ解除スイッチが押されるまで待機
        }

        bool executer(const command_type::home&)
        {
            // 原点取り
            return gantry.homing() && head.homing();
        }

        bool executer(const command_type::choco& cmd)
        {
            if (cmd.inject)
            {
                if (not head.move_to(cmd.color, cmd.z))
                    return false;
                head.inject(cmd.color, true);    // 移動できたらチョコの射出
                return true;
            }
            else
            {
                head.inject(cmd.color, false);            // チョコの射出停止してから移動
                return head.move_to(cmd.color, cmd.z);
            }
        }

        bool executer(const command_type::clear&)
        {
            sequencer.clear_command();
            return true;
        }

        bool executer(const command_type::dump& cmd)
        {
            if (cmd.type == command_type::dump::dump_type::current)
                sequencer.dump_current();
            else if (cmd.type == command_type::dump::dump_type::all)
                sequencer.dump_all();
            return true;
        }
    };

}    // namespace choco
