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
                         u32 cumulativeScore, u8 x, u8 y)
{
    if (x < K_GRID_WIDTH && y < K_GRID_HEIGHT)
    {
        u32 index{ x + y * K_GRID_WIDTH };
        cumulativeScore += grid[index];
        if (cumulativeScore < cellBestScore[index])
        {
            cellBestScore[index] = cumulativeScore;
            cellQueue.emplace(cumulativeScore, x, y);
        }
    }
}

u32 ComputeBestPathScore(const std::vector<u8>& grid)
{
    CellQueue cellQueue(CompareCells);
    std::vector<u32> cellBestScore(grid.size(), std::numeric_limits<u32>::max());

    TryVisitCell(cellQueue, cellBestScore, grid, 0, 0, 0);

    while (!cellQueue.empty())
    {
        CellSearchInfo cellInfo{ cellQueue.top() };
        cellQueue.pop();

        if (cellInfo.x == K_GRID_WIDTH - 1 && cellInfo.y == K_GRID_HEIGHT - 1)
        {

        }
        else
        {
            TryVisitCell(cellQueue, cellBestScore, grid, cellInfo.CumulativeScore, cellInfo.x + 1, cellInfo.y);
            TryVisitCell(cellQueue, cellBestScore, grid, cellInfo.CumulativeScore, cellInfo.x - 1, cellInfo.y);
            TryVisitCell(cellQueue, cellBestScore, grid, cellInfo.CumulativeScore, cellInfo.x, cellInfo.y + 1);
            TryVisitCell(cellQueue, cellBestScore, grid, cellInfo.CumulativeScore, cellInfo.x, cellInfo.y - 1);
        }
    }

    return cellBestScore[K_GRID_CELL_COUNT - 1] - grid[0];
}

int main()
{
    std::vector<u8> grid(K_GRID_CELL_COUNT, 0);
    if (ReadInput(grid))
    {
        u32 bestScore{ ComputeBestPathScore(grid) };
        fmt::print("Best Score: {}.\n", bestScore);
    }
    else
    {
        fmt::print("Failed to open input file.\n");
    }
    return 0;
}