#include "DatabaseHandler.hpp"

db::DatabaseHandler::DatabaseHandler(const std::string& db_path)
    : db_path_{db_path}, storage_{init_storage(db_path_)} {
    sync_storage();
}

orm::User db::DatabaseHandler::get_user_by_id(uint64_t id) {
    return storage_.get<orm::User>(id);
}
