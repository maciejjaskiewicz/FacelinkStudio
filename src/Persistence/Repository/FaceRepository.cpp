#include "FaceRepository.hpp"

#include "Persistence/FlsDatabase.hpp"
#include "Persistence/SqlStatement.hpp"

#include <dlib/image_processing.h>
#include <opencv2/imgcodecs.hpp>

namespace Fls
{
    FaceRepository::FaceRepository(FlsDatabase& db)
        : mDb(db)
    { }

    std::vector<int> FaceRepository::getFacesIds(const int personId) const
    {
        std::vector<int> personFaces;

        SqlStatement facesSt(mDb, "SELECT Id FROM Face WHERE OwnerId = ?");

        facesSt.bindInt(1, personId);
        facesSt.exec();

        while (facesSt.moveNext())
            personFaces.push_back(facesSt.getColumnAsInt(0));

        return personFaces;
    }

    bool FaceRepository::add(const std::shared_ptr<FaceModel>& faceModel) const
    {
        try
        {
            SqlStatement st(mDb, "INSERT INTO Face(OwnerId, Embeddings, Image, CreatedDate) "\
                "VALUES(?,?,?,DATETIME('now'))");

            std::ostringstream encodedEmbeddings;
            dlib::serialize(faceModel->embeddings, encodedEmbeddings);

            std::vector<uchar> encodedImg;
            cv::imencode(".jpg", faceModel->image, encodedImg);

            st.bindInt(1, faceModel->personId);
            st.bindStream(2, encodedEmbeddings);
            st.bindBlob(3, encodedImg);

            st.exec();

            faceModel->id = st.lastInsertedId();
        }
        catch (const std::exception& e)
        {
            XN_ERROR(e.what());
            return false;
        }
        
        return true;
    }

    bool FaceRepository::remove(const int faceId) const
    {
        try
        {
            SqlStatement st(mDb, "DELETE FROM Face WHERE Id = ?");

            st.bindInt(1, faceId);
            st.exec();
        }
        catch (const std::exception& e)
        {
            XN_ERROR(e.what());
            return false;
        }

        return true;
    }
}