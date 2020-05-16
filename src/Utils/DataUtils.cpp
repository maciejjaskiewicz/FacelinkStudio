#include "DataUtils.hpp"

#include <opencv2/core/mat.hpp>

namespace Fls
{
    void DataUtils::matToVector(const cv::Mat& mat, std::vector<float>& out)
    {
        if (mat.isContinuous())
        {
            out.assign(reinterpret_cast<float*>(mat.data), reinterpret_cast<float*>(mat.data) + mat.total());
        }
        else
        {
            for (auto i = 0; i < mat.rows; ++i)
            {
                out.insert(out.end(), mat.ptr<float>(i), mat.ptr<float>(i) + mat.cols);
            }
        }
    }

    std::string DataUtils::vectorToString(const std::vector<int>& arr, const std::string& separator)
    {
        std::stringstream ss;
        const auto n = arr.size();

        for (std::size_t i = 0; i < n; i++)
        {
            ss << arr[i];
            if (i < n - 1) ss << separator;
        }

        return ss.str();
    }
}