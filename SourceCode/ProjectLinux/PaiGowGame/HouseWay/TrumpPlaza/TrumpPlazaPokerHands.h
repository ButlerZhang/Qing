#pragma once
#include "../../PokerHands.h"

PAI_GOW_NS_BEGIN

class TrumpPlazaPokerHands : public PokerHands {
private:

    bool hasStraightFlush();
    bool hasFlush();
    bool hasStraight();

    bool isSplitFourOfAKind(int fourOfAKindIndex, int threeOfAKindIndex);
    bool isSplitTwoPairs(int firstPairIndex, int secondPairIndex);
};

PAI_GOW_NS_END