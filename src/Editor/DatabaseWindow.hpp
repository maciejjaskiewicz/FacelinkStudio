#pragma once

#include "Persistence/Repository/PersonModel.hpp"

#include <Xenon/Graphics.hpp>

namespace Fls
{
    class DatabaseWindow
    {
    public:
        explicit DatabaseWindow();

        void init();

        void updateGui(const Xenon::DeltaTime& deltaTime);
    private:
        void mainView();
        void detailView();

        bool mWindowActive{ false };

        bool mNeedsGridRefresh{ true };
        std::vector<std::shared_ptr<PersonGridModel>> mGridData;
        std::shared_ptr<PersonModel> mPersonData;

        bool mDetailView{ false };
        int mDetailViewPersonId{ -1 };
        std::string mSearchBuffer;
    };
}
