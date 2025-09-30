#pragma once

#include <vector>
#include <string>

#include "wifi.hpp"
#include "../file/file.hpp"

/// @brief アクセスポイント設定ファイルを読み込む
/// @param config_file 設定ファイル (1行に1つ、SSID,パスワード の形式で記述)
/// @return 読み込んだアクセスポイント設定
inline std::vector<AccessPoint> load_access_point_configs(file&& config_file)
{
    std::vector<AccessPoint> aps{};

    if (const auto all_line_opt = config_file.read_all_line())
    {
        std::istringstream iss{ *all_line_opt };
        std::string line;

        while (std::getline(iss, line))
        {
            const auto comma_pos = line.find(',');
            if (comma_pos == std::string::npos)
                continue;

            const auto ssid = line.substr(0, comma_pos);
            const auto pass = line.substr(comma_pos + 1);

            if (ssid.size() == 0 || pass.size() == 0)
                continue;

            aps.push_back(AccessPoint{ ssid.c_str(), pass.c_str() });
        }
    }

    return aps;
}
