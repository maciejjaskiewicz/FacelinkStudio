#include "FaceClassifier.hpp"

#include "Persistence/SqlStatement.hpp"
#include "Persistence/DatabaseTransaction.hpp"
#include "Events/RetrainFaceClassifierEvent.hpp"
#include "Utils/DataUtils.hpp"

#include <Xenon/Core/ApplicationServices.hpp>
#include <dlib/image_processing.h>
#include <dlib/svm/svm_multiclass_linear_trainer.h>

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

        Xenon::ApplicationServices::EventBus::ref().subscribe<RetrainFaceClassifierEvent>(
            [this](const RetrainFaceClassifierEvent& event)
        {
            train();
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

    void FaceClassifier::train()
    {
        SqlStatement st(*mDatabase, "SELECT Id, OwnerId, Embeddings FROM Face");
        st.exec();

        std::vector<int> personIds, labels;
        std::vector<Sample> samples;

        while (st.moveNext())
        {
            std::vector<float> embeddingsVec;

            const auto id = st.getColumnAsInt(0);
            const auto label = st.getColumnAsInt(1);
            auto embeddingsData = st.getColumnAsStream(2);

            dlib::deserialize(embeddingsVec, embeddingsData);
            dlib::matrix<float, 0, 1> embeddings(dlib::mat(embeddingsVec));

            personIds.emplace_back(id);
            labels.emplace_back(label);
            samples.emplace_back(dlib::matrix_cast<double>(embeddings));
        }

        try
        {
            Normalizer normalizer;
            normalizer.train(samples);

            for (auto& sample : samples)
                sample = normalizer(sample);

            dlib::svm_multiclass_linear_trainer<LinearKernel, int> trainer;
            trainer.set_c(5);

            Classifier classifier;
            classifier.normalizer = normalizer;
            classifier.function = trainer.train(samples, labels);

            std::ostringstream serializedClassifier;
            dlib::serialize(classifier, serializedClassifier);

            DatabaseTransaction transaction(*mDatabase);

            SqlStatement cst(*mDatabase, "INSERT INTO Classifier(Data, Dirty, CreatedDate) VALUES(?,?,DATETIME('now'))");
            cst.bindStream(1, serializedClassifier);
            cst.bindInt(2, 0);
            cst.exec();

            const auto classifierId = cst.lastInsertedId();

            SqlStatement fst(*mDatabase, "UPDATE Face SET ClassifierId = ? WHERE "\
                "Id IN (" + DataUtils::vectorToString(personIds, ",") + ")");
            fst.bindInt(1, classifierId);
            fst.exec();

            transaction.commit();

            mClassifier = classifier;
        }
        catch (const std::exception& e)
        {
            XN_ERROR("Failed to retrain Face Classifier {}", e.what());
        }
    }
}
