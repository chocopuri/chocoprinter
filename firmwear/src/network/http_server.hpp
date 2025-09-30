#pragma once

#include <string>
#include <string_view>
#include <functional>

struct HttpResponse
{
    int code;
    std::string content_type;
    std::string content;
};

void http_server_begin();

// client -> self
void http_server_add_post_handler(std::string&& endpoint, std::function<HttpResponse(std::string_view)>&& post_handler);

// self -> client
void http_server_add_get_handler(std::string&& endpoint, std::function<HttpResponse()>&& get_handler);

void http_server_update();
