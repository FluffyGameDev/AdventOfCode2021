#include "pairregistry.h"

namespace Day18
{
    PairRegistry::PairRegistry()
    {
        m_Pairs.reserve(200000); //Just to be safe XD
    }

    u32 PairRegistry::AllocatePair()
    {
        u32 newID{ (u32)m_Pairs.size() };
        m_Pairs.emplace_back();
        return newID;
    }

    Pair& PairRegistry::GetPair(u32 pairID)
    {
        return m_Pairs[pairID];
    }

    const Pair& PairRegistry::GetPair(u32 pairID) const
    {
        return m_Pairs[pairID];
    }

    u32 PairRegistry::GetPairCount() const
    {
        return (u32)m_Pairs.size();
    }
}