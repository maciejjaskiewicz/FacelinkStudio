#pragma once
#include <memory>

#include "FaceModel.hpp"

namespace Fls
{
    class FlsDatabase;

    class FaceRepository
    {
    public:
        explicit FaceRepository(FlsDatabase& db);

        XN_NODISCARD std::vector<int> getFacesIds(int personId) const;

        XN_NODISCARD bool add(const std::shared_ptr<FaceModel>& faceModel) const;
        XN_NODISCARD bool remove(int faceId) const;
    private:
        FlsDatabase& mDb;
    };
}