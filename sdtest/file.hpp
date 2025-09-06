#pragma once

#include <string>
#include <optional>

#include <Arduino.h>

bool sd_card_begin(uint8_t chip_select_pin = PICO_DEFAULT_SPI_CSN_PIN);

class file
{
    std::string filename;

public:
    file(std::string&& filename);

    /// @brief 全行読み出し
    std::optional<std::string> read_all_line() const;

    /// @brief 上書き
    bool overwrite(const std::string& new_string);

    /// @brief このファイルが存在するかどうか
    bool exists() const;
};
