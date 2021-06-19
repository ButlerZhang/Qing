#pragma once
#include "../../PokerHands.h"

PAI_GOW_NS_BEGIN

class GoldenNuggetPokerHands : public PokerHands {
private:

    bool hasStraightFlush();
    bool hasFourOfAKind();
    bool hasFlush();
    bool hasStraight();

    bool isSplitTwoPairs(int firstPairIndex, int secondPairIndex);
    bool isUseSpecialTwoPairsInStraightOrFlush();
};

PAI_GOW_NS_END