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
        explicit Database() = default;
        explicit Database(const std::string& dbPath);
        virtual ~Database() = default;

        void open(const std::string& dbPath);
        virtual void exec(const std::string& sqlStatement);
        virtual void ensureCreated() = 0;

        XN_NODISCARD bool isOpen() const;
        XN_NODISCARD bool tableExists(const std::string& tableName);
        XN_NODISCARD std::shared_ptr<sqlite3> get() const;

        Database(const Database&) = delete;
        const Database& operator=(const Database&) = delete;
    protected:
        std::string mDbPath;
        std::shared_ptr<sqlite3> mDb;

        struct DatabaseDeleter
        {
            void operator()(sqlite3* db) const;
        };
    };
}