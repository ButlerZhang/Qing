﻿#pragma once
#include "../../PokerHands.h"

PAI_GOW_NS_BEGIN

class BallyPokerHands : public PokerHands {
private:

    bool hasStraightFlush();
    bool hasFlush();
    bool hasStraight();

    bool isSplitTwoPairs(int firstPairIndex, int secondPairIndex);
    bool isSplitFourOfAKind(int fourOfAKindIndex, int threeOfAKindIndex);
    bool isStraightIncludeAceAndJoker(int firstCard, int sixthCard, int seventhCard);
};

PAI_GOW_NS_END