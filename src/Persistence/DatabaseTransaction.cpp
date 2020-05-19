#include "DatabaseTransaction.hpp"

#include "Database.hpp"

namespace Fls
{
    DatabaseTransaction::DatabaseTransaction(Database& db)
        : mDb(db)
    {
        mDb.exec("BEGIN TRANSACTION");
    }

    DatabaseTransaction::~DatabaseTransaction() noexcept(false)
    {
        if(!mCommitted)
        {
            mDb.exec("ROLLBACK");
        }
    }

    void DatabaseTransaction::commit()
    {
        if(!mCommitted)
        {
            mCommitted = true;
            mDb.exec("COMMIT");
        }
    }
}