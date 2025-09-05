#pragma once

#include <string>
#include <string_view>
#include <functional>

void http_server_begin();

// client -> self
void http_server_add_post_handler(const char* endpoint, std::function<std::string(std::string_view)>&& post_handler);

// self -> client
void http_server_add_get_handler(const char* endpoint, std::function<std::string()>&& get_handler);

void http_server_update();
