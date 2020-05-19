#pragma once

#include "Database.hpp"

#include "Repository/PersonRepository.hpp"
#include "Repository/FaceRepository.hpp"

namespace Fls
{
    class FlsDatabase final : public Database
    {
    public:
        explicit FlsDatabase();
        explicit FlsDatabase(const std::string& dbPath);

        void ensureCreated() override;

        XN_NODISCARD PersonRepository& personRepository() const;
        XN_NODISCARD FaceRepository& faceRepository() const;
    private:
        void initRepositories();

        void createPersonTable();
        void createClassifierTable();
        void createFaceTable();
        void createPersonView();

        std::unique_ptr<PersonRepository> mPersonRepository;
        std::unique_ptr<FaceRepository> mFaceRepository;
    };
}
