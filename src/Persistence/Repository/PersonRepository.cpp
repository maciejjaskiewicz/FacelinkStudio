#include "PersonRepository.hpp"

#include "Persistence/FlsDatabase.hpp"
#include "Persistence/SqlStatement.hpp"

#include <dlib/image_processing.h>
#include <opencv2/imgcodecs.hpp>
#include "Persistence/DatabaseTransaction.hpp"

namespace Fls
{
    PersonRepository::PersonRepository(FlsDatabase& db)
        : mDb(db)
    { }

    std::shared_ptr<PersonModel> PersonRepository::get(const int personId) const
    {
        auto result = std::make_shared<PersonModel>();

        SqlStatement st(mDb, "SELECT Id, Name, Birthdate, Note FROM Person WHERE Id = ?");

        st.bindInt(1, personId);
        st.exec();

        if(st.moveNext())
        {
            result->id = st.getColumnAsInt(0);
            result->name = st.getColumnAsText(1);
            result->birthdate = st.getColumnAsText(2);
            result->note = st.getColumnAsText(3);

            SqlStatement fst(mDb, "SELECT Id, Embeddings, Image FROM Face WHERE OwnerId = ?");

            fst.bindInt(1, personId);
            fst.exec();

            while (fst.moveNext())
            {
                auto faceModel = std::make_shared<FaceModel>();

                faceModel->id = fst.getColumnAsInt(0);
                faceModel->personId = personId;

                auto embeddings = fst.getColumnAsStream(1);
                dlib::deserialize(faceModel->embeddings, embeddings);

                auto img = fst.getColumnAsBlob(2);
                faceModel->image = cv::imdecode(img, cv::IMREAD_COLOR);

                result->faces.emplace_back(faceModel);
            }
        }

        return result;
    }

    std::vector<std::shared_ptr<PersonGridModel>> PersonRepository::getGrid(const std::string& filter) const
    {
        std::vector<std::shared_ptr<PersonGridModel>> result;

        SqlStatement st(mDb, "SELECT Id, Name, Birthdate, Note, Faces, CreatedDate "\
            "FROM PersonView WHERE Name LIKE '%" + filter + "%'");
        
        st.exec();

        while (st.moveNext())
        {
            auto model = std::make_shared<PersonGridModel>();

            model->id = st.getColumnAsInt(0);
            model->name = st.getColumnAsText(1);
            model->birthdate = st.getColumnAsText(2);
            model->note = st.getColumnAsText(3);
            model->faces = st.getColumnAsInt(4);
            model->createdDate = st.getColumnAsText(5);

            result.push_back(model);
        }

        return result;
    }

    bool PersonRepository::add(const std::shared_ptr<PersonModel>& personModel) const
    {
        auto failed = false;

        try
        {
            DatabaseTransaction transaction(mDb);

            SqlStatement st(mDb, "INSERT INTO Person(Name, Birthdate, Note, CreatedDate) "\
                "VALUES(?,?,?,datetime('now'))");

            st.bindString(1, personModel->name);
            !personModel->birthdate.empty() ? st.bindString(2, personModel->birthdate) : st.bindNull(2);
            !personModel->note.empty() ? st.bindString(3, personModel->note) : st.bindNull(3);

            st.exec();

            const auto newPersonId = st.lastInsertedId();

            for(const auto& face : personModel->faces)
            {
                face->personId = newPersonId;
                failed = failed || !mDb.faceRepository().add(face);
            }

            transaction.commit();
        }
        catch (const std::exception& e)
        {
            XN_ERROR(e.what());
            failed = true;
        }

        return !failed;
    }

    bool PersonRepository::update(const std::shared_ptr<PersonModel>& personModel) const
    {
        auto failed = false;

        std::vector<int> facesToRemove;
        std::vector<std::shared_ptr<FaceModel>> facesToAdd;

        const auto personFaces = mDb.faceRepository().getFacesIds(personModel->id);

        for(const auto& face : personModel->faces)
        {
            if(face->isNew())
            {
                face->personId = personModel->id;
                facesToAdd.push_back(face);
            }
        }

        for(const auto faceId : personFaces)
        {
            auto it = std::find_if(personModel->faces.begin(), personModel->faces.end(),
                [faceId](const std::shared_ptr<FaceModel>& faceModel) {return faceModel->id == faceId; });

            if (it == personModel->faces.end())
                facesToRemove.push_back(faceId);
        }

        try
        {
            DatabaseTransaction transaction(mDb);

            SqlStatement st(mDb, "UPDATE Person SET Name = ?, Birthdate = ?, Note = ? WHERE Id = ?");

            st.bindString(1, personModel->name);
            !personModel->birthdate.empty() ? st.bindString(2, personModel->birthdate) : st.bindNull(2);
            !personModel->note.empty() ? st.bindString(3, personModel->note) : st.bindNull(3);
            st.bindInt(4, personModel->id);

            st.exec();

            for (const auto& face : facesToAdd)
                failed = failed || !mDb.faceRepository().add(face);

            for (const auto faceId : facesToRemove)
                failed = failed || !mDb.faceRepository().remove(faceId);

            transaction.commit();
        }
        catch (const std::exception& e)
        {
            XN_ERROR(e.what());
            failed = true;
        }

        return !failed;
    }

    bool PersonRepository::remove(const int personId) const
    {
        try
        {
            DatabaseTransaction transaction(mDb);

            SqlStatement deleteFacesSt(mDb, "DELETE FROM Face WHERE OwnerId = ?");
            SqlStatement deletePersonSt(mDb, "DELETE FROM Person WHERE Id = ?");

            deleteFacesSt.bindInt(1, personId);
            deletePersonSt.bindInt(1, personId);

            deleteFacesSt.exec();
            deletePersonSt.exec();

            transaction.commit();
        }
        catch (const std::exception& e)
        {
            XN_ERROR(e.what());
            return false;
        }

        return true;
    }
}