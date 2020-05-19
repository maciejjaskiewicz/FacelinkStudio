#include "DatabaseWindow.hpp"

#include "FacelinkStudioServices.hpp"
#include "Events/FaceSelectedEvent.hpp"
#include "Events/OpenDatabaseEvent.hpp"
#include "Events/RetrainFaceClassifierEvent.hpp"

#include <Xenon/Core/ApplicationServices.hpp>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <imgui_internal.h>
#include <ImGuiFileDialogIcons.hpp>

namespace Fls
{
    DatabaseWindow::DatabaseWindow()
    {
        mPersonData = std::make_shared<PersonModel>();
    }

    void DatabaseWindow::init()
    {
        Xenon::ApplicationServices::EventBus::ref().subscribe<OpenDatabaseEvent>(
            [this](const OpenDatabaseEvent& event)
        {
            mWindowActive = true;
        });

        Xenon::ApplicationServices::EventBus::ref().subscribe<FaceSelectedEvent>(
        [this](const FaceSelectedEvent& event)
        {
            if (!mDetailView)
                return;

            event.faceModel->id = -1;
            mPersonData->faces.push_back(event.faceModel);
        });
    }

    void DatabaseWindow::updateGui(const Xenon::DeltaTime& deltaTime)
    {
        if(mNeedsGridRefresh)
        {
            mGridData = FlsServices::Database::ref().personRepository().getGrid(mSearchBuffer);
            mNeedsGridRefresh = false;
        }

        if(mDetailViewPersonId != -1 && mDetailViewPersonId != mPersonData->id)
        {
            mPersonData = FlsServices::Database::ref().personRepository().get(mDetailViewPersonId);
        }

        const auto viewport = static_cast<ImGuiViewportP*>(ImGui::GetMainViewport());

        ImGui::SetNextWindowPos(viewport->GetMainRect().GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x / 2, viewport->Size.y / 2), ImGuiCond_Appearing);

