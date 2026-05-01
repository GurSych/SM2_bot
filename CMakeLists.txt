cmake_minimum_required(VERSION 3.20)
project(sm2_bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TG_BOT_TOKEN)

set(APP_VERSION_MAJOR "0"        )
set(APP_VERSION_MINOR "0"        )
set(APP_VERSION_PATCH "1"        )
set(APP_VERSION_PRTAG "Pre-Alpha")
set(APP_VERSION_prtag "pre-alpha")

string(TIMESTAMP CMAKE_BUILD_TIME "%Y-%m-%d")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/config.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/config.hpp @ONLY)

include(FetchContent)

FetchContent_Declare(TgBot
  GIT_REPOSITORY https://github.com/reo7sp/tgbot-cpp.git
  GIT_TAG        v1.10
)
FetchContent_MakeAvailable(TgBot)

FetchContent_Declare(sqlite_orm
  GIT_REPOSITORY https://github.com/fnc12/sqlite_orm.git
  GIT_TAG        v1.9.1
)
FetchContent_MakeAvailable(sqlite_orm)

add_executable(sm2_bot
  src/main.cpp
  src/tg/TelegramHandler.cpp
  src/tg/LanguageHandler.cpp
  src/db/DatabaseHandler.cpp
)
set_target_properties(sm2_bot PROPERTIES OUTPUT_NAME "sm2_bot")

FetchContent_Declare(json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(json)

target_include_directories(sm2_bot PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(sm2_bot PRIVATE ${CMAKE_BINARY_DIR}/include        )

target_link_libraries(sm2_bot PRIVATE TgBot                       )
target_link_libraries(sm2_bot PRIVATE sqlite_orm::sqlite_orm      )
target_link_libraries(sm2_bot PRIVATE nlohmann_json::nlohmann_json)
