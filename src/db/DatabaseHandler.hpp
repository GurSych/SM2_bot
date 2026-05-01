#ifndef SM2_BOT_SRC_DB_DATABASEHANDLER_HPP
#define SM2_BOT_SRC_DB_DATABASEHANDLER_HPP
#pragma once

#include <string>
#include <sqlite_orm/sqlite_orm.h>
namespace sql = sqlite_orm;

#include "ORMClasses.hpp"

namespace db {
    inline auto init_storage(const std::string& path) {
        return sql::make_storage(path,
            sql::make_table("users",
                sql::make_column("id",          &orm::User::id,          sql::primary_key().autoincrement()),
                sql::make_column("telegram_id", &orm::User::telegram_id, sql::unique()                     ),
                sql::make_column("username",    &orm::User::username                                       ),
                sql::make_column("language",    &orm::User::language,    sql::default_value("en")          ),
                sql::make_column("status",      &orm::User::status,      sql::default_value("user")        ),
                sql::make_column("created_at",  &orm::User::created_at                                     )
            )
        );
    }

    using sql_storage_t = decltype(init_storage(""));

    class DatabaseHandler {
    public:
        DatabaseHandler(const std::string& db_path);
        ~DatabaseHandler() = default;

        void sync_storage() { storage_.sync_schema(); }

        sql_storage_t&     get_storage()       { return storage_; }
        const std::string& get_db_path() const { return db_path_; }

    private:
        std::string db_path_;
        sql_storage_t storage_;
    };
}

#endif // SM2_BOT_SRC_DB_DATABASEHANDLER_HPP
