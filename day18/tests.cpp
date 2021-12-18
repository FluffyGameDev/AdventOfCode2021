#include "tests.h"

#include <fmt/core.h>

#include "pair.h"
#include "pairoperations.h"
#include "pairregistry.h"

namespace Day18
{
    namespace Tests
    {
        static bool ValidateReadLine()
        {
            PairRegistry registry{};
            u32 pairID{ Operations::ReadPair(std::string_view("[[1,[2,3]],14]"), registry) };

            bool isValid{ registry.GetPairCount() == 3 };
            if (isValid)
            {
                Pair& rootPair{ registry.GetPair(pairID) };
                isValid &= rootPair.LeftValue.Type == ValueType::SubPair;
                isValid &= rootPair.RightValue.Type == ValueType::Regular;
                isValid &= rootPair.RightValue.Value == 14;

                if (isValid)
                {
                    Pair& leftPair{ registry.GetPair(rootPair.LeftValue.Value) };
                    isValid &= leftPair.LeftValue.Type == ValueType::Regular;
                    isValid &= leftPair.LeftValue.Value == 1;
                    isValid &= leftPair.RightValue.Type == ValueType::SubPair;

                    if (isValid)
                    {
                        Pair& rightPair{ registry.GetPair(leftPair.RightValue.Value) };
                        isValid &= rightPair.LeftValue.Type == ValueType::Regular;
                        isValid &= rightPair.LeftValue.Value == 2;
                        isValid &= rightPair.RightValue.Type == ValueType::Regular;
                        isValid &= rightPair.RightValue.Value == 3;
                    }
                }
            }


            if (!isValid)
            {
                fmt::print("Failed 'ReadLine' Test.\n");
            }

            return isValid;
        }

        static bool ValidateCopy()
        {
            PairRegistry registry{};
            u32 pairID{ Operations::ReadPair(std::string_view("[[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]"), registry) };
            u32 expectedID{ Operations::ReadPair(std::string_view("[[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]"), registry) };

            u32 copy{ Operations::CopyPair(pairID, registry) };

            bool isValid{ Operations::ArePairsEqual(copy, expectedID, registry) };
            if (!isValid)
            {
                fmt::print("Result: '");
                Operations::DiplayPair(copy, registry);
                fmt::print("'\n");
                fmt::print("Expected: '");
                Operations::DiplayPair(expectedID, registry);
                fmt::print("'\n");

                fmt::print("Failed 'Copy' Test.\n");
            }

            return isValid;
        }

        static bool ValidateComparison()
        {
            PairRegistry registry{};
            u32 pairID1{ Operations::ReadPair(std::string_view("[[1,[2,3]],4]"), registry) };
            u32 pairID2{ Operations::ReadPair(std::string_view("[[1,[2,4]],4]"), registry) };
            u32 pairID3{ Operations::ReadPair(std::string_view("[[1,[2,3]],4]"), registry) };

            bool isValid{ true };
            isValid &= !Operations::ArePairsEqual(pairID1, pairID2, registry);
            isValid &= Operations::ArePairsEqual(pairID1, pairID3, registry);
            isValid &= !Operations::ArePairsEqual(pairID2, pairID3, registry);

            if (!isValid)
            {
                fmt::print("Failed 'Comparison' Test.\n");
            }

            return isValid;
        }

        static bool ValidateExplode()
        {
            PairRegistry registry{};
            u32 pairID{ Operations::ReadPair(std::string_view("[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]"), registry) };
            u32 comparisonPairID{ Operations::ReadPair(std::string_view("[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]"), registry) };
            u32 noExplosionPairID{ Operations::ReadPair(std::string_view("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]"), registry) };

            bool isValid{ true };
            isValid &= Operations::ExplodePair(pairID, registry);
            isValid &= !Operations::ExplodePair(noExplosionPairID, registry);
            isValid &= Operations::ArePairsEqual(pairID, comparisonPairID, registry);

            if (!isValid)
            {
                fmt::print("Failed 'Explosion' Test.\n");
            }
            return isValid;
        }

