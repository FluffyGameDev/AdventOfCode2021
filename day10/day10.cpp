#include <fstream>
#include <numeric>
#include <stack>
#include <string>
#include <vector>

#include <fmt/core.h>


using u64 = std::uint64_t;

struct LineSyntaxReport
{
    bool IsValid{};
    char IlllegalChar{};
    std::string RequiredAutoComplete{};
};

bool ReadInput(std::vector<std::string>& lines)
{
    static const char* inputFile{ "input.txt" };
    std::ifstream inputStream{ inputFile };

    bool readSucceeded{ inputStream.is_open() };
    if (readSucceeded)
    {
        std::string lineText;
        while (std::getline(inputStream, lineText))
        {
            lines.push_back(std::move(lineText));
        }

        inputStream.close();
    }

    return readSucceeded;
}

void ComputeLineSyntaxReport(const std::string& line, LineSyntaxReport& syntaxReport)
{
    std::vector<char> expectedChars{};

    for (char c : line)
    {
        switch (c)
        {
            case '(': { expectedChars.push_back(')'); break; }
            case '[': { expectedChars.push_back(']'); break; }
            case '{': { expectedChars.push_back('}'); break; }
            case '<': { expectedChars.push_back('>'); break; }

            case ')':
            case ']':
            case '}':
            case '>':
            {
                if (c != expectedChars.back())
                {
                    syntaxReport.IsValid = false;
                    syntaxReport.IlllegalChar = c;
                    return;
                }
                expectedChars.pop_back();
                break;
            }
        }
    }

    syntaxReport.IsValid = expectedChars.empty();
    std::copy(expectedChars.rbegin(), expectedChars.rend(), std::back_inserter(syntaxReport.RequiredAutoComplete));

    return;
}

u64 ComputeIllegalCharErrorScore(char illegalCharacter)
{
    u64 errorScore{};

    switch (illegalCharacter)
    {
        case ')': { errorScore = 3; break; }
        case ']': { errorScore = 57; break; }
        case '}': { errorScore = 1197; break; }
        case '>': { errorScore = 25137; break; }
    }

    return errorScore;
}

u64 ComputeAutoCorrectScore(const std::string& autoCorrectText)
{
    u64 score{};

    for (char c : autoCorrectText)
    {
        score *= 5;
        switch (c)
        {
            case ')': { score += 1; break; }
            case ']': { score += 2; break; }
            case '}': { score += 3; break; }
            case '>': { score += 4; break; }
        }
    }

    return score;
}

std::tuple<u64, u64> ComputeLinesScores(const std::vector<std::string>& lines)
{
    u64 errorTotal{};

    std::vector<u64> autoCorrectScores{};

    for (const std::string& line : lines)
    {
        LineSyntaxReport report{};
        ComputeLineSyntaxReport(line, report);

        if (!report.IsValid)
        {
            if (report.IlllegalChar != 0)
            {
                errorTotal += ComputeIllegalCharErrorScore(report.IlllegalChar);
            }
            else
            {
                autoCorrectScores.push_back(ComputeAutoCorrectScore(report.RequiredAutoComplete));
            }
        }
    }

    size_t midElementIndex{ autoCorrectScores.size() / 2 };
    std::nth_element(autoCorrectScores.begin(), autoCorrectScores.begin() + midElementIndex, autoCorrectScores.end());
    u64 midAutocorrectScore{ autoCorrectScores[midElementIndex] };

    return { errorTotal, midAutocorrectScore };
}

int main()
{
    std::vector<std::string> lines{};
    if (ReadInput(lines))
    {
        auto [errorTotal, midAutocorrectScore] { ComputeLinesScores(lines) };
        fmt::print("Total Syntax Error Score: {}.\n", errorTotal);
        fmt::print("Middle Auto-correct Score: {}.\n", midAutocorrectScore);
    }
    else
    {
        fmt::print("Failed to open input file.\n");
    }
    return 0;
}