#include "mdns_service.hpp"

#include <LEAmDNS.h>

mdns_service::mdns_service(std::string&& hostname)
    : hostname{ std::move(hostname) }
{}

void mdns_service::begin()
{
    MDNS.begin(hostname.c_str());
}

void mdns_service::update()
{
    MDNS.update();
}
