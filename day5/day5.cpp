#include <cstdio>
#include <set>
#include <vector>

#include <fmt/core.h>

using i32 = std::int32_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

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

bool ReadInput(std::vector<Line>& lines)
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
			lines.emplace_back(newLine);
		}

		fclose(fileHandle);
	}

	return readSucceeded;
}

bool IsInRange(i32 x, i32 a, i32 b)
{
	if (a < b)
	{
		return a <= x && x <= b;
	}
	else
	{
		return a >= x && x >= b;
	}
}

void FindIntersectionPoints(const Line& line1, const Line& line2, std::vector<Vec2>& intersections)
{
	bool line1Hor{ line1.Start.y == line1.End.y };
	bool line1Ver{ line1.Start.x == line1.End.x };
	bool line2Hor{ line2.Start.y == line2.End.y };
	bool line2Ver{ line2.Start.x == line2.End.x };

	bool line1Valid{ line1Hor || line1Ver };
	bool line2Valid{ line2Hor || line2Ver };
	if (!line1Valid || !line2Valid) return;

	if (line1Hor && line2Hor)
	{
		if (line1.Start.y == line2.Start.y)
		{
			i32 line1Left{ std::min(line1.Start.x, line1.End.x) };
			i32 line1Right{ std::max(line1.Start.x, line1.End.x) };
			i32 line2Left{ std::min(line2.Start.x, line2.End.x) };
			i32 line2Right{ std::max(line2.Start.x, line2.End.x) };

			if (line1Left <= line2Right || line1Right >= line2Left)
			{
				i32 innerLeft{ std::max(line1Left, line2Left) };
				i32 innerRight{ std::min(line1Right, line2Right) };
				for (i32 i = innerLeft; i <= innerRight; ++i)
				{
					Vec2 point{ i, line1.Start.y };
					intersections.push_back(point);
				}
			}
		}
	}
	else if (line1Ver && line2Ver)
	{
		if (line1.Start.x == line2.Start.x)
		{
			i32 line1Top{ std::min(line1.Start.y, line1.End.y) };
			i32 line1Bottom{ std::max(line1.Start.y, line1.End.y) };
			i32 line2Top{ std::min(line2.Start.y, line2.End.y) };
			i32 line2Bottom{ std::max(line2.Start.y, line2.End.y) };

			if (line1Top <= line2Bottom || line1Bottom >= line2Top)
			{
				i32 innerTop{ std::max(line1Top, line2Top) };
				i32 innerBottom{ std::min(line1Bottom, line2Bottom) };
				for (i32 i = innerTop; i <= innerBottom; ++i)
				{
					Vec2 point{ line1.Start.x, i };
					intersections.push_back(point);
				}
			}
		}
	}
	else
	{
		const Line& lineHor{ line1Hor ? line1 : line2 };
		const Line& lineVer{ line1Hor ? line2 : line1 };
		Vec2 intersectionPoint{ lineVer.Start.x, lineHor.Start.y };

		if (IsInRange(intersectionPoint.x, lineHor.Start.x, lineHor.End.x) &&
			IsInRange(intersectionPoint.y, lineVer.Start.y, lineVer.End.y))
		{
			intersections.push_back(intersectionPoint);
		}
	}
}

void ComputeLineIntersections(const std::vector<Line>& lines, std::vector<Vec2>& intersections)
{
	const auto endIt{ lines.end() };
	for (auto line1It = lines.begin(); line1It != endIt; ++line1It)
	{
		for (auto line2It = line1It + 1; line2It != endIt; ++line2It)
		{
			FindIntersectionPoints(*line1It, *line2It, intersections);
		}
	}
}

u32 ComputeUniqueIntersectionCount(const std::vector<Vec2>& intersections)
{
	std::set<u64> uniqueIntersections;

	for (const Vec2& position : intersections)
	{
		const u64& positionHash{ reinterpret_cast<const u64&>(position) };
		uniqueIntersections.insert(positionHash);
	}

	return (u32)uniqueIntersections.size();
}

int main()
{
	std::vector<Line> lines{};
	if (ReadInput(lines))
	{
		std::vector<Vec2> intersections{};
		ComputeLineIntersections(lines, intersections);

		u32 intersectionCount{ ComputeUniqueIntersectionCount(intersections) };
		fmt::print("Unique Intersection Count: {}.\n", intersectionCount);
	}
	else
	{
		fmt::print("Failed to open input file.\n");
	}

	return 0;
}