#include "FlsDatabase.hpp"
#include "SqlStatement.hpp"

namespace Fls
{
    FlsDatabase::FlsDatabase(const std::string& dbPath)
        : Database(dbPath)
    { }

    void FlsDatabase::ensureCreated()
    {
        XN_ASSERT(isOpen());

        if (!tableExists("Person")) createPersonTable();
        if (!tableExists("Classifier")) createClassifierTable();
        if (!tableExists("Face")) createFaceTable();
    }

    void FlsDatabase::createPersonTable()
    {
        constexpr auto createSql = R"(
            CREATE TABLE Person (
                Id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
                Name TEXT NOT NULL,
                Birthdate DATE,
                Note TEXT,
                CreatedDate DATETIME NOT NULL
            );
        )";

        exec(createSql);
    }

    void FlsDatabase::createClassifierTable()
    {
        constexpr auto createSql = R"(
            CREATE TABLE Classifier (
                Id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
                Data BLOB NOT NULL,
                Dirty BOOLEAN NOT NULL,
                CreatedDate DATETIME NOT NULL
            );
        )";

        exec(createSql);
    }

    void FlsDatabase::createFaceTable()
    {
        constexpr auto createSql = R"(
            CREATE TABLE Face (
                Id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
                OwnerId INTEGER NOT NULL,
                Embeddings BLOB NOT NULL,
                Image BLOB NOT NULL,
                ClassifierId INTEGER,
                CreatedDate DATETIME NOT NULL,
                FOREIGN KEY(OwnerId) REFERENCES Person(Id),
                FOREIGN KEY(ClassifierId) REFERENCES Classifier(Id)
            );
        )";

        exec(createSql);
    }
}
