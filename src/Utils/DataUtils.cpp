#include "DataUtils.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/core/opengl.hpp>

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

    std::shared_ptr<Xenon::Texture2D> DataUtils::imgToTexture(const cv::Mat& mat)
    {
        cv::cuda::GpuMat dRgb;
        const cv::cuda::GpuMat dImg(mat);

        cv::cuda::cvtColor(dImg, dRgb, cv::COLOR_BGR2RGB);

        const cv::ogl::Buffer buffer(dRgb, cv::ogl::Buffer::PIXEL_UNPACK_BUFFER, true);
        buffer.bind(cv::ogl::Buffer::PIXEL_UNPACK_BUFFER);

        const Xenon::Texture2DConfiguration textureCfg(dImg.cols, dImg.rows,
            Xenon::TextureFormat::RGB);
        const auto texture = Xenon::Texture2D::create(nullptr, textureCfg, 1);

        cv::ogl::Buffer::unbind(cv::ogl::Buffer::PIXEL_UNPACK_BUFFER);

        return texture;
    }
}
