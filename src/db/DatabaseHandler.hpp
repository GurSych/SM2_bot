#ifndef SM2_BOT_SRC_DB_DATABASEHANDLER_HPP
#define SM2_BOT_SRC_DB_DATABASEHANDLER_HPP
#pragma once

#include <string>
#include <sqlite_orm/sqlite_orm.h>
namespace sql = sqlite_orm;

namespace db {
    inline auto init_storage(const std::string& path) {
        return sql::make_storage(path
            // Boo~!
        );
    }

    using sql_storage_t = decltype(init_storage(""));

    class DatabaseHandler {
    public:
        DatabaseHandler(const std::string& db_path)
            : db_path_{db_path}, storage_{init_storage(db_path_)} {}
        ~DatabaseHandler() = default;

        sql_storage_t&     get_storage()       { return storage_; }
        const std::string& get_db_path() const { return db_path_; }

        // Boo~!

    private:
        std::string db_path_;
        sql_storage_t storage_;
    };
}

#endif // SM2_BOT_SRC_DB_DATABASEHANDLER_HPP
