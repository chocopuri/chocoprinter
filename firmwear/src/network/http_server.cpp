#include "http_server.hpp"

#include <Arduino.h>
#include <WebServer.h>

static WebServer g_server(80);

void http_server_begin()
{
    g_server.begin();
}

void http_server_add_post_handler(const std::string& endpoint, std::function<HttpResponse(std::string_view)>&& post_handler)
{
    g_server.on(String{ endpoint.c_str() }, HTTP_POST, [=]()
                {
                    String body = g_server.arg("plain");
                    HttpResponse res = post_handler(body.c_str());
                    
                    Serial.println("[ OK ] Handle http post request");
                    Serial.println(endpoint.c_str());
                    Serial.println(body);
                    Serial.println("[ OK ] response:");
                    Serial.println(res.content.c_str());
                    
                    g_server.send(res.code, res.content_type.c_str(), res.content.c_str());
                });

    // OPTIONS のハンドラ（プリフライト用）
    g_server.on(String{ endpoint.c_str() }, HTTP_OPTIONS, [=]() {
        g_server.sendHeader("Access-Control-Allow-Origin", "*");
        g_server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        g_server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        g_server.send(204); // No Content
    });
}

void http_server_add_get_handler(const std::string& endpoint, std::function<HttpResponse()>&& get_handler)
{
    g_server.on(String{ endpoint.c_str() }, HTTP_GET, [=]()
                {
                    HttpResponse res = get_handler();
                    
                    Serial.println("[ OK ] Handle http get request");
                    Serial.println(endpoint.c_str());
                    Serial.println("[ OK ] response:");
                    Serial.println(res.content.c_str());

                    g_server.send(res.code, res.content_type.c_str(), res.content.c_str());
                });
}

void http_server_update()
{
    g_server.handleClient();
}
