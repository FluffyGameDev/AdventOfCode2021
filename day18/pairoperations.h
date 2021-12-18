#pragma once

#include <string_view>

#include "pair.h"
#include "pairregistry.h"

namespace Day18::Operations
{
    void DiplayPair(u32 pairID, const PairRegistry& registry);
    u32 ReadPair(std::string_view& rawText, PairRegistry& registry);
    u32 CopyPair(u32 pairID, PairRegistry& registry);
    bool ArePairsEqual(u32 pairID1, u32 pairID2, const PairRegistry& registry);
    bool ExplodePair(u32 pairID, PairRegistry& registry);
    bool SplitPair(u32 pairID, PairRegistry& registry);
    void ReducePair(u32 pairID, PairRegistry& registry);
    u32 AddPairs(u32 pairID1, u32 pairID2, PairRegistry& registry);
    u32 ComputePairMagnitude(u32 pairID, const PairRegistry& registry);
}