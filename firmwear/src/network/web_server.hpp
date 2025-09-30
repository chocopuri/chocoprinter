#pragma once

#include <vector>
#include "../file/file.hpp"
#include "http_server.hpp"

class WebServer
{
    std::vector<file> content_files;

public:
    WebServer(std::vector<file>&& content_files)
        : content_files{ std::move(content_files) }
    {
    }

    void begin()
    {
        for (auto& file : content_files)
        {
            const auto handler = [&file]() -> HttpResponse
            {
                if (const auto all_line_opt = file.read_all_line())
                    return { 200, "text/html", *all_line_opt };
                else
                    return { 500, "text/html", "server internal error. \n file open failed." };
            };

            http_server_add_get_handler("/" + file.get_filename(), handler);
        }
    }
};
