#include "src/command_receiver/command_receiver.hpp"

void setup()
{
    command_receiver_begin();
}

void loop()
{
    command_receiver_update();
}
