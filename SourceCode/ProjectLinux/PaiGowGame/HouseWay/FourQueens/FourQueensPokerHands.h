#pragma once
#include "../../PokerHands.h"

PAI_GOW_NS_BEGIN

class FourQueensPokerHands : public PokerHands {
private:

    bool hasStraightFlush();
    bool hasFlush();
    bool hasStraight();
    bool hasFourOfAKind();

    bool isSplitTwoPairs(int firstPairIndex, int secondPairIndex);
    bool isUseSpecialTwoPairsInStraightOrFlush();
};

PAI_GOW_NS_END