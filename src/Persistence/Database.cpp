#include "Database.hpp"

#include "SqlStatement.hpp"

namespace Fls
{
    Database::Database(const std::string& dbPath)
        : mDbPath(dbPath)
    {
        open(dbPath);
    }

    void Database::open(const std::string& dbPath)
    {
        mDbPath = dbPath;

        sqlite3* ptr = nullptr;
        const auto status = sqlite3_open(dbPath.c_str(), &ptr);

        mDb.reset(ptr, DatabaseDeleter());

        if (status != SQLITE_OK)
        {
            throw std::exception(sqlite3_errmsg(mDb.get()));
        }
    }

    void Database::exec(const std::string& sqlStatement)
    {
        SqlStatement(*this, sqlStatement).exec();
    }

    bool Database::isOpen() const
    {
        return mDb != nullptr;
    }

    std::shared_ptr<sqlite3> Database::get() const
    {
        return mDb;
    }

    void Database::DatabaseDeleter::operator()(sqlite3* db) const
    {
        sqlite3_close(db);
    }
}