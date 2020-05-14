#pragma once

#include <Xenon/Core/Api.hpp>
#include <sqlite3.h>

#include <string>
#include <memory>
#include <vector>

namespace Fls
{
    class Database;

    class SqlStatement
    {
    public:
        SqlStatement(Database& db, std::string sqlStatement);
        ~SqlStatement();

        void exec();
        bool moveNext();

        void bindInt(uint32_t parameterId, const int& item);
        void bindString(uint32_t parameterId, const std::string& item);
        void bindBlob(uint32_t parameterId, const std::vector<uint8_t>& item);
        void bindStream(uint32_t parameterId, const std::ostringstream& item);

        XN_NODISCARD std::string getColumnName(uint32_t idx) const;

        XN_NODISCARD int getColumnAsInt(uint32_t idx) const;
        XN_NODISCARD std::string getColumnAsText(uint32_t idx) const;
        XN_NODISCARD std::vector<uint8_t> getColumnAsBlob(uint32_t idx) const;
        XN_NODISCARD std::istringstream getColumnAsStream(uint32_t idx) const;

        XN_NODISCARD uint32_t columns() const;
        XN_NODISCARD const std::string& sqlString() const;

        SqlStatement(const SqlStatement&) = delete;
        const SqlStatement& operator=(const SqlStatement&) = delete;
    private:
        void reset();

        bool mNeedsReset{ false };
        int mStepStatus{ SQLITE_DONE };
        bool mAtFirstStep{ true };

        std::shared_ptr<sqlite3> mDb;
        sqlite3_stmt* mStmt{ nullptr };
        std::string mSqlString;
    };
}