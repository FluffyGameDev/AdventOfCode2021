#include <fstream>
#include <string>
#include <unordered_map>

#include <fmt/core.h>

using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using InsertionRules = std::unordered_map<u16, char>;
using FrequencyMap = std::unordered_map<u16, u64>;

void ReadInsertionRule(const std::string& rawText, InsertionRules& insertionRules)
{
    char input[2]{};
    char output{};
    sscanf_s(rawText.c_str(), "%c%c -> %c", &input[0], 1, &input[1], 1, &output, 1);
    insertionRules[*((u16*)input)] = output;
}

bool ReadInput(std::string& polymerTemplate, InsertionRules& insertionRules)
{
    static const char* inputFile{ "input.txt" };
    std::ifstream inputStream{ inputFile };

    bool readSucceeded{ inputStream.is_open() };
    if (readSucceeded)
    {
        std::getline(inputStream, polymerTemplate);

        std::string lineText{};
        std::getline(inputStream, lineText);
        while (std::getline(inputStream, lineText))
        {
            ReadInsertionRule(lineText, insertionRules);
        }

        inputStream.close();
    }

    return readSucceeded;
}

void InitFrequencyMap(const std::string& polymerTemplate, FrequencyMap& frequencyMap)
{
    size_t maxIndex{ polymerTemplate.size() - 1 };
    for (size_t i = 0; i < maxIndex; ++i)
    {
        const u16* elementPair{ (const u16*)&polymerTemplate[i] };
        ++frequencyMap[*elementPair];
    }
}

void ComputeOutputElements(InsertionRules& insertionRules,
                           u16 input, u16& output1, u16& output2)
{
    const char* ruleIndividualElements{ (const char*)&input };
    char outputElement{ insertionRules[input] };

    char* output1Chars{ (char*)&output1 };
    output1Chars[0] = ruleIndividualElements[0];
    output1Chars[1] = outputElement;

    char* output2Chars{ (char*)&output2 };
    output2Chars[0] = outputElement;
    output2Chars[1] = ruleIndividualElements[1];
}

void SimulateStep(InsertionRules& insertionRules, FrequencyMap& frequencyMap)
{
    FrequencyMap newElements{};
    for (auto& frequencyPair : frequencyMap)
    {
        u16 output1{};
        u16 output2{};
        if (frequencyPair.second > 0)
        {
            ComputeOutputElements(insertionRules, frequencyPair.first, output1, output2);
            newElements[output1] += frequencyPair.second;
            newElements[output2] += frequencyPair.second;
            frequencyPair.second = 0;
        }
    }

    for (const auto& newElement : newElements)
    {
        frequencyMap[newElement.first] += newElement.second;
    }
}

void SimulateNSteps(InsertionRules& insertionRules,
                   u32 stepCount,
                   FrequencyMap& frequencyMap)
{
    for (u32 i = 0; i < stepCount; ++i)
    {
        SimulateStep(insertionRules, frequencyMap);
    }
}

void ComputeElementsFrequency(const std::string& polymerTemplate,
                              const FrequencyMap& pairFrequencyMap,
                              FrequencyMap& elementsFrequency)
{
    for (const auto& pair : pairFrequencyMap)
    {
        const char* elements{ (const char*)&pair.first };
        elementsFrequency[elements[0]] += pair.second;
        elementsFrequency[elements[1]] += pair.second;
    }

    for (auto& element : elementsFrequency)
    {
        element.second /= 2;
    }

    ++elementsFrequency[polymerTemplate[0]];
    ++elementsFrequency[polymerTemplate[polymerTemplate.size() - 1]];
}

std::tuple<u64, u64> ComputeElementMinMaxQuantities(const FrequencyMap& elementsFrequency)
{
    u64 minQuantity{ std::numeric_limits<u64>::max() };
    u64 maxQuantity{ 0 };

    for (const auto& element : elementsFrequency)
    {
        minQuantity = std::min(minQuantity, element.second);
        maxQuantity = std::max(maxQuantity, element.second);
    }

    return { minQuantity, maxQuantity };
}

int main()
{
    std::string polymerTemplate{};
    InsertionRules insertionRules{};
    if (ReadInput(polymerTemplate, insertionRules))
    {
        FrequencyMap frequencyMap{};
        InitFrequencyMap(polymerTemplate, frequencyMap);
        SimulateNSteps(insertionRules, 10, frequencyMap);

        {
            FrequencyMap elementsFrequency{};
            ComputeElementsFrequency(polymerTemplate, frequencyMap, elementsFrequency);

            auto [minQuantity, maxQuantity] { ComputeElementMinMaxQuantities(elementsFrequency) };
            fmt::print("---------- 10 Iterations ----------\n", minQuantity);
            fmt::print("Min Element Quantity: {}.\n", minQuantity);
            fmt::print("Max Element Quantity: {}.\n", maxQuantity);
            fmt::print("Diff: {}.\n", maxQuantity - minQuantity);
        }

        SimulateNSteps(insertionRules, 30, frequencyMap);

        {
            FrequencyMap elementsFrequency{};
            ComputeElementsFrequency(polymerTemplate, frequencyMap, elementsFrequency);

            auto [minQuantity, maxQuantity] { ComputeElementMinMaxQuantities(elementsFrequency) };
            fmt::print("---------- 40 Iterations ----------\n", minQuantity);
            fmt::print("Min Element Quantity: {}.\n", minQuantity);
            fmt::print("Max Element Quantity: {}.\n", maxQuantity);
            fmt::print("Diff: {}.\n", maxQuantity - minQuantity);
        }
    }
    else
    {
        fmt::print("Failed to open input file.\n");
    }
    return 0;
}