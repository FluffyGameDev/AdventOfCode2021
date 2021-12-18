#pragma once

#include <vector>
#include "pair.h"

namespace Day18
{
    class PairRegistry
    {
    public:
        PairRegistry();

        u32 AllocatePair();
        Pair& GetPair(u32 pairID);
        const Pair& GetPair(u32 pairID) const;
        u32 GetPairCount() const;

    private:
        std::vector<Pair> m_Pairs;
    };
}