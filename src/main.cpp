#include <iostream>
#include "db/DatabaseHandler.hpp"
#include "tg/LanguageHandler.hpp"
#include "tg/TelegramHandler.hpp"

int main() {
    std::clog << "The program starting..." << std::endl;

    db::DatabaseHandler db(std::getenv("DB_PATH"));
    db.sync_storage();
    tg::LanguageHandler lang_handler{};
    lang_handler.load_dictionary("locale/en.json");
    lang_handler.load_dictionary("locale/ru.json");
    tg::TelegramHandler handler(std::getenv("TG_BOT_TOKEN"), db, lang_handler);

    std::clog << "Program started successfully." << std::endl;

    handler.run_polling();

    return 0;
}
