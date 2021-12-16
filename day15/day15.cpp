#include <fstream>
#include <queue>
#include <string>
#include <vector>

#include <fmt/core.h>

using u8 = std::uint8_t;
using u32 = std::uint32_t;

static constexpr size_t K_GRID_WIDTH{ 10 };
static constexpr size_t K_GRID_HEIGHT{ 10 };
static constexpr size_t K_GRID_CELL_COUNT{ K_GRID_WIDTH * K_GRID_HEIGHT };

static constexpr size_t K_GRID_EXTENSION{ 5 };
static constexpr size_t K_EXTENDED_GRID_WIDTH{ K_GRID_WIDTH * K_GRID_EXTENSION };
static constexpr size_t K_EXTENDED_GRID_HEIGHT{ K_GRID_HEIGHT * K_GRID_EXTENSION };
static constexpr size_t K_EXTENDED_GRID_CELL_COUNT{ K_EXTENDED_GRID_WIDTH * K_EXTENDED_GRID_HEIGHT };

bool ReadInput(std::vector<u8>& grid)
{
    static const char* inputFile{ "input.txt" };
    std::ifstream inputStream{ inputFile };

    bool readSucceeded{ inputStream.is_open() };
    if (readSucceeded)
    {
        std::string lineText{};
        for (u32 j = 0; j < K_GRID_HEIGHT; ++j)
        {
            std::getline(inputStream, lineText);
            for (u32 i = 0; i < K_GRID_WIDTH; ++i)
            {
                grid[i + j * K_GRID_WIDTH] = (u8)(lineText[i] - '0');
            }
        }

        inputStream.close();
    }

    return readSucceeded;
}

struct CellSearchInfo
{
    CellSearchInfo(u32 cumulativeScore, u8 xPos, u8 yPos)
        : CumulativeScore{ cumulativeScore }
        , x{ xPos }
        , y{ yPos }
    {
    }

    u32 CumulativeScore{};
    u8 x{};
    u8 y{};
};

bool CompareCells(const CellSearchInfo& lhs, const CellSearchInfo& rhs)
{
    return lhs.CumulativeScore < rhs.CumulativeScore;
}

using CellQueue = std::priority_queue<CellSearchInfo, std::vector<CellSearchInfo>, decltype(&CompareCells)>;

inline void TryVisitCell(CellQueue& cellQueue,
                         std::vector<u32>& cellBestScore,
                         const std::vector<u8>& grid,
                         u32 cumulativeScore, u8 x, u8 y, u32 w, u32 h)
{
    if (x < w && y < h)
    {
        u32 index{ x + y * w };
        cumulativeScore += grid[index];
        if (cumulativeScore < cellBestScore[index])
        {
            cellBestScore[index] = cumulativeScore;
            cellQueue.emplace(cumulativeScore, x, y);
        }
    }
}

u32 ComputeBestPathScore(const std::vector<u8>& grid, u32 w, u32 h)
{
    CellQueue cellQueue(CompareCells);
    std::vector<u32> cellBestScore(grid.size(), std::numeric_limits<u32>::max());

    TryVisitCell(cellQueue, cellBestScore, grid, 0, 0, 0, w, h);

    while (!cellQueue.empty())
    {
        CellSearchInfo cellInfo{ cellQueue.top() };
        cellQueue.pop();

        if (cellInfo.x != w - 1 || cellInfo.y != h - 1)
        {
            TryVisitCell(cellQueue, cellBestScore, grid, cellInfo.CumulativeScore, cellInfo.x + 1, cellInfo.y, w, h);
            TryVisitCell(cellQueue, cellBestScore, grid, cellInfo.CumulativeScore, cellInfo.x - 1, cellInfo.y, w, h);
            TryVisitCell(cellQueue, cellBestScore, grid, cellInfo.CumulativeScore, cellInfo.x, cellInfo.y + 1, w, h);
            TryVisitCell(cellQueue, cellBestScore, grid, cellInfo.CumulativeScore, cellInfo.x, cellInfo.y - 1, w, h);
        }
    }

    return cellBestScore[w * h - 1] - grid[0];
}

void BuildExtendedGrid(const std::vector<u8>& baseGrid, std::vector<u8>& extendedGrid)
{
    for (u32 index = 0; index < K_GRID_CELL_COUNT; ++index)
    {
        u32 x{ index % K_GRID_WIDTH};
        u32 y{ index / K_GRID_WIDTH};

        for (u32 j = 0; j < K_GRID_EXTENSION; ++j)
        {
            u32 extendedY{ y + j * K_GRID_HEIGHT };
            for (u32 i = 0; i < K_GRID_EXTENSION; ++i)
            {
                u32 extendedX{ x + i * K_GRID_WIDTH };
                u32 extendedIndex{ extendedX + extendedY * K_EXTENDED_GRID_WIDTH };
                extendedGrid[extendedIndex] = (baseGrid[index] + i + j);
                if (extendedGrid[extendedIndex] > 9) { extendedGrid[extendedIndex] -= 9; }
            }
        }
    }
}

int main()
{
    std::vector<u8> grid(K_GRID_CELL_COUNT, 0);
    if (ReadInput(grid))
    {
        //u32 bestScore{ ComputeBestPathScore(grid, K_GRID_WIDTH, K_GRID_HEIGHT) };
        //fmt::print("Best Score: {}.\n", bestScore);

        // Does not work for part 2. Must optimize!
        std::vector<u8> extendedGrid(K_EXTENDED_GRID_CELL_COUNT, 0);
        BuildExtendedGrid(grid, extendedGrid);
        u32 bestExtendedScore{ ComputeBestPathScore(extendedGrid, K_EXTENDED_GRID_WIDTH, K_EXTENDED_GRID_HEIGHT) };
        fmt::print("Best Extended Score: {}.\n", bestExtendedScore);
    }
    else
    {
        fmt::print("Failed to open input file.\n");
    }
    return 0;
}