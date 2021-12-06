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

bool IsInRange(i32 x, i32 a, i32 b)
{
	i32 minValue{ std::min(a, b) };
	i32 maxValue{ std::max(a, b) };
	return minValue <= x && x <= maxValue;
}

i32 DotProduct(const Vec2& v1, const Vec2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
	return { v1.x - v2.x, v1.y - v2.y };
}

bool AreDirectionsParallel(const Vec2& dir1, const Vec2& dir2)
{
	i32 dotProduct{ DotProduct(dir1, dir2) };
	i32 lenSquared1{ DotProduct(dir1, dir1) };
	i32 lenSquared2{ DotProduct(dir2, dir2) };

	return (lenSquared1 * lenSquared2 == dotProduct * dotProduct);
}

Vec2 ComputeManhattanUnit(const Vec2& v)
{
	return {
		v.x != 0 ? v.x / std::abs(v.x) : 0,
		v.y != 0 ? v.y / std::abs(v.y) : 0
	};
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

void FindIntersectionPoints(const Line& line1, const Line& line2, std::vector<Vec2>& intersections)
{
	if (AreDirectionsParallel(line1.End - line1.Start, line2.End - line2.Start))
	{
		if (AreDirectionsParallel(line2.Start - line1.Start, line2.End - line1.Start))
		{
			Vec2 unit{ ComputeManhattanUnit(line1.End - line1.Start) };
			Vec2 currentPosition{ line1.Start };
			while (currentPosition.x != line1.End.x || currentPosition.y != line1.End.y)
			{
				// Could be made more efficient by iterating only over the points we need to add.
				if (IsInRange(currentPosition.x, line2.Start.x, line2.End.x) &&
					IsInRange(currentPosition.y, line2.Start.y, line2.End.y))
				{
					intersections.push_back(currentPosition);
				}

				currentPosition.x += unit.x;
				currentPosition.y += unit.y;
			}

			if (IsInRange(currentPosition.x, line2.Start.x, line2.End.x) &&
				IsInRange(currentPosition.y, line2.Start.y, line2.End.y))
			{
				intersections.push_back(currentPosition);
			}
		}
	}
	else
	{
		Vec2 unit{ ComputeManhattanUnit(line1.End - line1.Start) };
		Vec2 currentPosition{ line1.Start };
		while (currentPosition.x != line1.End.x || currentPosition.y != line1.End.y)
		{
			// Need a way to compute intersection point with proper math...
			Vec2 d1{ currentPosition - line2.Start };
			Vec2 d2{ currentPosition - line2.End };
			if (AreDirectionsParallel(d1, d2) &&
				DotProduct(d1, d2) <= 0)
			{
				intersections.push_back(currentPosition);
				break;
			}

			currentPosition.x += unit.x;
			currentPosition.y += unit.y;
		}

		{
			Vec2 d1{ currentPosition - line2.Start };
			Vec2 d2{ currentPosition - line2.End };
			if (AreDirectionsParallel(d1, d2) &&
				DotProduct(d1, d2) < 0)
			{
				intersections.push_back(currentPosition);
			}
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
	if (ReadInput(lines, /*ignoreDiagonals*/false))
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