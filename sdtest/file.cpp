//
//   SDカードのライブラリがドでかいので分割
//
//   ほかのヘッダーにSDカードライブラリがインクルードがされないように、このソースファイル
//   に封じ込め、コンパイル時間短縮を図った。実際速くなり快適
//

#include "file.hpp"

#include <SPI.h>
#include <SD.h>


bool sd_card_begin(uint8_t chip_select_pin)
{
    return SD.begin(chip_select_pin);
}

file::file(std::string&& filename)
    : filename{ std::move(filename) }
{}

std::optional<std::string> file::read_all_line() const
{
    File file = SD.open(filename.c_str(), FILE_READ);
    if (not file)
        return std::nullopt;

    std::string all_line;

    while (file.available())
    {
        char buffer[256];
        const size_t read_size = file.read((unsigned char*)buffer, sizeof buffer);
        all_line.append(buffer, read_size);
    }

    file.close();

    return all_line;
}

bool file::overwrite(const std::string& new_string)
{
    SD.remove(filename.c_str());

    File file = SD.open(filename.c_str(), FILE_WRITE);
    if (not file)
        return false;

    const size_t writed_size = file.write(new_string.data(), new_string.size());
    if (writed_size != new_string.size())
    {
        file.close();
        return false;
    }

    file.close();

    return true;
}

bool file::exists() const
{
    return SD.exists(filename.c_str());
}
