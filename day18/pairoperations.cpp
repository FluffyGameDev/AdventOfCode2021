#include "pairoperations.h"

#include <stack>
#include <fmt/core.h>

namespace Day18::Operations
{
    namespace Internal
    {
        static void Expect(std::string_view& rawText, char expectedChar)
        {
            FMT_ASSERT(rawText.size() > 0 && rawText[0] == expectedChar, "Unexpected char.");
            rawText.remove_prefix(1);
        }

        static PairValue ReadValue(std::string_view& rawText, PairRegistry& registry)
        {
            u32 value{};
            ValueType type{};

            if (std::isdigit(rawText[0]))
            {
                type = ValueType::Regular;
                
                size_t numberLength{};
                value = (u32)(std::stoll(&rawText[0], &numberLength));
                rawText.remove_prefix(numberLength);
            }
            else
            {
                type = ValueType::SubPair;
                value = ReadPair(rawText, registry);
            }

            return { type, value };
        }

        bool FindExplodablePair(u32 rootPair, u32& explodablePair, std::stack<u32>& pairStack, PairRegistry& registry)
        {
            bool foundExplodable{};
            Pair& pair{ registry.GetPair(rootPair) };
            if (pair.LeftValue.Type == ValueType::Regular &&
                pair.RightValue.Type == ValueType::Regular)
            {
                if (pairStack.size() >= 4)
                {
                    foundExplodable = true;
                    explodablePair = rootPair;
                }
            }
            else
            {
                pairStack.push(rootPair);
                if (pair.LeftValue.Type == ValueType::SubPair)
                {
                    foundExplodable = FindExplodablePair(pair.LeftValue.Value, explodablePair, pairStack, registry);
                }
                if (!foundExplodable && pair.RightValue.Type == ValueType::SubPair)
                {
                    foundExplodable = FindExplodablePair(pair.RightValue.Value, explodablePair, pairStack, registry);
                }
                if (!foundExplodable)
                {
                    pairStack.pop();
                }
            }
            return foundExplodable;
        }

        Pair* FindLeftRegularValue(u32 pairID, PairRegistry& registry)
        {
            Pair& pair{ registry.GetPair(pairID) };
            if (pair.LeftValue.Type == ValueType::Regular)
            {
                return &pair;
            }
            else
            {
                return FindLeftRegularValue(pair.LeftValue.Value, registry);
            }
        }

        Pair* FindRightRegularValue(u32 pairID, PairRegistry& registry)
        {
            Pair& pair{ registry.GetPair(pairID) };
            if (pair.RightValue.Type == ValueType::Regular)
            {
                return &pair;
            }
            else
            {
                return FindRightRegularValue(pair.RightValue.Value, registry);
            }
        }

        void PropagateExplosionLeft(u32 explodablePairID, std::stack<u32> pairStack, PairRegistry& registry)
        {
            u32 previousID{ explodablePairID };
            Pair& explodablePair{ registry.GetPair(explodablePairID) };
            while (!pairStack.empty())
            {
                Pair& currentPair{ registry.GetPair(pairStack.top()) };
                if (currentPair.LeftValue.Type == ValueType::SubPair)
                {
                    if (currentPair.LeftValue.Value != previousID)
                    {
                        Pair* foundLeftPair{ FindRightRegularValue(currentPair.LeftValue.Value, registry) };
                        if (foundLeftPair != &explodablePair)
                        {
                            foundLeftPair->RightValue.Value += explodablePair.LeftValue.Value;
                            return;
                        }
                    }
                }
                else
                {
                    currentPair.LeftValue.Value += explodablePair.LeftValue.Value;
                    return;
                }

                previousID = pairStack.top();
                pairStack.pop();
            }
        }

        void PropagateExplosionRight(u32 explodablePairID, std::stack<u32> pairStack, PairRegistry& registry)
        {
            u32 previousID{ explodablePairID };
            Pair& explodablePair{ registry.GetPair(explodablePairID) };
            while (!pairStack.empty())
            {
                Pair& currentPair{ registry.GetPair(pairStack.top()) };
                if (currentPair.RightValue.Type == ValueType::SubPair)
                {
                    if (currentPair.RightValue.Value != previousID)
                    {
                        Pair* foundRightPair{ FindLeftRegularValue(currentPair.RightValue.Value, registry) };
                        if (foundRightPair != &explodablePair)
                        {
                            foundRightPair->LeftValue.Value += explodablePair.RightValue.Value;
                            return;
                        }
                    }
                }
                else
                {
                    currentPair.RightValue.Value += explodablePair.RightValue.Value;
                    return;
                }

                previousID = pairStack.top();
                pairStack.pop();
            }
        }

        static bool SplitValue(PairValue& value, PairRegistry& registry)
        {
            bool didSplit{};
            if (value.Type == ValueType::Regular)
            {
                if (value.Value >= 10)
                {
                    didSplit = true;

                    u32 newPairID{ registry.AllocatePair() };
                    Pair& newPair{ registry.GetPair(newPairID) };

                    newPair.LeftValue.Type = ValueType::Regular;
                    newPair.LeftValue.Value = value.Value / 2;
                    newPair.RightValue.Type = ValueType::Regular;
                    newPair.RightValue.Value = value.Value - newPair.LeftValue.Value;

                    value.Type = ValueType::SubPair;
                    value.Value = newPairID;
                }
            }
            else
            {
                didSplit = SplitPair(value.Value, registry);
            }
            return didSplit;
        }
    }

