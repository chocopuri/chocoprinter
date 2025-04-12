//
//    コマンド解析器
//
//    Copyright (c) 2025 okawa yusuke
//


//
// # コマンド定義
//
// - 原点取り
//   home
//
// - 移動速度指定
//   speed 速度[mm/s]
//
// - 座標指定
//   go X座標[mm] Y座標[mm]
//
// - チョコ容器の移動
//   choco white/black Z座標[mm] inject/stop
//
//   inject の場合、目標 Z 座標まで移動してから射出
//   stop の場合、射出停止してから目標 Z 座標まで移動
//
// - 一時停止 本体のボタンを押すまで待機
//   pause
//
// - コマンド列消去 (デバッグ用)
//   clear
//
// - 送信例
//   pause                   # 本体のボタンを押すまで待機
//   home                    # 原点取り
//   speed 100               # 移動速度を 100 mm/s に指定
//   go 100 200              # 100 mm, 200 mm に移動
//
//   choco white 10 inject   # ホワイトチョコの容器をZ座標 10 mm 上に移動した後射出
//   go 300 400              # 300 mm, 400 mm に移動
//   choco white 20 stop     # ホワイトチョコの容器の射出を停止してからZ座標 20 mm 上に移動
//
//   choco black 10 inject   # ブラックチョコの容器をZ座標 10 mm 上に移動した後射出
//   go 100 200              # 100 mm, 200 mm に移動
//   choco black 20 stop     # ブラックチョコの容器の射出を停止してからZ座標 20 mm 上に移動
//

#pragma once

#include "type.hpp"

#include <optional>
#include <variant>

namespace choco
{

    namespace command_type
    {
        using go = vector2d;
        using speed = f64;

        struct pause{};
        struct home{};
        struct clear{};

        struct choco
        {
            f64 z;
            bool inject;
            choco_color color;
        };
    }    // namespace command_type

    /// @brief コマンド
    /// @note コマンドの種類によって使用されるメンバが異なるため、共用体を使用している
    using command = std::variant<
        command_type::go,       // type::go
        command_type::speed,    // type::speed
        command_type::home,     // type::home
        command_type::choco,    // type::choco
        command_type::pause,    // type::pause
        command_type::clear     // type::clear
        >;

    /// @brief コマンドを解析する
    /// @param string コマンド文字列
    /// @return プログラムで扱いやすい形のコマンド
    std::optional<command>
    parse_command(const std::string& string);

    /// @brief コマンドを文字列に変換する デバッグ用
    /// @param command コマンド
    /// @return 文字列
    std::string
    command_to_string(const command& command);

}    // namespace choco
