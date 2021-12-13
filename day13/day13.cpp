#include <fstream>
#include <set>
#include <string>
#include <vector>

#include <fmt/core.h>

using u32 = std::uint32_t;
using u64 = std::uint64_t;

struct Vec2
{
    u32 x{};
    u32 y{};
};

enum class FoldAxis
{
    X,
    Y
};

struct Fold
{
    FoldAxis Axis{};
    u32 FoldCoordinate{};
};

void ReadPoint(const std::string& rawText, Vec2& point)
{
    sscanf(rawText.c_str(), "%u,%u", &point.x, &point.y);
}

void ReadFold(const std::string& rawText, Fold& fold)
{
    char axis{};
    sscanf(rawText.c_str(), "fold along %c=%u", &axis, &fold.FoldCoordinate);
    fold.Axis = (axis == 'x' ? FoldAxis::X : FoldAxis::Y);
}

bool ReadInput(std::vector<Vec2>& points, std::vector<Fold>& folds)
{
    static const char* inputFile{ "input.txt" };
    std::ifstream inputStream{ inputFile };

    bool readSucceeded{ inputStream.is_open() };
    if (readSucceeded)
    {
        std::string lineText{};
        while (std::getline(inputStream, lineText) && !lineText.empty())
        {
            ReadPoint(lineText, points.emplace_back());
        }

        while (std::getline(inputStream, lineText) && !lineText.empty())
        {
            ReadFold(lineText, folds.emplace_back());
        }

        inputStream.close();
    }

    return readSucceeded;
}

void PerformFold(std::vector<Vec2>& points, const Fold& fold)
{
    switch (fold.Axis)
    {
        case FoldAxis::X:
        {
            for (Vec2& point : points)
            {
                if (point.x > fold.FoldCoordinate)
                {
                    point.x = 2 * fold.FoldCoordinate - point.x;
                }
            }
            break;
        }
        case FoldAxis::Y:
        {
            for (Vec2& point : points)
            {
                if (point.y > fold.FoldCoordinate)
                {
                    point.y = 2 * fold.FoldCoordinate - point.y;
                }
            }
            break;
        }
    }
}

u32 CountUniquePoints(const std::vector<Vec2>& points)
{
    std::set<u64> pointSet{};
    for (const Vec2& point : points)
    {
        const u64* pointU64{ reinterpret_cast<const u64*>(&point) };
        pointSet.insert(*pointU64);
    }
    return (u32)pointSet.size();
}

void PrintPoints(const std::vector<Vec2>& points)
{
    std::vector<std::string> lines{};
    lines.resize(8, std::string(80, ' '));

    for (const Vec2& point : points)
    {
        lines[point.y][point.x] = '#';
    }

    for (const std::string& line : lines)
    {
        fmt::print("{}\n", line);
    }
}

int main()
{
    std::vector<Vec2> points{};
    std::vector<Fold> folds{};
    if (ReadInput(points, folds))
    {
        PerformFold(points, folds[0]);
        u32 uniquePointCount{ CountUniquePoints(points) };
        fmt::print("Unique Points after one fold: {}.\n", uniquePointCount);

        for (const Fold& fold : folds)
        {
            PerformFold(points, fold);
        }
        PrintPoints(points);
    }
    else
    {
        fmt::print("Failed to open input file.\n");
    }
    return 0;
}