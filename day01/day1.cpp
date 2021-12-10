#include <algorithm>
#include <fstream>
#include <numeric>
#include <vector>

#include <fmt/core.h>

bool ReadInputValues(std::vector<std::uint32_t>& inputValues)
{
    static const char* inputFile{ "input.txt" };
    std::ifstream inputStream{ inputFile };

    bool readSucceeded{ inputStream.is_open() };
    if (readSucceeded)
    {
        while (!inputStream.eof())
        {
            std::uint32_t readValue{};
            inputStream >> readValue;
            inputValues.push_back(readValue);
        }

        inputStream.close();
    }

    return readSucceeded;
}

std::uint32_t ComputeIncreaseCount(const std::vector<std::uint32_t>& inputValues)
{
    using ValuePair = std::pair<std::uint32_t, std::uint32_t>;
    auto hasAnIncrease = [](const std::uint32_t& value)
    {
        const ValuePair* valuePair{ (const ValuePair*)(&value) };
        return valuePair->second > valuePair->first;
    };

    return (std::uint32_t)std::count_if(inputValues.begin(), inputValues.end() - 1, hasAnIncrease);
}

std::uint32_t ComputeSumIncreaseCount(const std::vector<std::uint32_t>& inputValues)
{
    static constexpr std::uint32_t valuesPerSum{ 3 };
    auto computeSum = [](const std::uint32_t& value)
    {
        return std::accumulate(&value, &value + valuesPerSum, 0);
    };

    std::vector<std::uint32_t> sums{};
    sums.resize(inputValues.size() - valuesPerSum + 1);
    std::transform(inputValues.begin(), inputValues.end() - valuesPerSum + 1, sums.begin(), computeSum);

    // Allocating an array just for the sums can be considered wasteful.
    // In this case, I just wanted to have fun with std algorithms.

    return ComputeIncreaseCount(sums);
}

int main()
{
    // Reading all values to a vector is suboptimal.
    // It's better to compute the results as we read the file.
    // I just wanted to have fun with some std algorithms. ;)

    std::vector<std::uint32_t> inputValues{};
    if (ReadInputValues(inputValues))
    {
        auto increaseCount = ComputeIncreaseCount(inputValues);
        auto sumIncreaseCount = ComputeSumIncreaseCount(inputValues);

        fmt::print("Increase Count: {}\n", increaseCount);
        fmt::print("Sum Increase Count: {}\n", sumIncreaseCount);
    }
    else
    {
        fmt::print("Failed to open input file.\n");
    }

    return 0;
}