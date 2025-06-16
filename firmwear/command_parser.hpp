//
//    コマンド解析器
//
//    Copyright (c) 2025 okawa yusuke
//


//
// # コマンド定義  ([] は変数)
//
// - 原点取り
//   home
//
// - 移動
//   move [color(white/black)] [x(mm)] [y(mm)] [z(mm)] [speed(mm/s)] [choco emit(emit/stop)]
//
// - 一時停止 本体のボタンを押すまで待機
//   pause
//
// - 送信例
//   pause                     # 本体のボタンを押すまで待機
//   home                      # 原点取り
//   move white 0 0 0 20 stop  # 白チョコの座標を基準に、チョコを出さずに (0,0,0) へ 20m/s で移動
//   move white 9 9 9 20 emit  # チョコを出しながら (9,9,9) へ 20m/s で移動
//

#pragma once

#include "type.hpp"

#include <optional>
#include <variant>

namespace choco
{

    namespace command_type
    {

        struct immediate_tag{};  // 受信後即実行したい場合はこの型を継承する


        using go = vector2d;
        using speed = f64;

        struct pause{};
        struct home{};
        struct clear : immediate_tag{};

        struct dump : immediate_tag
        {
            enum class dump_type
            {
                all,
                current,
            } type;
        };

        struct choco
        {
            f64 z;
            bool inject;
            choco_color color;
        };
    }    // namespace command_type

    /// @brief コマンド
    using command = std::variant<
        command_type::go,
        command_type::speed,
        command_type::home,
        command_type::choco,
        command_type::pause,
        command_type::clear,
        command_type::dump
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
