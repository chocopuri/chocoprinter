#pragma once

#include <string>
#include <optional>
#include <vector>

class command_receiver
{
    virtual std::optional<std::vector<std::string>> receive_all_text_async() = 0;
};
