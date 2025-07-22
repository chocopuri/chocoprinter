#pragma once

#include <ArduinoJson.h>
#include "file.hpp"

class json_store
{
    JsonDocument doc;
    file config_file;
public:

    json_store(std::string&& file_name)
        : config_file{ std::move(file_name) }
    {
    }

    bool begin()
    {
        const auto json_string = config_file.read_all_line();
        if (not json_string)
            return false;

        DeserializationError error = deserializeJson(doc, *json_string);
        if (error)
            return false;
        else
            return true;
    }

    JsonDocument& get_storage()
    {
        return doc;
    }

    void save()
    {
        std::string json_string;
        serializeJson(doc, json_string);
        config_file.overwrite(json_string);
    }

    void dump() const
    {
        serializeJson(doc, Serial);
    }

    void dump_file() const
    {
        if (const auto json_string = config_file.read_all_line())
        {
            Serial.println(json_string->c_str());
        }
        else
        {
            Serial.println("file read error");
        }
    }

};
