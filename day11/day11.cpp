#include <fstream>
#include <string>
#include <vector>
#include <Windows.h>

#include <fmt/core.h>


using u8 = std::uint8_t;
using i32 = std::uint32_t;

static constexpr i32 K_SIMULATION_LENGTH{ 100 };

struct Grid
{
    std::vector<u8> Cells{};
    std::vector<bool> FlashState{};
    i32 Width{};
    i32 Height{};
};

bool ReadInput(Grid& grid)
{
    static const char* inputFile{ "input.txt" };
    std::ifstream inputStream{ inputFile };

    bool readSucceeded{ inputStream.is_open() };
    if (readSucceeded)
    {
        static constexpr i32 gridWidth{ 10 };
        static constexpr i32 gridHeight{ 10 };
        grid.Width = gridWidth;
        grid.Height = gridHeight;
        grid.FlashState.resize(gridWidth * gridHeight, false);
        grid.Cells.reserve(gridWidth * gridHeight);

        std::string lineText;
        for (i32 j = 0; j < gridHeight; ++j)
        {
            std::getline(inputStream, lineText);
            for (i32 i = 0; i < gridWidth; ++i)
            {
                char c{ lineText[i] };
                if (std::isdigit(c))
                {
                    grid.Cells.push_back((u8)(c - '0'));
                }
            }
        }

        inputStream.close();
    }

    return readSucceeded;
}

void TryUpdateCell(Grid& grid, i32 x, i32 y)
{
    if (x >= 0 && x < grid.Width && y >= 0 && y < grid.Height)
    {
        ++grid.Cells[x + y * grid.Width];
    }
}

void FlashCell(Grid& grid, i32 x, i32 y)
{
    grid.FlashState[x + y * grid.Width] = true;

    TryUpdateCell(grid, x - 1, y - 1);
    TryUpdateCell(grid, x    , y - 1);
    TryUpdateCell(grid, x + 1, y - 1);
    TryUpdateCell(grid, x - 1, y    );
    TryUpdateCell(grid, x + 1, y    );
    TryUpdateCell(grid, x - 1, y + 1);
    TryUpdateCell(grid, x    , y + 1);
    TryUpdateCell(grid, x + 1, y + 1);
}

i32 ResolveFlashes(Grid& grid)
{
    i32 flashes{};
    i32 cellCount{ grid.Width * grid.Height };

    for (i32 j = 0; j < grid.Height; ++j)
    {
        for (i32 i = 0; i < grid.Width; ++i)
        {
            i32 cellIndex{ i + j * grid.Width };
            if (!grid.FlashState[cellIndex] && grid.Cells[cellIndex] > 9)
            {
                FlashCell(grid, i, j);
                ++flashes;
            }
        }
    }

    return flashes;
}

i32 SimulateGridIteration(Grid& grid)
{
    i32 flashCounter{};
    i32 cellCount{ grid.Width * grid.Height };
    for (i32 i = 0; i < cellCount; ++i)
    {
        grid.FlashState[i] = false;
        ++grid.Cells[i];
    }

    while (ResolveFlashes(grid) > 0);

    for (i32 i = 0; i < cellCount; ++i)
    {
        if (grid.FlashState[i])
        {
            grid.Cells[i] = 0;
            ++flashCounter;
        }
    }

    return flashCounter;
}

void DisplayGrid(const Grid& grid)
{
    HANDLE cout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    for (i32 j = 0; j < grid.Height; ++j)
    {
        for (i32 i = 0; i < grid.Width; ++i)
        {
            i32 cellIndex{ i + j * grid.Width };
            if (grid.FlashState[cellIndex])
            {
                SetConsoleTextAttribute(cout_handle, FOREGROUND_GREEN);
            }
            else
            {
                SetConsoleTextAttribute(cout_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            fmt::print("{}", grid.Cells[cellIndex]);
        }
        SetConsoleTextAttribute(cout_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        fmt::print("\n");
    }
}

int main()
{
    Grid grid{};
    if (ReadInput(grid))
    {
        i32 flashCounter{};
        for (i32 i = 0; i < K_SIMULATION_LENGTH; ++i)
        {
            flashCounter += SimulateGridIteration(grid);
        }
        DisplayGrid(grid);
        fmt::print("Flash Counter at {} steps: {}.\n\n", K_SIMULATION_LENGTH, flashCounter);

        i32 iterationCount{ K_SIMULATION_LENGTH };
        while (flashCounter != grid.Width * grid.Height)
        {
            flashCounter = SimulateGridIteration(grid);
            ++iterationCount;
        }

        DisplayGrid(grid);

        fmt::print("Iteration Count To Sync: {}.\n", iterationCount);
    }
    else
    {
        fmt::print("Failed to open input file.\n");
    }
    return 0;
}