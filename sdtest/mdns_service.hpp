#pragma once

#include <string>

class mdns_service
{
    std::string hostname;

public:
    mdns_service(std::string&& hostname);

    void begin();

    void update();
};
