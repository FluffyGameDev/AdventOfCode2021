#include <algorithm>
#include <array>
#include <fstream>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <fmt/core.h>

using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct BingoBoardCell
{
	BingoBoardCell(u8 value)
		: Value{ value }
		, HasBeenCalled{ false }
	{
	}

	u8 Value;
	bool HasBeenCalled;
};

struct BingoBoard
{
	static constexpr u32 K_GRID_WIDTH{ 5 };
	static constexpr u32 K_CELL_COUNT{ K_GRID_WIDTH * K_GRID_WIDTH };

	std::array<std::weak_ptr<BingoBoardCell>, K_CELL_COUNT> Cells;
};

struct BingoBoardWinnerData
{
	BingoBoardWinnerData(const BingoBoard* board, u32 lastCalledNumber, u32 score)
		: Board{ board }
		, LastCalledNumber{ lastCalledNumber }
		, Score{ score }
	{
	}

	const BingoBoard* Board{};
	u32 LastCalledNumber{};
	u32 Score{};
};

class BingoCellLocator
{
public:
	BingoCellLocator()
	{
		for (u8 i = 0; i < K_MAX_CELL_VALUE; ++i)
		{
			m_Numbers[i] = std::make_shared<BingoBoardCell>(i);
		}
	}

	void CallNumber(u8 number)
	{
		m_Numbers[number]->HasBeenCalled = true;
	}

	std::shared_ptr<BingoBoardCell> GetCell(u8 cellValue) const
	{
		return m_Numbers[cellValue];
	}

private:
	static constexpr u8 K_MAX_CELL_VALUE{ 100 };

	std::array<std::shared_ptr<BingoBoardCell>, K_MAX_CELL_VALUE> m_Numbers;
};

void ParseCalledNumbers(const std::string& rawText, std::vector<u8>& calledNumbers)
{
	std::stringstream ss(rawText);
	std::string number_as_string;
	while (std::getline(ss, number_as_string, ','))
	{
		calledNumbers.push_back(std::stoi(number_as_string));
	}
}

void ReadBingoBoard(std::istream& inputStream, BingoBoard& board, const BingoCellLocator& cellLocator)
{
	for (u32 i = 0; i < BingoBoard::K_CELL_COUNT; ++i)
	{
		u32 cellValue{};
		inputStream >> cellValue;
		board.Cells[i] = cellLocator.GetCell(cellValue);
	}
}

bool ReadInput(std::vector<u8>& calledNumbers, std::vector<BingoBoard>& boards, const BingoCellLocator& cellLocator)
{
	static const char* inputFile{ "input.txt" };
	std::ifstream inputStream{ inputFile };

	bool readSucceeded{ inputStream.is_open() };
	if (readSucceeded)
	{
		std::string calledNumbersText{};
		inputStream >> calledNumbersText;
		ParseCalledNumbers(calledNumbersText, calledNumbers);

		static constexpr u32 estimatedBoardCount{ 100 };
		boards.reserve(estimatedBoardCount);
		while (!inputStream.eof())
		{
			BingoBoard& newBoard{ boards.emplace_back() };
			ReadBingoBoard(inputStream, newBoard, cellLocator);
		}

		inputStream.close();
	}

	return readSucceeded;
}

bool BoardHasACompleteLine(const BingoBoard& board)
{
	for (u32 i = 0; i < BingoBoard::K_GRID_WIDTH; ++i)
	{
		bool completeRow{ true };
		bool completeColumn{ true };

		for (u32 j = 0; j < BingoBoard::K_GRID_WIDTH; ++j)
		{
			completeRow &= board.Cells[i + (size_t)j * BingoBoard::K_GRID_WIDTH].lock()->HasBeenCalled;
			completeColumn &= board.Cells[j + (size_t)i * BingoBoard::K_GRID_WIDTH].lock()->HasBeenCalled;
		}

		if (completeRow || completeColumn)
		{
			return true;
		}
	}
	return false;
}

void CheckForWinner(const std::vector<const BingoBoard*>& boards, std::vector<const BingoBoard*>& winners)
{
	const BingoBoard* winner{};
	for (const BingoBoard* currentBoard : boards)
	{
		if (BoardHasACompleteLine(*currentBoard))
		{
			winners.push_back(currentBoard);
		}
	}
}

u32 ComputeBoardScore(const BingoBoard& board, u32 lastCalledNumber)
{
	auto countUncalled = [](u32 total, const std::weak_ptr<BingoBoardCell>& cell)
	{
		const std::shared_ptr<BingoBoardCell> cellShared{ cell.lock() };
		total += (!cellShared->HasBeenCalled ? cellShared->Value : 0);
		return total;
	};
	return std::accumulate(board.Cells.begin(), board.Cells.end(), 0, countUncalled) * lastCalledNumber;
}

std::vector<BingoBoardWinnerData> ComputeWinners(const std::vector<u8>& calledNumbers, const std::vector<BingoBoard>& boards, BingoCellLocator& cellLocator)
{
	std::vector<BingoBoardWinnerData> winners{};
	std::vector<const BingoBoard*> roundWinners{};
	std::vector<const BingoBoard*> remainingBoards{};
	remainingBoards.resize(boards.size());

	auto boardRefToPointer = [](const BingoBoard& board) { return &board; };
	std::transform(boards.begin(), boards.end(), remainingBoards.begin(), boardRefToPointer);

	const BingoBoard* winner{};
	//The first 4 numbers could be ignored for better performance.
	for (u8 number : calledNumbers)
	{
		cellLocator.CallNumber(number);

		CheckForWinner(remainingBoards, roundWinners);
		for (const BingoBoard* winner : roundWinners)
		{
			remainingBoards.erase(std::remove(remainingBoards.begin(), remainingBoards.end(), winner));

			u32 winnerScore{ ComputeBoardScore(*winner, number) };
			winners.emplace_back(winner, number, winnerScore);
		}
		roundWinners.clear();
	}

	return winners;
}

int main()
{
	std::vector<u8> calledNumbers{};
	std::vector<BingoBoard> boards{};
	BingoCellLocator cellLocator{};

	if (ReadInput(calledNumbers, boards, cellLocator))
	{
		std::vector<BingoBoardWinnerData> winners{ ComputeWinners(calledNumbers, boards, cellLocator) };

		if (winners.size() > 0)
		{
			for (const BingoBoardWinnerData& winnerData : winners)
			{
				fmt::print("Winner Score: {}\n", winnerData.Score);
			}
		}
		else
		{
			fmt::print("Failed to find a winner.\n");
		}
	}
	else
	{
		fmt::print("Failed to open input file.\n");
	}

	return 0;
}