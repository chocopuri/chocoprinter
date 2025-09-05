#include "http_server.hpp"

#include <Arduino.h>
#include <WebServer.h>

static WebServer g_server(80);

void http_server_begin()
{
    g_server.begin();
}

void http_server_add_post_handler(const char* endpoint, std::function<std::string(std::string_view)>&& post_handler)
{
    g_server.on(endpoint, HTTP_POST, [&post_handler]()
                {
                    String body = g_server.arg("plain");
                    std::string res = post_handler(body.c_str());
                    g_server.send(200, "application/json", res.data());
                });
}

void http_server_add_get_handler(const char* endpoint, std::function<std::string()>&& get_handler)
{
    g_server.on(endpoint, HTTP_GET, [&get_handler]()
                {
                    std::string res = get_handler();
                    g_server.send(200, "application/json", res.data());
                });
}

void http_server_update()
{
    g_server.handleClient();
}
