#include <algorithm>
#include <array>
#include <fstream>
#include <numeric>

#include <fmt/core.h>


using u32 = std::uint32_t;
using u64 = std::uint64_t;

static constexpr u32 K_DAYS_PER_REPRODUCTION{ 7 };
static constexpr u32 K_NEW_FISH_EXTRA_DAYS{ 2 };
static constexpr u32 K_DAYS_PER_REPRODUCTION_NEW_FISH{ K_DAYS_PER_REPRODUCTION + K_NEW_FISH_EXTRA_DAYS };
static constexpr u32 K_NUMBER_OF_DAYS{ 256 };

using FishCounter = std::array<u64, K_DAYS_PER_REPRODUCTION_NEW_FISH>;

bool ReadInput(FishCounter& fishCounter)
{
    static const char* inputFile{ "input.txt" };
    std::ifstream inputStream{ inputFile };

    bool readSucceeded{ inputStream.is_open() };
    if (readSucceeded)
    {
        while (!inputStream.eof())
        {
            char readchar{};
            inputStream >> readchar;
            u32 daysLeft{ (u32)(readchar - '0') };
            ++fishCounter[daysLeft];

            inputStream >> readchar;
        }

        inputStream.close();
    }

    return readSucceeded;
}

void SimulateDay(FishCounter& fishCounter)
{
    u64 fishGivingBirth{ fishCounter[0] };
    std::rotate(fishCounter.begin(), fishCounter.begin() + 1, fishCounter.end());
    fishCounter[K_DAYS_PER_REPRODUCTION - 1] += fishGivingBirth;
    fishCounter[K_DAYS_PER_REPRODUCTION_NEW_FISH - 1] = fishGivingBirth;
}

void SimulateForNDays(FishCounter& fishCounter, u32 simulationLength)
{
    for (u32 day = 0; day < simulationLength; ++day)
    {
        SimulateDay(fishCounter);
    }
}

u64 ComputeTotalFishCount(const FishCounter& fishCounter)
{
    return std::accumulate(fishCounter.begin(), fishCounter.end(), 0ULL);
}

int main()
{
    FishCounter fishCounter{};
    if (ReadInput(fishCounter))
    {
        SimulateForNDays(fishCounter, K_NUMBER_OF_DAYS);
        u64 totalFishCount{ ComputeTotalFishCount(fishCounter) };

        fmt::print("Total fish count: {}.\n", totalFishCount);
    }
    else
    {
        fmt::print("Failed to open input file.\n");
    }

    return 0;
}