#include "Random.hpp"

namespace Fls
{
    int Random::generateInt(const int min, const int max)
    {
        static thread_local std::mt19937 generator;
        const std::uniform_int_distribution<int> distribution(min, max);
        return distribution(generator);
    }

    std::string Random::generateHex(const uint32_t length)
    {
        std::stringstream ss;

        for (std::size_t i = 0; i < length; i++) 
        {
            std::stringstream hexStream;

            const auto c = static_cast<char>(generateInt(0, 255));
            hexStream << std::hex << c;

            auto hex = hexStream.str();
            ss << (hex.length() < 2 ? '0' + hex : hex);
        }

        return ss.str();
    }
}