    void DiplayPair(u32 pairID, const PairRegistry& registry)
    {
        const Pair& pair{ registry.GetPair(pairID) };

        fmt::print("[");
        if (pair.LeftValue.Type == ValueType::Regular)
        {
            fmt::print("{}", pair.LeftValue.Value);
        }
        else
        {
            DiplayPair(pair.LeftValue.Value, registry);
        }
        fmt::print(",");
        if (pair.RightValue.Type == ValueType::Regular)
        {
            fmt::print("{}", pair.RightValue.Value);
        }
        else
        {
            DiplayPair(pair.RightValue.Value, registry);
        }
        fmt::print("]");
    }

    u32 ReadPair(std::string_view& rawText, PairRegistry& registry)
    {
        u32 pairID{ registry.AllocatePair() };

        Internal::Expect(rawText, '[');
        PairValue leftValue{ Internal::ReadValue(rawText, registry) };
        Internal::Expect(rawText, ',');
        PairValue rightValue{ Internal::ReadValue(rawText, registry) };
        Internal::Expect(rawText, ']');

        Pair& pair{ registry.GetPair(pairID) };
        pair.LeftValue = leftValue;
        pair.RightValue = rightValue;

        return pairID;
    }

    u32 CopyPair(u32 pairID, PairRegistry& registry)
    {
        u32 newPairID{ registry.AllocatePair() };

        Pair& pair{ registry.GetPair(pairID) };

        ValueType leftType{ pair.LeftValue.Type };
        u32 leftValue{ pair.LeftValue.Value };
        ValueType rightType{ pair.RightValue.Type };
        u32 rightValue{ pair.RightValue.Value };

        if (leftType == ValueType::SubPair)
        {
            leftValue = CopyPair(leftValue, registry);
        }
        if (rightType == ValueType::SubPair)
        {
            rightValue = CopyPair(rightValue, registry);
        }

        Pair& pairCopy{ registry.GetPair(newPairID) };
        pairCopy.LeftValue.Type = leftType;
        pairCopy.LeftValue.Value = leftValue;
        pairCopy.RightValue.Type = rightType;
        pairCopy.RightValue.Value = rightValue;

        return newPairID;
    }

    bool ArePairsEqual(u32 pairID1, u32 pairID2, const PairRegistry& registry)
    {
        const Pair& pair1{ registry.GetPair(pairID1) };
        const Pair& pair2{ registry.GetPair(pairID2) };

        if (pair1.LeftValue.Type == pair2.LeftValue.Type)
        {
            if (pair1.LeftValue.Type == ValueType::Regular)
            {
                if (pair1.LeftValue.Value != pair2.LeftValue.Value)
                {
                    return false;
                }
            }
            else
            {
                return ArePairsEqual(pair1.LeftValue.Value, pair2.LeftValue.Value, registry);
            }
        }
        else
        {
            return false;
        }

        if (pair1.RightValue.Type == pair2.RightValue.Type)
        {
            if (pair1.RightValue.Type == ValueType::Regular)
            {
                if (pair1.RightValue.Value != pair2.RightValue.Value)
                {
                    return false;
                }
            }
            else
            {
                return ArePairsEqual(pair1.RightValue.Value, pair2.RightValue.Value, registry);
            }
        }
        else
        {
            return false;
        }

        return true;
    }

    bool ExplodePair(u32 pairID, PairRegistry& registry)
    {
        bool didExplosion{};

        std::stack<u32> pairStack{};
        u32 explodablePairID{};
        if (Internal::FindExplodablePair(pairID, explodablePairID, pairStack, registry))
        {
            didExplosion = true;

            Internal::PropagateExplosionLeft(explodablePairID, pairStack, registry);
            Internal::PropagateExplosionRight(explodablePairID, pairStack, registry);

            Pair& parentPair{ registry.GetPair(pairStack.top()) };
            if (parentPair.LeftValue.Type == ValueType::SubPair && parentPair.LeftValue.Value == explodablePairID)
            {
                parentPair.LeftValue.Type = ValueType::Regular;
                parentPair.LeftValue.Value = 0;
            }
            else
            {
                parentPair.RightValue.Type = ValueType::Regular;
                parentPair.RightValue.Value = 0;
            }
        }


        return didExplosion;
    }

    bool SplitPair(u32 pairID, PairRegistry& registry)
    {
        Pair& pair{ registry.GetPair(pairID) };
        return Internal::SplitValue(pair.LeftValue, registry) ||
               Internal::SplitValue(pair.RightValue, registry);
    }

    void ReducePair(u32 pairID, PairRegistry& registry)
    {
        while (ExplodePair(pairID, registry) || SplitPair(pairID, registry));
    }

    u32 AddPairs(u32 pairID1, u32 pairID2, PairRegistry& registry)
    {
        u32 additionPairID{ registry.AllocatePair() };
        Pair& additionPair{ registry.GetPair(additionPairID) };

        additionPair.LeftValue.Type = ValueType::SubPair;
        additionPair.LeftValue.Value = pairID1;
        additionPair.RightValue.Type = ValueType::SubPair;
        additionPair.RightValue.Value = pairID2;

        return additionPairID;
    }

    u32 ComputePairMagnitude(u32 pairID, const PairRegistry& registry)
    {
        u32 totalMagnitude{};
        const Pair& pair{ registry.GetPair(pairID) };

        if (pair.LeftValue.Type == ValueType::Regular)
        {
            totalMagnitude += pair.LeftValue.Value * 3;
        }
        else
        {
            totalMagnitude += ComputePairMagnitude(pair.LeftValue.Value, registry) * 3;
        }

        if (pair.RightValue.Type == ValueType::Regular)
        {
            totalMagnitude += pair.RightValue.Value * 2;
        }
        else
        {
            totalMagnitude += ComputePairMagnitude(pair.RightValue.Value, registry) * 2;
        }

        return totalMagnitude;
    }
}