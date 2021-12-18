#pragma once

#include "types.h"

namespace Day18
{
    enum class ValueType
    {
        Regular,
        SubPair
    };

    struct PairValue
    {
        ValueType Type;
        u32 Value;
    };

    struct Pair
    {
        PairValue LeftValue;
        PairValue RightValue;
    };
}