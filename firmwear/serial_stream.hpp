#pragma once

#include <ostream>

namespace choco
{
    class serial_stream
        : std::streambuf
    {
    public:
        serial_stream()
        {}

        std::streamsize xsgetn(char* s, std::streamsize n) override
        {   
            
        }
    };
}    // namespace choco