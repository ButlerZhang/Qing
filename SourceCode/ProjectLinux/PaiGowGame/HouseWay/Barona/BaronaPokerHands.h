﻿#pragma once
#include "../../PokerHands.h"

PAI_GOW_NS_BEGIN

class BaronaPokerHands : public PokerHands {
private:

    bool hasStraightFlush();
    bool hasFlush();
    bool hasStraight();

    bool isSplitFullHouse();
    bool isSplitTwoPairs(int firstPairIndex, int secondPairIndex);
    bool isSplitFourOfAKind(int fourOfAKindIndex, int threeOfAKindIndex);
};

PAI_GOW_NS_END