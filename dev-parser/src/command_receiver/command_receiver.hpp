#pragma once

#include "../network/wifi.hpp"
#include "../network/mdns.hpp"
#include "../network/http_server.hpp"

void command_receiver_begin()
{
    wifi_begin({
        AccessPoint{ "dokinchan", "kokokoko" },
        AccessPoint{ "localnet-5G", "kokokoko" },
    });

    mdns_begin("pico");    // http://pico.local

    http_server_begin();

    http_server_add_post_handler("/", [](std::string_view sv) -> std::string
                                 {
                                    Serial.write(sv.data(), sv.size());    // 生データを送る
                                    Serial.println();                      // 改行追加
                                    return "hoge";
                                });
}

void command_receiver_update()
{
    http_server_update();
    mdns_update();
}
