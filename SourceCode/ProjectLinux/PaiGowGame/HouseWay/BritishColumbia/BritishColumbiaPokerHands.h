﻿#pragma once
#include "../../PokerHands.h"

PAI_GOW_NS_BEGIN

class BritishColumbiaPokerHands : public PokerHands {
private:

    bool hasStraightFlush();
    bool hasFlush();
    bool hasStraight();

    bool isSplitFourOfAKind(int fourOfAKindIndex, int threeOfAKindIndex);
    bool isSplitTwoPairs(int firstPairIndex, int secondPairIndex);
    bool isExceptionOnePairInStraightOrFlush(int pair, const std::vector<int> &oldResultCardArray);
    bool isStraightIncludeAceAndJoker(int firstCard, int sixthCard, int seventhCard);
};

PAI_GOW_NS_END