#pragma once

#include <Xenon/Core/Api.hpp>
#include <sqlite3.h>

#include <string>
#include <memory>

namespace Fls
{
    class Database
    {
    public:
        Database() = default;
        Database(const std::string& dbPath);

        void open(const std::string& dbPath);
        void exec(const std::string& sqlStatement);

        XN_NODISCARD bool isOpen() const;
        XN_NODISCARD std::shared_ptr<sqlite3> get() const;

        Database(const Database&) = delete;
        const Database& operator=(const Database&) = delete;
    private:
        std::string mDbPath;
        std::shared_ptr<sqlite3> mDb;

        struct DatabaseDeleter
        {
            void operator()(sqlite3* db) const;
        };
    };
}