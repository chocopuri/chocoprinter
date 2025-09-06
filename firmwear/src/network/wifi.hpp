#pragma once

#include <vector>


struct AccessPoint
{
    const char* ssid;
    const char* pass;
};


void wifi_begin(const std::vector<AccessPoint>& aps);
