#pragma once

namespace Fls
{
    class Database;

    class DatabaseTransaction
    {
    public:
        explicit DatabaseTransaction(Database& db);
        ~DatabaseTransaction() noexcept(false);

        void commit();

        DatabaseTransaction(const DatabaseTransaction&) = delete;
        const DatabaseTransaction& operator=(const DatabaseTransaction&) = delete;
    private:
        Database& mDb;
        bool mCommitted{ false };
    };
}
