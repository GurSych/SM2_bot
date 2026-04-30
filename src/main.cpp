#include <iostream>
#include "tg/TelegramHandler.hpp"

int main() {
    std::clog << "The program starting..." << std::endl;

    tg::TelegramHandler handler(std::getenv("TG_BOT_TOKEN"));

    handler.run_polling();

    return 0;
}
