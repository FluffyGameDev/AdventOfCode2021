#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <fmt/core.h>

using u32 = std::uint32_t;
using CaveID = std::uint64_t;

static constexpr u32 u32Max{ std::numeric_limits<u32>::max() };

static constexpr CaveID K_START_CAVE_ID{ 0x0000007472617473 };
static constexpr CaveID K_END_CAVE_ID{ 0x0000000000646e65 };

struct Cave
{
    std::vector<size_t> NearbyCaves;
    CaveID ID;
    bool IsSmall{};
};

struct CaveConnection
{
    CaveID Entrance;
    CaveID Exit;
};

struct CaveNetwork
{
    std::vector<Cave> Caves;
    size_t StartIndex{};
    size_t EndIndex{};
};

struct Route
{
    std::vector<size_t> CaveSequence{};
};

union CaveIDToText
{
    CaveID ID;
    char Text[sizeof(CaveID)];
};

CaveID ConvertTextToCaveID(const std::string_view& caveName)
{
    CaveID offset{ (CaveID)(8 * caveName.size()) };
    CaveID mask{ (1ULL << offset) - 1ULL };
    const CaveID* rawID{ reinterpret_cast<const CaveID*>(caveName.data()) };
    return (CaveID)(*rawID & mask);
}

void ReadCaveConnection(CaveConnection& connection, const std::string_view rawText)
{
    std::size_t separatorIndex{ rawText.find('-') };
    std::string_view entranceText{ rawText.substr(0, separatorIndex) };
    std::string_view exitText{ rawText.substr(separatorIndex + 1, rawText.size() - separatorIndex - 1)  };
    connection.Entrance = ConvertTextToCaveID(entranceText);
    connection.Exit = ConvertTextToCaveID(exitText);
}

bool ReadInput(std::vector<CaveConnection>& caveConnections)
{
    static const char* inputFile{ "input.txt" };
    std::ifstream inputStream{ inputFile };

    bool readSucceeded{ inputStream.is_open() };
    if (readSucceeded)
    {
        std::string lineText{};
        while (std::getline(inputStream, lineText))
        {
            CaveConnection& connection{ caveConnections.emplace_back() };
            ReadCaveConnection(connection, lineText);
        }

        inputStream.close();
    }

    return readSucceeded;
}

bool IsSmallCave(CaveID caveID)
{
    CaveIDToText converter;
    converter.ID = caveID;
    return std::islower(converter.Text[0]);
}

void BuildCaveSet(std::unordered_map<CaveID, size_t>& caveIDs, const std::vector<CaveConnection>& caveConnections)
{
    for (const CaveConnection& connection : caveConnections)
    {
        if (caveIDs.find(connection.Entrance) == caveIDs.end())
        {
            caveIDs[connection.Entrance] = caveIDs.size();
        }
        if (caveIDs.find(connection.Exit) == caveIDs.end())
        {
            caveIDs[connection.Exit] = caveIDs.size();
        }
    }
}

void BuildCaveList(std::vector<Cave>& caves, const std::unordered_map<CaveID, size_t>& caveIDs)
{
    size_t caveCount{ caveIDs.size() };
    caves.resize(caveCount);

    for (const auto& caveID : caveIDs)
    {
        caves[caveID.second].ID = caveID.first;
        caves[caveID.second].IsSmall = IsSmallCave(caveID.first);
    }
}

void ConnectCaves(std::vector<Cave>& caves, std::unordered_map<CaveID, size_t>& caveIDs, const std::vector<CaveConnection>& caveConnections)
{
    for (const CaveConnection& connection : caveConnections)
    {
        size_t entranceIndex{ caveIDs[connection.Entrance] };
        size_t exitIndex{ caveIDs[connection.Exit] };
        caves[entranceIndex].NearbyCaves.push_back(exitIndex);
        caves[exitIndex].NearbyCaves.push_back(entranceIndex);
    }
}

void BuildCaveNetwork(CaveNetwork& caveNetwork, const std::vector<CaveConnection>& caveConnections)
{
    std::unordered_map<CaveID, size_t> caveIDs{};
    BuildCaveSet(caveIDs, caveConnections);
    BuildCaveList(caveNetwork.Caves, caveIDs);
    ConnectCaves(caveNetwork.Caves, caveIDs, caveConnections);

    caveNetwork.StartIndex = caveIDs[K_START_CAVE_ID];
    caveNetwork.EndIndex = caveIDs[K_END_CAVE_ID];
}

void VisitCave(size_t caveIndex,
               u32 dualVisitCave,
               std::vector<const Cave*>& caveSequence,
               std::vector<u32>& visitCount,
               std::vector<Route>& routes,
               const CaveNetwork& caveNetwork)
{
    const Cave* currentCave{ &caveNetwork.Caves[caveIndex] };
    caveSequence.push_back(currentCave);

    if (caveIndex == caveNetwork.EndIndex)
    {
        Route& newRoute{ routes.emplace_back() };
        newRoute.CaveSequence.resize(caveSequence.size());

        auto caveToID = [](const Cave* cave) { return cave->ID; };
        std::transform(caveSequence.begin(), caveSequence.end(), newRoute.CaveSequence.begin(), caveToID);
    }
    else
    {
        ++visitCount[caveIndex];

        for (size_t nearbyCaveID : currentCave->NearbyCaves)
        {
            if (nearbyCaveID != caveNetwork.StartIndex)
            {
                const Cave* nearbyCave{ &caveNetwork.Caves[nearbyCaveID] };
                if (nearbyCave->IsSmall)
                {
                    if (visitCount[nearbyCaveID] == 0)
                    {
                        VisitCave(nearbyCaveID, dualVisitCave, caveSequence, visitCount, routes, caveNetwork);
                    }
                    else if (visitCount[nearbyCaveID] == 1 && dualVisitCave == u32Max)
                    {
                        VisitCave(nearbyCaveID, nearbyCaveID, caveSequence, visitCount, routes, caveNetwork);
                    }
                }
                else
                {
                    VisitCave(nearbyCaveID, dualVisitCave, caveSequence, visitCount, routes, caveNetwork);
                }
            }
        }

        --visitCount[caveIndex];
    }

    caveSequence.pop_back();
}

void ComputeAllPossibleRoutes(std::vector<Route>& routes, const CaveNetwork& caveNetwork)
{
    std::vector<const Cave*> caveSequence{};
    std::vector<u32> visitCount(caveNetwork.Caves.size(), 0);
    VisitCave(caveNetwork.StartIndex, u32Max, caveSequence, visitCount, routes, caveNetwork);
}

void DisplayRoute(const Route& route)
{
    CaveIDToText converter{};
    for (CaveID id : route.CaveSequence)
    {
        converter.ID = id;
        fmt::print("{}-", converter.Text);
    }
    fmt::print("\n");
}

int main()
{
    std::vector<CaveConnection> caveConnections;
    if (ReadInput(caveConnections))
    {
        CaveNetwork network{};
        BuildCaveNetwork(network, caveConnections);

        std::vector<Route> routes{};
        ComputeAllPossibleRoutes(routes, network);
        //std::for_each(routes.begin(), routes.end(), DisplayRoute);
        fmt::print("Possible Route Count: {}.\n", routes.size());
    }
    else
    {
        fmt::print("Failed to open input file.\n");
    }
    return 0;
}