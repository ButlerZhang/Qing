﻿#pragma once
#include "../../PokerHands.h"

PAI_GOW_NS_BEGIN

class FlamingoHiltonPokerHands : public PokerHands {
private:

    bool hasStraightFlush();
    bool hasFourOfAKind();
    bool hasFlush();
    bool hasStraight();

    bool isSplitTwoPairs(int firstPairIndex, int secondPairIndex);
    bool isUseSpecialTwoPairsInStraightOrFlush(PokerHandsType biggestType);
};

PAI_GOW_NS_END