        if(mWindowActive && ImGui::Begin("Database", &mWindowActive))
        {
            if (mDetailView)
                detailView();
            else
                mainView();

            ImGui::End();
        }
    }

    void DatabaseWindow::mainView()
    {
        static auto rowMinHeight = 20.0f;

        if (ImGui::Button(ICON_IMFDLG_ADD "##DatabaseAddNewPerson"))
        {
            mDetailViewPersonId = -1;
            mPersonData = std::make_shared<PersonModel>();
            mDetailView = true;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Add Person");

        ImGui::SameLine();

        if (ImGui::Button(ICON_IMFDLG_RESET "##DatabaseSearchFiledReset"))
        {
            mSearchBuffer = "";
            mNeedsGridRefresh = true;
        }

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Reset searching");

        ImGui::SameLine();
        ImGui::Text(ICON_IMFDLG_SEARCH);
        ImGui::SameLine();

        if (ImGui::InputText("##DatabaseSearchFiled", &mSearchBuffer))
        {
            mNeedsGridRefresh = true;
        }

        ImGui::SameLine();
        if(ImGui::Button("Retrain Face Classifier"))
            ImGui::OpenPopup("Retrain Face Classifier");


        if (ImGui::BeginPopupModal("Retrain Face Classifier", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure you want to retrain\nFace Classifier?\n\n");
            ImGui::Separator();

            if (ImGui::Button("Yes", ImVec2(120, 0)))
            {
                Xenon::ApplicationServices::EventBus::ref().enqueue<RetrainFaceClassifierEvent>();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("No", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders
            | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
            | ImGuiTableFlags_ScrollFreezeTopRow | ImGuiTableFlags_ScrollFreezeLeftColumn
            | ImGuiTableFlags_SizingPolicyFixedX;

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        if (ImGui::BeginTable("##dbTable", 7, flags))
        {
            ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed, -1.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, -1.0f);
            ImGui::TableSetupColumn("Birthdate", ImGuiTableColumnFlags_WidthFixed, -1.0f);
            ImGui::TableSetupColumn("Note", ImGuiTableColumnFlags_WidthStretch, -1.0f);
            ImGui::TableSetupColumn("Faces", ImGuiTableColumnFlags_WidthFixed, -1.0f);
            ImGui::TableSetupColumn("Created Date", ImGuiTableColumnFlags_WidthFixed, -1.0f);
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, -1.0f);

            ImGui::TableAutoHeaders();

            constexpr auto emptyBirthdate = "----------------";

            static ImVector<int> selection;
            ImGuiListClipper clipper;
            clipper.Begin(mGridData.size());

            while (clipper.Step())
            {
                for (auto rowN = clipper.DisplayStart; rowN < clipper.DisplayEnd; rowN++)
                {
                    const auto item = mGridData[rowN];

                    const auto itemIsSelected = selection.contains(item->id);
                    ImGui::PushID(item->id);
                    ImGui::TableNextRow(ImGuiTableRowFlags_None, rowMinHeight);

                    auto label = std::to_string(item->id);

                    if (ImGui::Selectable(label.c_str(), itemIsSelected, ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap, ImVec2(0, rowMinHeight)))
                    {
                        if (ImGui::GetIO().KeyCtrl)
                        {
                            if (itemIsSelected)
                                selection.find_erase_unsorted(item->id);
                            else
                                selection.push_back(item->id);
                        }
                        else
                        {
                            selection.clear();
                            selection.push_back(item->id);
                        }
                    }

                    ImGui::TableNextCell();
                    ImGui::TextUnformatted(item->name.c_str());

                    ImGui::TableNextCell();
                    ImGui::TextUnformatted(item->birthdate.empty() ? emptyBirthdate : item->birthdate.c_str());

                    ImGui::TableNextCell();
                    ImGui::TextUnformatted(item->note.c_str());

                    ImGui::TableNextCell();
                    ImGui::Text("%d", item->faces);

                    ImGui::TableNextCell();
                    ImGui::TextUnformatted(item->createdDate.c_str());

                    ImGui::TableNextCell();
                    ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(4 / 7.0f, 0.6f, 0.6f)));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(4 / 7.0f, 0.7f, 0.7f)));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(4 / 7.0f, 0.8f, 0.8f)));
                    if (ImGui::Button("Details"))
                    {
                        mDetailView = true;
                        mDetailViewPersonId = item->id;
                    }
                    ImGui::SameLine();
                    ImGui::PopStyleColor(3);

                    ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0 / 7.0f, 0.6f, 0.6f)));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0 / 7.0f, 0.7f, 0.7f)));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0 / 7.0f, 0.8f, 0.8f)));
                    if (ImGui::Button("Delete"))
                        ImGui::OpenPopup("Delete Person");
                    ImGui::PopStyleColor(3);

                    if (ImGui::BeginPopupModal("Delete Person", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                    {
                        ImGui::Text("Are you sure you want to delete this\nperson from database?\n\n");
                        ImGui::Separator();

                        if (ImGui::Button("Yes", ImVec2(120, 0)))
                        {
                            const auto deleteStatus = FlsServices::Database::ref().personRepository().remove(item->id);

                            if (!deleteStatus) XN_ERROR("Failed to delete person")

                            mNeedsGridRefresh = true;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SetItemDefaultFocus();
                        ImGui::SameLine();
                        if (ImGui::Button("No", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                        ImGui::EndPopup();
                    }

                    ImGui::PopID();
                }
            }

            ImGui::EndTable();
        }
    }

    void DatabaseWindow::detailView()
    {
        if (ImGui::Button(ICON_IMFDLG_RESET "   Back"))
        {
            mDetailView = false;
            mPersonData = std::make_shared<PersonModel>();
        }
        ImGui::SameLine();

        if (ImGui::Button(ICON_IMFDLG_OK "   Save Changes"))
            ImGui::OpenPopup("Save changes");

        if (ImGui::BeginPopupModal("Save changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure to save your changes?\n\n");
            ImGui::Separator();

            if (ImGui::Button("Yes", ImVec2(120, 0)))
            {
                auto saveStatus = true;

                if (mPersonData->isNew())
                    saveStatus = FlsServices::Database::ref().personRepository().add(mPersonData);
                else
                    saveStatus = FlsServices::Database::ref().personRepository().update(mPersonData);

                if (!saveStatus)
                    XN_ERROR("Failed to save changes")

                mNeedsGridRefresh = true;

                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("No", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, 10.0f));

        const auto firstFace = mPersonData && !mPersonData->faces.empty() ? mPersonData->faces[0]->texture() : nullptr;
        auto* firstFaceTexturePtr = firstFace ? reinterpret_cast<void*>(static_cast<intptr_t>(firstFace->id())) : nullptr;

        ImGui::Image(firstFaceTexturePtr, ImVec2(128.0f, 128.0f));
        ImGui::SameLine();

        ImGui::BeginGroup();

        const auto groupY = ImGui::GetCursorPosY();

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
        ImGui::BeginGroup();
        ImGui::Text("Name");
        ImGui::InputText("##PersonNameInput", &mPersonData->name);
        ImGui::EndGroup();
        ImGui::PopItemWidth();

        ImGui::SameLine();

        ImGui::SetCursorPosY(groupY - 3.0f);
        ImGui::PushItemWidth(-1);
        ImGui::BeginGroup();
        ImGui::Text("Date");

        ImGui::InputText("##PersonNameDate", &mPersonData->birthdate);

        ImGui::EndGroup();
        ImGui::PopItemWidth();

        ImGui::PushItemWidth(-1);
        ImGui::BeginGroup();
        ImGui::Text("Note");
        ImGui::InputTextMultiline("##PersonNameNote", &mPersonData->note, ImVec2(0.0f, 65.0f));
        ImGui::EndGroup();
        ImGui::PopItemWidth();

        ImGui::EndGroup();

        ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, 5.0f));

        ImGui::Text("Faces");

        const auto faceItemSize = 96.0f;
        const auto contentWidth = ImGui::GetContentRegionAvailWidth();
        const auto contentHeight = ImGui::GetContentRegionAvail().y;

        ImGui::BeginChild("##ContentRegion", { contentWidth, contentHeight }, true);

        auto columns = static_cast<int>(contentWidth / faceItemSize);
        columns = columns < 1 ? 1 : columns;
        ImGui::Columns(columns, nullptr, false);

        auto idx{ 0 }, idxToRemove{ -1 };

        for (const auto& face : mPersonData->faces)
        {
            ImGui::PushID(idx);

            auto* texturePtr = reinterpret_cast<void*>(static_cast<intptr_t>(face->texture()->id()));

            ImGui::BeginGroup();

            ImGui::ImageButton(texturePtr, { faceItemSize - 20, faceItemSize - 16 });

            if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
                idxToRemove = idx;

            ImGui::EndGroup();

            ImGui::PopID();
            ImGui::NextColumn();
            idx++;
        }

        if(idxToRemove != -1)
        {
            mPersonData->faces.erase(mPersonData->faces.begin() + idxToRemove);
            idxToRemove = -1;
        }

        ImGui::EndChild();
    }
}
