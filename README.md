# SM2_bot

**SM2 Bot is a telegram bot that uses a SuperMemo algorithm to help users memorize foreign words**

# Using the program

> Project is in development so this section is updated together with project itself

# Tech stack and dependencies

- C++23
- SQLite
- reo7sp/tgbot-cpp library (for Telegram bot)
- fnc12/sqlite_orm library (for SQLite database)
- CMake

## Project structure:

    ├─ src/                            Program source files
    │  ├─ db/                          Database handling files
    │  │  └─ ...
    │  ├─ sm/                          SuperMemo handling files
    │  │  └─ ...
    │  ├─ tg/                          Telegram bot handling files
    │  │  └─ ...
    │  ├─ tools/                       Tools files
    │  │  └─ ...
    │  ├─ .env                         Environment file
    │  ├─ config.hpp.in                Config defines header file
    │  └─ main.cpp                     Main program file
    └─ CMakeLists.txt                  CMake configuration file
