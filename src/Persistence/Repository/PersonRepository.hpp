#pragma once
#include <memory>

#include "PersonModel.hpp"

namespace Fls
{
    class FlsDatabase;

    class PersonRepository
    {
    public:
        explicit PersonRepository(FlsDatabase& db);

        XN_NODISCARD std::shared_ptr<PersonModel> get(int personId) const;
        XN_NODISCARD std::vector<std::shared_ptr<PersonGridModel>> getGrid(const std::string& filter) const;

        XN_NODISCARD bool add(const std::shared_ptr<PersonModel>& personModel) const;
        XN_NODISCARD bool update(const std::shared_ptr<PersonModel>& personModel) const;
        XN_NODISCARD bool remove(int personId) const;
    private:
        FlsDatabase& mDb;
    };
}