#include "SqlStatement.hpp"

#include "Database.hpp"

namespace Fls
{
    SqlStatement::SqlStatement(Database& db, std::string sqlStatement)
        : mDb(db.get()), mSqlString(std::move(sqlStatement))
    {
        const auto size = static_cast<int>(mSqlString.size() + 1);
        const auto status = sqlite3_prepare_v2(mDb.get(), mSqlString.c_str(), size, &mStmt, nullptr);

        if (status != SQLITE_OK)
        {
            sqlite3_finalize(mStmt);
            throw std::exception(sqlite3_errmsg(mDb.get()));
        }

        if (mStmt == nullptr)
        {
            throw std::exception("Invalid SQL statement");
        }
    }

    SqlStatement::~SqlStatement()
    {
        sqlite3_finalize(mStmt);
    }

    void SqlStatement::exec()
    {
        reset();

        mStepStatus = sqlite3_step(mStmt);
        mNeedsReset = true;

        if (mStepStatus != SQLITE_DONE && mStepStatus != SQLITE_ROW)
        {
            if (mStepStatus == SQLITE_ERROR)
                throw std::exception(sqlite3_errmsg(mDb.get()));
            if (mStepStatus == SQLITE_BUSY)
                throw std::exception("statement::exec() failed.  SQLITE_BUSY returned");

            throw std::exception("statement::exec() failed.");
        }
    }

    bool SqlStatement::moveNext()
    {
        if (mStepStatus == SQLITE_ROW)
        {
            if (mAtFirstStep)
            {
                mAtFirstStep = false;
                return true;
            }

            mStepStatus = sqlite3_step(mStmt);

            if (mStepStatus == SQLITE_DONE) return false;
            if (mStepStatus == SQLITE_ROW) return true;

            throw std::exception(sqlite3_errmsg(mDb.get()));
        }

        return false;
    }

    void SqlStatement::bindInt(const uint32_t parameterId, const int& item)
    {
        reset();

        const auto status = sqlite3_bind_int(mStmt, parameterId, item);

        if (status != SQLITE_OK)
        {
            throw std::exception(sqlite3_errmsg(mDb.get()));
        }
    }

    void SqlStatement::bindString(const uint32_t parameterId, const std::string& item)
    {
        reset();

        const auto status = sqlite3_bind_text(mStmt, parameterId, item.c_str(), -1, SQLITE_TRANSIENT);

        if (status != SQLITE_OK)
        {
            throw std::exception(sqlite3_errmsg(mDb.get()));
        }
    }

    void SqlStatement::bindBlob(const uint32_t parameterId, const std::vector<uint8_t>& item)
    {
        reset();

        const auto status = sqlite3_bind_blob(mStmt, parameterId, &item[0], 
            static_cast<int>(item.size()), SQLITE_TRANSIENT);

        if (status != SQLITE_OK)
        {
            throw std::exception(sqlite3_errmsg(mDb.get()));
        }
    }

    void SqlStatement::bindStream(const uint32_t parameterId, const std::ostringstream& item)
    {
        reset();

        const auto& str = item.str();
        const auto status = sqlite3_bind_blob(mStmt, parameterId, str.data(), 
            static_cast<int>(str.size()), SQLITE_TRANSIENT);

        if (status != SQLITE_OK)
        {
            throw std::exception(sqlite3_errmsg(mDb.get()));
        }
    }

    void SqlStatement::bindNull(const uint32_t parameterId)
    {
        reset();

        const auto status = sqlite3_bind_null(mStmt, parameterId);

        if (status != SQLITE_OK)
        {
            throw std::exception(sqlite3_errmsg(mDb.get()));
        }
    }

    std::string SqlStatement::getColumnName(const uint32_t idx) const
    {
        return std::string(sqlite3_column_name(mStmt, idx));
    }

    int SqlStatement::getColumnAsInt(const uint32_t idx) const
    {
        return sqlite3_column_int(mStmt, idx);
    }

    std::string SqlStatement::getColumnAsText(const uint32_t idx) const
    {
        const auto data = reinterpret_cast<const char*>(sqlite3_column_text(mStmt, idx));

        if (data != nullptr)
            return std::string(data);

        return std::string();
    }

    std::vector<uint8_t> SqlStatement::getColumnAsBlob(const uint32_t idx) const
    {
        const auto* data = static_cast<const uint8_t*>(sqlite3_column_blob(mStmt, idx));
        const auto size = sqlite3_column_bytes(mStmt, idx);

        return std::vector<uint8_t>(data, data + size);
    }

    std::istringstream SqlStatement::getColumnAsStream(const uint32_t idx) const
    {
        const auto data = static_cast<const char*>(sqlite3_column_blob(mStmt, idx));
        const auto size = sqlite3_column_bytes(mStmt, idx);

        return std::istringstream(std::string(data, size));
    }

    uint32_t SqlStatement::columns() const
    {
        if ((mAtFirstStep == false) && (mStepStatus == SQLITE_ROW))
        {
            return sqlite3_column_count(mStmt);
        }

        return 0;
    }

    int SqlStatement::lastInsertedId() const
    {
        return static_cast<int>(sqlite3_last_insert_rowid(mDb.get()));
    }

    const std::string& SqlStatement::sqlString() const
    {
        return mSqlString;
    }

    void SqlStatement::reset()
    {
        if (mNeedsReset)
        {
            if (sqlite3_reset(mStmt) != SQLITE_OK)
            {
                mStepStatus = SQLITE_DONE;
                throw std::exception(sqlite3_errmsg(mDb.get()));
            }

            mNeedsReset = false;
            mStepStatus = SQLITE_DONE;
            mAtFirstStep = true;
        }
    }
}