//
// json を直接いじるのは面倒なので、あるデータ構造からjsonに変換、またその逆を行うラッパーを定義する
//

#pragma once

#include <vector>

#include "json_store.hpp"

// expect json:

// {
//     "wifi_config" [
//         {
//             "ssid": "aaa",
//             "pass": "aaapas",
//         },
//         {
//             "ssid": "bbb",
//             "pass": "bbbpas",
//         },
//         {
//             "ssid": "ccc",
//             "pass": "cccpas",
//         }
//     ],
//     ...
// }


struct wifi_config {
    std::string ssid;
    std::string pass;
};

inline std::vector<wifi_config> get_all_wifi_config(json_store& store)
{
    if (not store.get_storage()["wifi_config"].is<JsonArray>())
        return {};

    std::vector<wifi_config> configs;

    const JsonArray conf_elements = store.get_storage()["wifi_config"];
    for (auto&& conf_element : conf_elements)
    {
        if (not conf_element["ssid"].is<std::string>() || not conf_element["pass"].is<std::string>())
            continue;

        wifi_config conf;

        conf.ssid = conf_element["ssid"].as<std::string>();
        conf.pass = conf_element["pass"].as<std::string>();

        configs.push_back(std::move(conf));
    }

    return configs;
}

enum class config_update_state
{
    add_new_config,
    password_update,
};

config_update_state add_or_update_config(json_store& store, wifi_config&& new_config)
{

    // 
    // if (not store.get_storage()["wifi_config"].is<JsonArray>())
    // {
    //     store.get_storage().to<JsonArray>();
    // }


    JsonArray conf_elements = store.get_storage()["wifi_config"];
    for (auto&& conf_element : conf_elements)
    {
        if (not conf_element["ssid"].is<std::string>() || not conf_element["pass"].is<std::string>())
            continue;

        // 登録済みのSSIDの場合パスワードを更新する
        if (conf_element["ssid"].as<std::string>() == new_config.ssid)
        {
            conf_element["pass"] = new_config.pass;
            return config_update_state::password_update;
        }
    }

    JsonObject new_entry = conf_elements.add<JsonObject>();
    new_entry["ssid"] = new_config.ssid;
    new_entry["pass"] = new_config.pass;

    return config_update_state::add_new_config;
}
