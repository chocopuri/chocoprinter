#pragma once

#include <vector>
#include <string>

#include "wifi.hpp"
#include "../file/file.hpp"

inline std::vector<AccessPoint> load_access_point_configs(const file& access_point_file)
{
    std::vector<AccessPoint> aps{};

    if (const auto all_line_opt = access_point_file.read_all_line())
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
