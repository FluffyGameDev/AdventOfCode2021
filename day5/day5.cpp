#include <algorithm>
#include <cstdio>
#include <vector>

#include <fmt/core.h>

using u8 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

static constexpr size_t K_GRID_WIDTH{ 1000 };

struct Vec2
{
	i32 x{};
	i32 y{};
};

struct Line
{
	Vec2 Start{};
	Vec2 End{};
};

Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
	return { v1.x - v2.x, v1.y - v2.y };
}

i32 Abs(i32 value)
{
	return (value < 0 ? -value : value);
}



bool ReadInput(std::vector<Line>& lines, bool ignoreDiagonals)
{
	static const char* inputFile{ "input.txt" };
	FILE* fileHandle{};
	fopen_s(&fileHandle, inputFile, "r");
	bool readSucceeded{ fileHandle != nullptr };

	if (readSucceeded)
	{
		Line newLine{};
		while (fscanf_s(fileHandle, "%d,%d -> %d,%d\n", &newLine.Start.x, &newLine.Start.y, &newLine.End.x, &newLine.End.y) == 4)
		{
			bool isDiagonal{ newLine.Start.x != newLine.End.x && newLine.Start.y != newLine.End.y };
			if (!ignoreDiagonals || !isDiagonal)
			{
				lines.emplace_back(newLine);
			}
		}

		fclose(fileHandle);
	}

	return readSucceeded;
}

Vec2 ComputeManhattanUnit(const Vec2& v)
{
	return {
		v.x != 0 ? v.x / Abs(v.x) : 0,
		v.y != 0 ? v.y / Abs(v.y) : 0
	};
}

void DrawLineOnGrid(std::vector<u8>& grid, const Line& line)
{
	Vec2 unit{ ComputeManhattanUnit(line.End - line.Start) };
	Vec2 curPos{ line.Start };

	while (curPos.x != line.End.x || curPos.y != line.End.y)
	{
		++grid[curPos.x + curPos.y * K_GRID_WIDTH];

		curPos.x += unit.x;
		curPos.y += unit.y;
	}

	if (line.Start.x != line.End.x || line.Start.y != line.End.y)
	{
		++grid[curPos.x + curPos.y * K_GRID_WIDTH];
	}
}

void DrawLinesOnGrid(std::vector<u8>& grid, const std::vector<Line>& lines)
{
	for (const Line& line : lines)
	{
		DrawLineOnGrid(grid, line);
	}
}

u64 ComputeIntersectionCount(const std::vector<u8>& grid)
{
	auto hasMultipleIntersections = [](u8 cellCount) { return cellCount > 1; };
	return (u64)std::count_if(grid.begin(), grid.end(), hasMultipleIntersections);
}

int main()
{
	std::vector<Line> lines{};
	if (ReadInput(lines, /*ignoreDiagonals*/false))
	{
		static constexpr size_t gridCellCount{ K_GRID_WIDTH * K_GRID_WIDTH };
		std::vector<u8> grid(gridCellCount, 0);

		// Brute forcing the hell out of this problem.
		// Could be a lot more efficient with some vector math.
		DrawLinesOnGrid(grid, lines);

		u64 intersectionCount{ ComputeIntersectionCount(grid) };

		fmt::print("Unique Intersection Count: {}.\n", intersectionCount);
	}
	else
	{
		fmt::print("Failed to open input file.\n");
	}

	return 0;
}