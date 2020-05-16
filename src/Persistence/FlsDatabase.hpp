#pragma once

#include "Database.hpp"

namespace Fls
{
    class FlsDatabase final : public Database
    {
    public:
        explicit FlsDatabase() = default;
        explicit FlsDatabase(const std::string& dbPath);

        void ensureCreated() override;
    private:
        void createPersonTable();
        void createClassifierTable();
        void createFaceTable();
    };
}
