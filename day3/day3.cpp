#include <array>
#include <algorithm>
#include <functional>
#include <fstream>
#include <vector>

#include <fmt/core.h>

constexpr size_t bitsPerValue{ 12 };
using BitCounter = std::array<std::uint32_t, bitsPerValue>;

bool ParseInput(std::vector<std::uint32_t>& valueList)
{
    static const char* inputFile{ "input.txt" };
    std::ifstream inputStream{ inputFile };

    bool readSucceeded{ inputStream.is_open() };
    if (readSucceeded)
    {
        std::string currentValue{};
        while (!inputStream.eof())
        {
            inputStream >> currentValue;

            std::uint32_t valueNumber{ strtoul(currentValue.c_str(), nullptr, 2) };
            valueList.push_back(valueNumber);
        }

        inputStream.close();
    }

    return readSucceeded;
}

std::uint32_t ComputeBitCountAtIndex(const std::vector<std::uint32_t>& valueList, std::uint32_t index)
{
    std::uint32_t mask = 1 << (bitsPerValue - index - 1);
    auto matchesMask = [mask](std::uint32_t value) { return (value & mask) == mask; };
    return (std::uint32_t)std::count_if(valueList.begin(), valueList.end(), matchesMask);
}

void ComputeBitCount(const std::vector<std::uint32_t>& valueList, BitCounter& bitCount)
{
    // Not ideal to work this way, but ComputeBitCountAtIndex can be reused for part 2.
    for (std::uint32_t i = 0; i < bitsPerValue; ++i)
    {
        bitCount[i] = ComputeBitCountAtIndex(valueList, i);
    }
}

std::uint32_t ComputeRating(std::vector<std::uint32_t> valueList, std::function<bool(std::uint32_t, std::uint32_t)> filterCriterion)
{
    //Yuck! A vector copy. I really need to look into views and filtering!
    for (std::uint32_t i = 0; i < bitsPerValue; ++i)
    {
        std::uint32_t bitCount{ ComputeBitCountAtIndex(valueList, i) };
        std::uint32_t totalCount{ (std::uint32_t)valueList.size() };

        std::uint32_t bitShift{ bitsPerValue - i - 1 };
        std::uint32_t mask{ 1UL << bitShift };
        std::uint32_t expectedValue{ (std::uint32_t)filterCriterion(bitCount, totalCount) << bitShift };

        auto filterRule = [mask, expectedValue](std::uint32_t v) { return (v & mask) == expectedValue; };

        std::vector<std::uint32_t> oldValueList{ std::move(valueList) };
        valueList.resize(expectedValue ? bitCount : (totalCount - bitCount));
        std::copy_if(oldValueList.begin(), oldValueList.end(), valueList.begin(), filterRule);

        if (valueList.size() == 1)
        {
            return valueList[0];
        }
    }
    
    return 0;
}

std::tuple<std::uint32_t, std::uint32_t> ComputeGammaEpsilonValues(const BitCounter& bitCount, size_t totalValueCount)
{
    const size_t bitCountLimit{ totalValueCount / 2 };
    std::uint32_t gamma{};
    
    auto accumulateMostCommonBit = [&gamma, bitCountLimit](std::uint32_t bitCount)
        { gamma = (gamma << 1) | (bitCount > bitCountLimit); };

    std::for_each(bitCount.cbegin(), bitCount.cend(), accumulateMostCommonBit);

    std::uint32_t mask{ (1 << bitsPerValue) - 1 };
    std::uint32_t epsilon{ ~gamma & mask };
    return { gamma, epsilon };
}

int main()
{
    std::vector<std::uint32_t> valueList{};
	if (ParseInput(valueList))
	{
        BitCounter bitCount{};
        ComputeBitCount(valueList, bitCount);
        auto [gamma, epsilon] { ComputeGammaEpsilonValues(bitCount, valueList.size()) };
        fmt::print("Gamme: {}\n", gamma);
        fmt::print("Epsilon: {}\n", epsilon);
        fmt::print("Gamma * Epsilon: {}\n", gamma * epsilon);

        auto o2Filter = [](std::uint32_t count, std::uint32_t total) { return count >= (total - count); };
        std::uint32_t o2GeneratorRating{ ComputeRating(valueList, o2Filter) };

        auto co2Filter = [](std::uint32_t count, std::uint32_t total) { return count < (total - count); };
        std::uint32_t co2scrubberRating{ ComputeRating(valueList, co2Filter) };

        fmt::print("O2 Generator Rating: {}\n", o2GeneratorRating);
        fmt::print("CO2 scrubber Rating: {}\n", co2scrubberRating);
        fmt::print("O2 Generator * CO2 scrubber: {}\n", o2GeneratorRating * co2scrubberRating);
	}
	else
	{
        fmt::print("Failed to open input file.\n");
	}

	return 0;
}