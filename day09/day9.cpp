#include <algorithm>
#include <fstream>
#include <functional>
#include <limits>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include <fmt/core.h>


using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

static constexpr size_t K_MAP_WIDTH{ 100 };
static constexpr size_t K_MAP_HEIGHT{ 100 };
static constexpr size_t K_MAP_CELL_COUNT{ K_MAP_WIDTH * K_MAP_HEIGHT };
static constexpr u32 K_TOP_BASSIN_COUNT{ 3 };
static constexpr i32 NO_BASIN_ID{ std::numeric_limits<i32>::max() };
using HeightMap = u8[K_MAP_HEIGHT][K_MAP_WIDTH];
using BasinMap = i32[K_MAP_HEIGHT][K_MAP_WIDTH];

struct Vec2
{
    u32 x{};
    u32 y{};
};

bool ReadInput(HeightMap& heightMap)
{
    static const char* inputFile{ "input.txt" };
    std::ifstream inputStream{ inputFile };

    bool readSucceeded{ inputStream.is_open() };
    if (readSucceeded)
    {
        u32 y{};
        std::string lineText;
        while (std::getline(inputStream, lineText))
        {
            std::transform(lineText.begin(), lineText.end(), &heightMap[y][0],
                [](char c) { return (u8)(c - '0'); });
            ++y;
        }

        inputStream.close();
    }

    return readSucceeded;
}

u32 ComputeLowerPointCount(const HeightMap& heightMap, const Vec2& pos)
{
    u32 lowerPointCount{};

    u8 height{ heightMap[pos.y][pos.x] };
    if (pos.x > 0 && heightMap[pos.y][pos.x - 1] <= height) { ++lowerPointCount; }
    if (pos.y > 0 && heightMap[pos.y - 1][pos.x] <= height) { ++lowerPointCount; }
    if (pos.x < K_MAP_WIDTH - 1 && heightMap[pos.y][pos.x + 1] <= height) { ++lowerPointCount; }
    if (pos.y < K_MAP_HEIGHT - 1 && heightMap[pos.y + 1][pos.x] <= height) { ++lowerPointCount; }

    return lowerPointCount;
}

void ComputeLowPoints(const HeightMap& heightMap, std::vector<Vec2>& lowPoints)
{
    for (u32 y = 0; y < K_MAP_HEIGHT; ++y)
    {
        for (u32 x = 0; x < K_MAP_WIDTH; ++x)
        {
            Vec2 pos{ x, y };
            if (ComputeLowerPointCount(heightMap, pos) == 0)
            {
                lowPoints.push_back(pos);
            }
        }
    }
}

u32 ComputeTotalRiskLevel(const HeightMap& heightMap, const std::vector<Vec2>& lowPoints)
{
    auto addRiskLevel = [&heightMap](u32 total, const Vec2& pos) { return total + heightMap[pos.y][pos.x] + 1; };
    return std::accumulate(lowPoints.begin(), lowPoints.end(), 0, addRiskLevel);
}

void ValidateBasinID(i32& currentID, i32 newID)
{
    if (currentID == -1)
    {
        currentID = newID;
    }
    else if (newID != -1)
    {
        if (currentID != newID)
        {
            currentID = NO_BASIN_ID;
        }
    }
}

i32 FindBasinID(BasinMap& basinMap, const HeightMap& heightMap, u32 x, u32 y)
{
    if (basinMap[y][x] >= 0)
    {
        return basinMap[y][x];
    }
    if (heightMap[y][x] == 9)
    {
        basinMap[y][x] = NO_BASIN_ID;
        return basinMap[y][x];
    }
    else
    {
        i32 basinID{ -1 };

        u8 height{ heightMap[y][x] };
        if (basinID != NO_BASIN_ID && x > 0 && heightMap[y][x - 1] < height) { ValidateBasinID(basinID, FindBasinID(basinMap, heightMap, x - 1, y)); }
        if (basinID != NO_BASIN_ID && y > 0 && heightMap[y - 1][x] < height) { ValidateBasinID(basinID, FindBasinID(basinMap, heightMap, x, y - 1)); }
        if (basinID != NO_BASIN_ID && x < K_MAP_WIDTH - 1 && heightMap[y][x + 1] < height) { ValidateBasinID(basinID, FindBasinID(basinMap, heightMap, x + 1, y)); }
        if (basinID != NO_BASIN_ID && y < K_MAP_HEIGHT - 1 && heightMap[y + 1][x] < height) { ValidateBasinID(basinID, FindBasinID(basinMap, heightMap, x, y + 1)); }

        basinMap[y][x] = basinID;
        return basinID;
    }
}

void FillBasinMap(BasinMap& basinMap, const HeightMap& heightMap, const std::vector<Vec2>& lowPoints)
{
    std::fill(&basinMap[0][0], &basinMap[0][0] + K_MAP_CELL_COUNT, -1);

    i32 maxBasinID{ (i32)lowPoints.size() };
    for (i32 basinID = 0; basinID < maxBasinID; ++basinID)
    {
        const Vec2& pos{ lowPoints[basinID] };
        basinMap[pos.y][pos.x] = basinID;
    }

    for (u32 y = 0; y < K_MAP_HEIGHT; ++y)
    {
        for (u32 x = 0; x < K_MAP_WIDTH; ++x)
        {
            basinMap[y][x] = FindBasinID(basinMap, heightMap, x, y);
        }
    }
}

void ComputeAllBasinSizes(const HeightMap& heightMap, const std::vector<Vec2>& lowPoints, std::vector<u32>& basinSizes)
{
    BasinMap* basinMap{ new BasinMap{} };
    FillBasinMap(*basinMap, heightMap, lowPoints);

    basinSizes.resize(lowPoints.size(), 0);
    for (u32 y = 0; y < K_MAP_HEIGHT; ++y)
    {
        for (u32 x = 0; x < K_MAP_WIDTH; ++x)
        {
            i32 basinID{ (*basinMap[y][x] };
            if (basinID >= 0 && basinID < basinSizes.size())
            {
                ++basinSizes[basinID];
            }
        }
    }
}

u32 ComputeTopBasinSizes(const HeightMap& heightMap, const std::vector<Vec2>& lowPoints)
{
    std::vector<u32> basinSizes{};
    ComputeAllBasinSizes(heightMap, lowPoints, basinSizes);

    std::nth_element(basinSizes.begin(), basinSizes.begin() + K_TOP_BASSIN_COUNT, basinSizes.end(), std::greater{});
    return std::accumulate(basinSizes.begin(), basinSizes.begin() + K_TOP_BASSIN_COUNT, 1UL, std::multiplies{});
}

int main()
{
    HeightMap heightMap{};
    if (ReadInput(heightMap))
    {
        std::vector<Vec2> lowPoints{};
        ComputeLowPoints(heightMap, lowPoints);
        u32 totalRiskLevel{ ComputeTotalRiskLevel(heightMap, lowPoints) };
        fmt::print("Total Risk Level: {}.\n", totalRiskLevel);

        u32 topBasinSizes{ ComputeTopBasinSizes(heightMap, lowPoints) };
        fmt::print("Top Basin Sizes: {}.\n", topBasinSizes);
    }
    else
    {
        fmt::print("Failed to open input file.\n");
    }
    return 0;
}