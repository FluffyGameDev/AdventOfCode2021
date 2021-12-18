#include <fstream>
#include <vector>
#include <string>

#include <fmt/core.h>

#include "pairregistry.h"
#include "pairoperations.h"
#include "tests.h"
#include "types.h"

bool ReadInput(std::vector<u32>& pairs, Day18::PairRegistry& registry)
{
    static const char* inputFile{ "input.txt" };
    std::ifstream inputStream{ inputFile };

    bool readSucceeded{ inputStream.is_open() };
    if (readSucceeded)
    {
        std::string lineText{};
        while (std::getline(inputStream, lineText))
        {
            u32 newPairID{ Day18::Operations::ReadPair(std::string_view(lineText), registry) };
            pairs.push_back(newPairID);
        }

        inputStream.close();
    }

    return readSucceeded;
}

// Deliberately copying the registry to avoid part 1 affecting part 2.
u32 ComputeTotalSumMagnitude(std::vector<u32>& pairs, Day18::PairRegistry& registry)
{
    u32 sum{ pairs[0] };
    Day18::Operations::ReducePair(sum, registry);

    for (u32 i = 1; i < pairs.size(); ++i)
    {
        sum = Day18::Operations::AddPairs(sum, pairs[i], registry);
        Day18::Operations::ReducePair(sum, registry);
    }

    return Day18::Operations::ComputePairMagnitude(sum, registry);
}

u32 ComputeLargestSumMagnitude(std::vector<u32>& pairs, Day18::PairRegistry& registry)
{
    u32 maxMagnitude{};
    u32 pairCount{ (u32)pairs.size() };
    for (u32 i = 0; i < pairCount; ++i)
    {
        for (u32 j = 0; j < pairCount; ++j)
        {
            if (i != j)
            {
                u32 pair1Copy{ Day18::Operations::CopyPair(pairs[i], registry) };
                u32 pair2Copy{ Day18::Operations::CopyPair(pairs[j], registry) };

                u32 sum{ Day18::Operations::AddPairs(pair1Copy, pair2Copy, registry) };
                Day18::Operations::ReducePair(sum, registry);

                maxMagnitude = std::max(maxMagnitude, Day18::Operations::ComputePairMagnitude(sum, registry));
            }
        }
    }
    return maxMagnitude;
}

int main()
{
    if (!Day18::ValidateTests())
    {
        fmt::print("Failed Tests.\n");
        return -1;
    }
    else
    {
        fmt::print("All Tests Succeeded.\n");
    }

    std::vector<u32> pairs{};
    Day18::PairRegistry registry{};
    if (ReadInput(pairs, registry))
    {
        //fmt::print("Sum Magnitude: {}.\n", ComputeTotalSumMagnitude(pairs, registry));
        fmt::print("Largest Sum Magnitude: {}.\n", ComputeLargestSumMagnitude(pairs, registry));
    }
    else
    {
        fmt::print("Failed to open input file.\n");
    }
    return 0;
}