        static bool ValidateSplit()
        {
            PairRegistry registry{};
            u32 pairID{ Operations::ReadPair(std::string_view("[[[[0,7],4],[[7,8],[0,13]]],[1,1]]"), registry) };
            u32 comparisonPairID{ Operations::ReadPair(std::string_view("[[[[0,7],4],[[7,8],[0,[6,7]]]],[1,1]]"), registry) };
            u32 noSplitPairID{ Operations::ReadPair(std::string_view("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]"), registry) };

            bool isValid{ true };
            isValid &= Operations::SplitPair(pairID, registry);
            isValid &= !Operations::SplitPair(noSplitPairID, registry);
            isValid &= Operations::ArePairsEqual(pairID, comparisonPairID, registry);

            if (!isValid)
            {
                fmt::print("Failed 'Split' Test.\n");
            }
            return isValid;
        }

        static bool ValidateReduce()
        {
            PairRegistry registry{};
            u32 pairID{ Operations::ReadPair(std::string_view("[[[[[4,3],4],4],[7,[[8,4],9]]],[1,1]]"), registry) };
            u32 comparisonPairID{ Operations::ReadPair(std::string_view("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]"), registry) };

            Operations::ReducePair(pairID, registry);

            bool isValid{ Operations::ArePairsEqual(pairID, comparisonPairID, registry) };
            if (!isValid)
            {
                fmt::print("Failed 'Reduce' Test.\n");
            }
            return isValid;
        }

        static bool ValidateAddition()
        {
            PairRegistry registry{};
            u32 pairID1{ Operations::ReadPair(std::string_view("[1,2]"), registry) };
            u32 pairID2{ Operations::ReadPair(std::string_view("[[3,4],5]"), registry) };
            u32 expectedPairID{ Operations::ReadPair(std::string_view("[[1,2],[[3,4],5]]"), registry) };
            u32 addResultPairID{ Operations::AddPairs(pairID1, pairID2, registry) };

            bool isValid{ Operations::ArePairsEqual(addResultPairID, expectedPairID, registry) };
            if (!isValid)
            {
                fmt::print("Failed 'Addition' Test.\n");
            }

            return isValid;
        }

        static bool ValidateMagnitude()
        {
            PairRegistry registry{};
            u32 pairID{ Operations::ReadPair(std::string_view("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]"), registry) };

            bool isValid{ Operations::ComputePairMagnitude(pairID, registry) == 3488 };
            if (!isValid)
            {
                fmt::print("Failed 'Magnitude' Test.\n");
            }

            return isValid;
        }

        static bool ValidatePart2()
        {
            PairRegistry registry{};
            u32 pairID1{ Operations::ReadPair(std::string_view("[[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]"), registry) };
            u32 pairID2{ Operations::ReadPair(std::string_view("[[[0,[5,8]],[[1,7],[9,6]]],[[4,[1,2]],[[1,4],2]]]"), registry) };
            u32 expectedPairID{ Operations::ReadPair(std::string_view("[[[[7,8],[6,6]],[[6,0],[7,7]]],[[[7,8],[8,8]],[[7,9],[0,6]]]]"), registry) };

            u32 sum{ Day18::Operations::AddPairs(pairID1, pairID2, registry) };
            Day18::Operations::ReducePair(sum, registry);

            bool isValid{ true };
            isValid &= Operations::ArePairsEqual(expectedPairID, sum, registry);
            isValid &= Operations::ComputePairMagnitude(sum, registry) == 3993;

            if (!isValid)
            {
                fmt::print("Failed 'Part2' Test.\n");
            }

            return isValid;
        }
    }

    bool ValidateTests()
    {
        bool testsValid{ true };
        testsValid &= Tests::ValidateReadLine();
        testsValid &= Tests::ValidateCopy();
        testsValid &= Tests::ValidateComparison();
        testsValid &= Tests::ValidateExplode();
        testsValid &= Tests::ValidateSplit();
        testsValid &= Tests::ValidateReduce();
        testsValid &= Tests::ValidateAddition();
        testsValid &= Tests::ValidateMagnitude();
        testsValid &= Tests::ValidatePart2();
        return testsValid;
    }
}