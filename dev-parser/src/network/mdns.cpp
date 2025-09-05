#include "mdns.hpp"

#include <LEAmDNS.h>

void mdns_begin(const char* hostname)
{
    MDNS.begin(hostname);
}

void mdns_update()
{
    MDNS.update();
}
