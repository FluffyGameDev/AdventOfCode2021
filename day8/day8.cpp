#include <algorithm>
#include <fstream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <fmt/core.h>


using u8 = std::uint8_t;
using u32 = std::uint32_t;

static constexpr u32 K_INPUT_COUNT = 10;
static constexpr u32 K_OUTPUT_COUNT = 4;
static constexpr u32 K_DISPLAY_SEGMENT_COUNT = 7;
static constexpr u8 SEGMENT_MASK{ (1 << K_DISPLAY_SEGMENT_COUNT) - 1 };

struct NotesLine
{
	u8 Inputs[K_INPUT_COUNT];
	u8 Outputs[K_OUTPUT_COUNT];
};

using Notes = std::vector<NotesLine>;

u8 ReadDigit(const std::string& digitText)
{
	u8 digitValue{};
	for (char c : digitText)
	{
		u32 bitShiftAmount{ (u32)(c - 'a') };
		FMT_ASSERT(bitShiftAmount < K_DISPLAY_SEGMENT_COUNT, "Should not occur.");
		digitValue |= (1 << bitShiftAmount);
	}

	return digitValue;
}

void ReadNoteLine(NotesLine& notesLine, const std::string& lineText)
{
	std::string currentDigitText{};
	std::stringstream ss{ lineText };

	for (u32 i = 0; i < K_INPUT_COUNT; ++i)
	{
		ss >> currentDigitText;
		notesLine.Inputs[i] = ReadDigit(currentDigitText);
	}

	ss >> currentDigitText; // To ignore '|'

	for (u32 i = 0; i < K_OUTPUT_COUNT; ++i)
	{
		ss >> currentDigitText;
		notesLine.Outputs[i] = ReadDigit(currentDigitText);
	}
}

bool ReadInput(Notes& notes)
{
	static const char* inputFile{ "input.txt" };
	std::ifstream inputStream{ inputFile };

	bool readSucceeded{ inputStream.is_open() };
	if (readSucceeded)
	{
		std::string lineText;
		while (std::getline(inputStream, lineText))
		{
			NotesLine& notesLine{ notes.emplace_back() };
			ReadNoteLine(notesLine, lineText);
		}

		inputStream.close();
	}

	return readSucceeded;
}

u32 CountActiveSegmentCount(u8 digit)
{
	u32 segmentCount{};
	for (u32 i = 0; i < K_DISPLAY_SEGMENT_COUNT; ++i)
	{
		if (digit & (1 << i))
		{
			++segmentCount;
		}
	}
	return segmentCount;
}

bool IsSpecialSegmentCount(u32 segmentcount)
{
	return segmentcount == 2 ||
		segmentcount == 3 ||
		segmentcount == 4 ||
		segmentcount == 7;
}

u32 CountOutputSpecialDigits(const Notes& notes)
{
	u32 specialDigitCount{};

	for (const NotesLine& line : notes)
	{
		for (u8 digit : line.Outputs)
		{
			u32 segmentCount{ CountActiveSegmentCount(digit) };
			if (IsSpecialSegmentCount(segmentCount))
			{
				++specialDigitCount;
			}
		}
	}

	return specialDigitCount;
}

u8 PopMatchingPattern(std::vector<u8>& unmappedDigits, u8 pattern, u32 extraActiveSegments)
{
	u8 foundDigit{};
	for (u8 digit : unmappedDigits)
	{
		if ((digit & pattern & SEGMENT_MASK) == (pattern & SEGMENT_MASK))
		{
			if (CountActiveSegmentCount((digit & ~pattern) & SEGMENT_MASK) == extraActiveSegments)
			{
				foundDigit = digit;
				break;
			}
		}
	}

	unmappedDigits.erase(std::remove(unmappedDigits.begin(), unmappedDigits.end(), foundDigit));

	return foundDigit;
}

void DecodeInputMapping(const NotesLine& line, u8 num2Seg[K_INPUT_COUNT])
{
	std::vector<u8> unmappedDigits(line.Inputs, line.Inputs + K_INPUT_COUNT);

	static constexpr u8 NO_PATTERN{ 0 };
	num2Seg[1] = PopMatchingPattern(unmappedDigits, NO_PATTERN, 2);
	num2Seg[4] = PopMatchingPattern(unmappedDigits, NO_PATTERN, 4);
	num2Seg[7] = PopMatchingPattern(unmappedDigits, NO_PATTERN, 3);
	num2Seg[8] = PopMatchingPattern(unmappedDigits, NO_PATTERN, 7);

	num2Seg[0] = PopMatchingPattern(unmappedDigits, (num2Seg[8] & ~num2Seg[4]) | num2Seg[1], 1);
	num2Seg[2] = PopMatchingPattern(unmappedDigits, num2Seg[8] & ~num2Seg[4], 2);
	num2Seg[9] = PopMatchingPattern(unmappedDigits, num2Seg[4] | num2Seg[7], 1);

	u8 botSegment{ (u8)((num2Seg[9] & ~(num2Seg[4] | num2Seg[7])) & SEGMENT_MASK) };
	u8 midSegment{ (u8)(~num2Seg[0] & SEGMENT_MASK) };

	num2Seg[3] = PopMatchingPattern(unmappedDigits, num2Seg[7] | botSegment | midSegment, 0);
	num2Seg[6] = PopMatchingPattern(unmappedDigits, num2Seg[8] & ~num2Seg[1], 1);
	num2Seg[5] = unmappedDigits[0];
}

u8 FindMappedValue(u8 searchedDigit, u8 num2SegMapping[K_INPUT_COUNT])
{
	auto foundIt{ std::find(num2SegMapping, num2SegMapping + K_INPUT_COUNT, searchedDigit) };
	if (foundIt != num2SegMapping + K_INPUT_COUNT)
	{
		return foundIt - num2SegMapping;
	}

	FMT_ASSERT(false, "Failed to find mapping.");
	return 0;
}

u32 ComputeOutputValue(const NotesLine& line)
{
	u8 num2SegMapping[K_INPUT_COUNT]{};
	DecodeInputMapping(line, num2SegMapping);

	u32 outputValue{};
	for (u8 digit : line.Outputs)
	{
		u8 mappedValue{ FindMappedValue(digit, num2SegMapping) };
		outputValue = (outputValue * 10) + (u32)mappedValue;
	}
	return outputValue;
}

u32 CountTotalOutputValues(const Notes& notes)
{
	auto countOutputValue = [](u32 total, const NotesLine& line) { return total + ComputeOutputValue(line); };
	return std::accumulate(notes.begin(), notes.end(), 0, countOutputValue);
}

int main()
{
	Notes notes;
	if (ReadInput(notes))
	{
		u32 outputSpecialDigits{ CountOutputSpecialDigits(notes) };
		fmt::print("Output Special Digits (1, 4, 7, 8): {}.\n", outputSpecialDigits);

		u32 totalOutputValues{ CountTotalOutputValues(notes) };
		fmt::print("Total Output Values: {}.\n", totalOutputValues);
	}
	else
	{
		fmt::print("Failed to open input file.\n");
	}
	return 0;
}