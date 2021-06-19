#pragma once
#include "../../PokerHands.h"

PAI_GOW_NS_BEGIN

class SantaAnaStarPokerHands : public PokerHands {
private:

    bool hasFiveAces();
    bool hasStraightFlush();
    bool hasFlush();
    bool hasStraight();
    bool hasFourOfAKind();

    bool isSplitFullHouse();
    bool isSplitTwoPairs(int firstPairIndex, int secondPairIndex);
};

PAI_GOW_NS_END