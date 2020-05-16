#include "FaceClassifier.hpp"

#include "Persistence/SqlStatement.hpp"
#include "Utils/DataUtils.hpp"

namespace Fls
{
    void FaceClassifier::init(const std::shared_ptr<FlsDatabase>& database)
    {
        mDatabase = database;

        mInitializedFuture = std::async(std::launch::async, [this]()
        {
            constexpr auto selectClassifierSql = "SELECT Data FROM Classifier ORDER BY Id DESC LIMIT 1";
            SqlStatement selectClassifierStatement(*mDatabase, selectClassifierSql);

            selectClassifierStatement.exec();

            if (selectClassifierStatement.moveNext())
            {
                auto classifierData = selectClassifierStatement.getColumnAsStream(0);
                dlib::deserialize(mClassifier, classifierData);

                mInitialized.store(true);
            }
        });
    }

    std::vector<FaceClassificationResult> FaceClassifier::classify(const UserResource* userResource, 
        const std::vector<std::vector<float>>& embeddedFaces)
    {
        std::vector<FaceClassificationResult> result;

        if (!mInitialized.load() || !userResource || embeddedFaces.empty())
            return result;

        if (mLastFrameId != userResource->id)
        {
            std::vector<int> predictedPersonIds;
            predictedPersonIds.reserve(embeddedFaces.size());

            for(const auto& embeddings : embeddedFaces)
            {
                predictedPersonIds.push_back(classify(embeddings));
            }

            result = namePredictions(predictedPersonIds);

            mLastFrameId = userResource->id;
            mLastFrameClassificationResult = result;

            return result;
        }

        return result;
    }

    int FaceClassifier::classify(const std::vector<float>& faceEmbeddings) const
    {
        const auto embeddingsMat = dlib::mat(faceEmbeddings);
        const dlib::matrix<float, 0, 1> embeddings(embeddingsMat);

        const Sample sample = dlib::matrix_cast<double>(embeddings);

        const auto classificationResult = mClassifier.function.predict(mClassifier.normalizer(sample));

        if(classificationResult.second > mConfidenceThreshold)
            return classificationResult.first;

        return -1;
    }

    std::vector<FaceClassificationResult> FaceClassifier::namePredictions(const std::vector<int>& predictions) const
    {
        std::vector<FaceClassificationResult> result;

        std::vector<int> validPredictions;
        std::copy_if(predictions.begin(), predictions.end(), std::back_inserter(validPredictions), 
            [](const int i) { return i >= 0; }
        );

        const auto selectPeopleSql = "SELECT Id, Name FROM Person WHERE Id IN (" + DataUtils::vectorToString(validPredictions, ",") + ")";
        SqlStatement selectPeopleStatement(*mDatabase, selectPeopleSql);

        selectPeopleStatement.exec();

        std::map<int, std::string> selectedPeople;
        while (selectPeopleStatement.moveNext())
        {
            const auto id = selectPeopleStatement.getColumnAsInt(0);
            const auto name = selectPeopleStatement.getColumnAsText(1);

            selectedPeople.emplace(id, name);
        }

        for(const auto& prediction : predictions)
        {
            FaceClassificationResult classificationResult;

            const auto personIt = selectedPeople.find(prediction);
            if(personIt != selectedPeople.end())
            {
                classificationResult.personId = prediction;
                classificationResult.name = personIt->second;
            }

            result.emplace_back(classificationResult);
        }

        return result;
    }
}