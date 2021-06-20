﻿#include "CanterburyParkPokerHands.h"

PAI_GOW_NS_BEGIN

bool CanterburyParkPokerHands::hasFiveAces() {
    //必须要有鬼牌，且Ace必须有四张
    if (getCardCount(CARD_VALUE_JOKER) != 1 || getCardCount(CARD_SPADE_ACE) != 4) {
        return false;
    }

    //先组装结果
    resultCardArray[0] = CARD_SPADE_ACE;
    resultCardArray[1] = CARD_HEART_ACE;
    resultCardArray[2] = CARD_CLUB_ACE;
    resultCardArray[3] = CARD_DIAMOND_ACE;
    resultCardArray[4] = CARD_VALUE_JOKER;

    //如果只判断五张牌，那么直接返回结果
    if (getCardCount(CARD_VALUE_COUNT) == HIGH_HAND_CARD_COUNT) {
        fiveCardsType = PHT_FIVE_ACES;
        return true;
    }

    //保存最高牌型
    sevenCardsArray = resultCardArray;

    //始终把三条A放在左手牌，一对A放在右手牌
    resultCardArray[2] = CARD_VALUE_JOKER;
    resultCardArray[5] = CARD_CLUB_ACE;
    resultCardArray[6] = CARD_DIAMOND_ACE;
    resultCardArray[3] = resultCardArray[4] = 0;
    addRestCardsToResultCardArray(2);

    //需要判断左手牌是否存在对子
    PokerHandsType fiveType = isPair(resultCardArray[3], resultCardArray[4]) ? PHT_FULL_HOUSE : PHT_THREE_OF_A_KIND;
    setType(PHT_FIVE_ACES, fiveType, PHT_ONE_PAIR);
    return true;
}

bool CanterburyParkPokerHands::hasStraightFlush() {
    //相同花色的牌至少要有五张
    int straightFlushCardCount = 5;
    int flushIndex = getFlushIndex(straightFlushCardCount);
    if (flushIndex < 0) {
        return false;
    }

    //必须能组成顺子
    int straightCardCount = getLongestStraight(flushIndex);
    if (straightCardCount < 0) {
        return false;
    }

    //如果只是判断五张牌，直接返回结果，需要判断是不是皇家同花顺
    int cardTotalCount = getCardCount(CARD_VALUE_COUNT);
    PokerHandsType highCardType = isRoyalFlushWhenResultCardArrayIsStraightFlush() ? PHT_ROYAL_FLUSH : PHT_STRAIGHT_FLUSH;
    if (cardTotalCount == HIGH_HAND_CARD_COUNT) {
        fiveCardsType = highCardType;
        return true;
    }

    //把剩余的牌放到右手牌，同时备份结果，方便后续判断
    addRestCardsToResultCardArray(cardTotalCount - straightCardCount);
    sevenCardsArray = resultCardArray;
    cleanResultCardArray();

    //三条或普通对子：如果右手牌已经是对子，直接返回结果
    if (isPair(sevenCardsArray[5], sevenCardsArray[6])) {
        setType(highCardType, highCardType, PHT_ONE_PAIR);
        resultCardArray = sevenCardsArray;
        return true;
    }

    //两对：如果有两对则使用两对规则
    if (hasTwoPair()) {
        sevenCardsType = highCardType;
        return true;
    }

    //一对：如果使用一对规则，那剩余的五张牌必须能凑成顺子、同花或同花顺
    if (hasOnePair()) {
        if (isUseOnPairInStraightOrFlush(resultCardArray, sevenCardsArray)) {
            setType(highCardType, fiveCardsType, PHT_ONE_PAIR);
            return true;
        }
    }

    //7张同花顺时只需把最大牌移到右手牌
    if (straightCardCount == 7) {
        vectorCopy(sevenCardsArray, 1, resultCardArray, 0, HIGH_HAND_CARD_COUNT);
        resultCardArray[5] = sevenCardsArray[0];
        resultCardArray[6] = sevenCardsArray[6];
        setType(highCardType, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD);
        return true;
    }

    //6张同花顺时把最大牌移到右手牌
    int firstCard = getCardValueWithoutSuit(sevenCardsArray[0]);
    int sixthCard = getCardValueWithoutSuit(sevenCardsArray[5]);
    int seventhCard = getCardValueWithoutSuit(sevenCardsArray[6]);
    if (straightCardCount == 6) {
        resultCardArray = sevenCardsArray;
        if (firstCard < seventhCard && sixthCard < seventhCard) {
            //如果最后一张牌大于第一张牌和第六张牌，只需要对调右手两张牌即可
            int tempCard = resultCardArray[5];
            resultCardArray[5] = resultCardArray[6];
            resultCardArray[6] = tempCard;
        }
        else if (sixthCard < getCardValueWithoutSuit(CARD_SPADE_ACE)) {
            //否则把最大牌放到右手牌
            resultCardArray[5] = sevenCardsArray[0];
            resultCardArray[6] = sevenCardsArray[6];
            vectorCopy(sevenCardsArray, 1, resultCardArray, 0, HIGH_HAND_CARD_COUNT);
        }

        //移动之后，依然是同花顺
        setType(highCardType, isRoyalFlushWhenResultCardArrayIsStraightFlush() ? PHT_ROYAL_FLUSH : PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD);
        return true;
    }

    //5张同花顺时，尝试把最大张牌放到右手牌，判断剩余的牌能否凑成同花或顺子
    if (sixthCard <= firstCard) { //如果最大牌已经在右手牌，那就不需要判断了
        cleanResultCardArray();

        //尝试移动一张牌
        bool isChanged = true;
        if (getSuit(sevenCardsArray[6]) == flushIndex) {
            //最小的牌与左手牌相同花色
            resultCardArray[5] = sevenCardsArray[0];
            resultCardArray[6] = sevenCardsArray[5];
            addRestCardsToResultCardArray(5);
        }
        else if (getSuit(sevenCardsArray[5]) == flushIndex) {
            //第五张牌与左手牌相同花色
            resultCardArray[5] = sevenCardsArray[0];
            resultCardArray[6] = sevenCardsArray[6];
            addRestCardsToResultCardArray(5);
        }
        else if (getSmallerCardValue(sevenCardsArray[4]) == getCardValueWithoutSuit(sevenCardsArray[5])) {
            //前五张牌与第六张牌凑成顺子
            vectorCopy(sevenCardsArray, 1, resultCardArray, 0, HIGH_HAND_CARD_COUNT);
            resultCardArray[5] = sevenCardsArray[0];
            resultCardArray[6] = sevenCardsArray[6];
        }
        else if (getSmallerCardValue(sevenCardsArray[4]) == getCardValueWithoutSuit(sevenCardsArray[6])) {
            //前五张牌与第七张牌凑成顺子
            vectorCopy(sevenCardsArray, 1, resultCardArray, 0, HIGH_HAND_CARD_COUNT - 1);
            resultCardArray[4] = sevenCardsArray[6];
            resultCardArray[5] = sevenCardsArray[0];
            resultCardArray[6] = sevenCardsArray[5];
        }
        else {
            isChanged = false;
        }

        if (isChanged && isFiveCardsFlushOrStraightInResultCardArray()) {
            setType(highCardType, fiveCardsType, isPair(resultCardArray[5], resultCardArray[6]) ? PHT_ONE_PAIR : PHT_HIGH_CARD);
            return true;
        }

        //尝试移动两张牌
        vectorCopy(sevenCardsArray, 2, resultCardArray, 0, HIGH_HAND_CARD_COUNT);
        resultCardArray[5] = sevenCardsArray[0];
        resultCardArray[6] = sevenCardsArray[1];
        if (isFiveCardsFlushOrStraightInResultCardArray()) {
            setType(highCardType, fiveCardsType, PHT_HIGH_CARD);
            return true;
        }
    }

    //排除上面所有情况后，必定是打同花顺了
    resultCardArray = sevenCardsArray;
    setType(highCardType, highCardType, PHT_HIGH_CARD);
    return true;
}

bool CanterburyParkPokerHands::hasFlush() {
    //同花有5张牌
    int flushCardCount = 5;

    //相同花色的牌是否至少有五张
    int flushIndex = getFlushIndex(flushCardCount);
    if (flushIndex < 0) {
        return false;
    }

    //组装结果牌数组
    int startIndex = getCardValueWithoutSuit(CARD_VALUE_JOKER);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_2) - 1;
    int nextResultIndex = addDifferentCardsToResultCardArray(startIndex, stopIndex, flushIndex);

    //如果只判断五张牌，可以返回结果了
    int cardTotalCount = getCardCount(CARD_VALUE_COUNT);
    if (cardTotalCount == HIGH_HAND_CARD_COUNT) {
        fiveCardsType = PHT_FLUSH;
        return true;
    }

    //把剩余的牌加入到结果数组，同时备份结果，方便后续判断
    addRestCardsToResultCardArray(cardTotalCount - nextResultIndex);
    sevenCardsArray = resultCardArray;
    cleanResultCardArray();

    //三条或普通对子：如果右手牌已经是对子，直接返回结果
    if (isPair(sevenCardsArray[5], sevenCardsArray[6])) {
        setType(PHT_FLUSH, PHT_FLUSH, PHT_ONE_PAIR);
        resultCardArray = sevenCardsArray;
        return true;
    }

    //两对：如果有两对则使用两对规则
    if (hasTwoPair()) {
        sevenCardsType = PHT_FLUSH;
        return true;
    }

    //一对：如果使用一对规则，那剩余的五张牌必须能凑成同花或顺子
    if (hasOnePair()) {
        if (isUseOnPairInStraightOrFlush(resultCardArray, sevenCardsArray)) {
            setType(PHT_FLUSH, fiveCardsType, PHT_ONE_PAIR);
            return true;
        }
    }

    //7张同花，把最大牌放到右手牌
    if (nextResultIndex == 7) {
        vectorCopy(sevenCardsArray, 1, resultCardArray, 0, HIGH_HAND_CARD_COUNT);
        resultCardArray[5] = sevenCardsArray[0];
        resultCardArray[6] = sevenCardsArray[6];
        setType(PHT_FLUSH, PHT_FLUSH, PHT_HIGH_CARD);
        return true;
    }

    //6张同花，把最大牌移到右手牌
    int firstCard = getCardValueWithoutSuit(sevenCardsArray[0]);
    int sixthCard = getCardValueWithoutSuit(sevenCardsArray[5]);
    int seventhCard = getCardValueWithoutSuit(sevenCardsArray[6]);
    if (nextResultIndex == 6) {
        resultCardArray = sevenCardsArray;
        if (firstCard < seventhCard && sixthCard < seventhCard) {
            //如果最后一张牌大于第一张牌和第六张牌，只需要对调右手两张牌即可
            int tempCard = resultCardArray[5];
            resultCardArray[5] = resultCardArray[6];
            resultCardArray[6] = tempCard;
        }
        else {
            //只需要把最大牌放到右手牌
            resultCardArray[5] = sevenCardsArray[0];
            vectorCopy(sevenCardsArray, 1, resultCardArray, 0, HIGH_HAND_CARD_COUNT);
        }

        setType(PHT_FLUSH, PHT_FLUSH, PHT_HIGH_CARD);
        return true;
    }

    //5张同花时，尝试把最大张牌放到右手牌，判断剩余的牌能否凑成顺子
    if (sixthCard <= firstCard) { //如果最大牌已经在右手牌，那就不需要判断了
        cleanResultCardArray();

        //尝试移动一张牌
        bool isChanged = true;
        if (getSmallerCardValue(sevenCardsArray[4]) == getCardValueWithoutSuit(sevenCardsArray[5])) {
            //前五张牌与第六张牌凑成顺子
            vectorCopy(sevenCardsArray, 1, resultCardArray, 0, HIGH_HAND_CARD_COUNT);
            resultCardArray[5] = sevenCardsArray[0];
            resultCardArray[6] = sevenCardsArray[6];
        }
        else if (getSmallerCardValue(sevenCardsArray[4]) == getCardValueWithoutSuit(sevenCardsArray[6])) {
            //前五张牌与第七张牌凑成顺子
            vectorCopy(sevenCardsArray, 1, resultCardArray, 0, HIGH_HAND_CARD_COUNT - 1);
            resultCardArray[4] = sevenCardsArray[6];
            resultCardArray[5] = sevenCardsArray[0];
            resultCardArray[6] = sevenCardsArray[5];
        }
        else {
            isChanged = false;
        }

        if (isChanged && isFiveCardsFlushOrStraightInResultCardArray()) {
            setType(PHT_FLUSH, fiveCardsType, isPair(resultCardArray[5], resultCardArray[6]) ? PHT_ONE_PAIR : PHT_HIGH_CARD);
            return true;
        }

        //尝试移动两张牌
        vectorCopy(sevenCardsArray, 2, resultCardArray, 0, HIGH_HAND_CARD_COUNT);
        resultCardArray[5] = sevenCardsArray[0];
        resultCardArray[6] = sevenCardsArray[1];
        if (isFiveCardsFlushOrStraightInResultCardArray()) {
            setType(PHT_FLUSH, fiveCardsType, PHT_HIGH_CARD);
            return true;
        }
    }

    //排除上面所有情况后，直接打同花
    resultCardArray = sevenCardsArray;
    setType(PHT_FLUSH, PHT_FLUSH, PHT_HIGH_CARD);
    return true;
}

bool CanterburyParkPokerHands::hasStraight() {
    //先判断能否找到顺子
    int straightCardCount = getLongestStraight(CS_COUNT);
    if (straightCardCount < 0) {
        return false;
    }

    //如果只是判断五张牌，直接返回结果
    int cardTotalCount = getCardCount(CARD_VALUE_COUNT);
    if (cardTotalCount == HIGH_HAND_CARD_COUNT) {
        fiveCardsType = PHT_STRAIGHT;
        return true;
    }

    //把剩余的牌放到右手牌，同时备份结果，方便后续判断
    addRestCardsToResultCardArray(cardTotalCount - straightCardCount);
    sevenCardsArray = resultCardArray;
    cleanResultCardArray();

    //三条或普通对子：如果右手牌已经是对子，直接返回结果
    if (isPair(sevenCardsArray[5], sevenCardsArray[6])) {
        setType(PHT_STRAIGHT, PHT_STRAIGHT, PHT_ONE_PAIR);
        resultCardArray = sevenCardsArray;
        return true;
    }

    //两对：如果有两对则使用两对规则
    if (hasTwoPair()) {
        sevenCardsType = PHT_STRAIGHT;
        return true;
    }

    //一对：如果使用一对规则，那剩余的五张牌必须能凑成顺子
    if (hasOnePair()) {
        if (isUseOnPairInStraightOrFlush(resultCardArray, sevenCardsArray)) {
            setType(PHT_STRAIGHT, fiveCardsType, PHT_ONE_PAIR);
            return true;
        }
    }

    //7张顺子，只需要把最大牌移到右手牌
    if (straightCardCount == 7) {
        resultCardArray[5] = sevenCardsArray[0];
        resultCardArray[6] = sevenCardsArray[1];
        vectorCopy(sevenCardsArray, 2, resultCardArray, 0, HIGH_HAND_CARD_COUNT);
        setType(PHT_STRAIGHT, PHT_STRAIGHT, PHT_HIGH_CARD);
        return true;
    }

    //6张牌顺子
    resultCardArray = sevenCardsArray;
    if (straightCardCount == 6) {
        int firstCard = getCardValueWithoutSuit(resultCardArray[0]);
        int sixthCard = getCardValueWithoutSuit(resultCardArray[5]);
        int seventhCard = getCardValueWithoutSuit(resultCardArray[6]);

        if (firstCard < seventhCard && sixthCard < seventhCard) {
            //如果最后一张牌大于第一张牌和第六张牌，只需要对调右手两张牌即可
            int tempCard = resultCardArray[5];
            resultCardArray[5] = resultCardArray[6];
            resultCardArray[6] = tempCard;
        }
        else if (sixthCard < getCardValueWithoutSuit(CARD_SPADE_ACE)) {
            //只需要把最大牌放到右手牌
            resultCardArray[5] = sevenCardsArray[0];
            vectorCopy(sevenCardsArray, 1, resultCardArray, 0, HIGH_HAND_CARD_COUNT);
        }
    }

    //来到这里就直接打顺子牌了
    setType(PHT_STRAIGHT, PHT_STRAIGHT, PHT_HIGH_CARD);
    return true;
}

bool CanterburyParkPokerHands::hasFourOfAKind() {
    //四条有四张牌
    int fourOfAKindCardCount = 4;
    bool isNeedJoker = false;

    //寻找普通情况的四张牌，从大到小方向寻找
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_2) - 1;
    int fourOfAKindIndex = getSpecialCardCountIndex(startIndex, stopIndex, fourOfAKindCardCount);
    if (fourOfAKindIndex < 0) {
        if (getCardCount(CARD_VALUE_JOKER) <= 0) {
            return false;
        }

        //有Joker的情况下，三张A也满足条件
        isNeedJoker = true;
        fourOfAKindIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
        if (divideCardArray[fourOfAKindIndex][CS_COUNT] != 3) {
            return false;
        }
    }

    //保存四条
    addSameCardsToResultCardArray(fourOfAKindIndex, 0);
    if (isNeedJoker) addJokerToResultCardArray(fourOfAKindCardCount);

    //保存最高牌型
    sevenCardsArray = resultCardArray;

    //如果只判断五张牌，可以直接返回结果了
    if (getCardCount(CARD_VALUE_COUNT) == HIGH_HAND_CARD_COUNT) {
        addRestCardsToResultCardArray(1);
        fiveCardsType = PHT_FOUR_OF_A_KIND;
        return true;
    }

    //4+3牌型，这里不需要考虑AA+Joker，因为它可以当做4+2+1牌型
    int threeOfAKindCardCount = 3;
    int tempStartIndex = isNeedJoker ? (startIndex - 1) : startIndex;
    int threeOfAKindIndex = getSpecialCardCountIndex(tempStartIndex, stopIndex, threeOfAKindCardCount);
    if (threeOfAKindIndex > 0) {
        if (fourOfAKindIndex > threeOfAKindIndex) {
            cleanResultCardArray();
            addSameCardsToResultCardArray(threeOfAKindIndex, 0);
            addSameCardsToResultCardArray(fourOfAKindIndex, threeOfAKindCardCount);
            if (isNeedJoker) addJokerToResultCardArray(DEALING_CARD_COUNT_EACH);
            setType(PHT_FOUR_OF_A_KIND, PHT_FULL_HOUSE, PHT_ONE_PAIR);
        }
        else {
            addSameCardsToResultCardArray(threeOfAKindIndex, fourOfAKindCardCount);
            setType(PHT_FOUR_OF_A_KIND, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR);
        }

        return true;
    }

    //4+2+1牌型，这里不要需要考虑对子A+Joker
    int twoOfAKindCardCount = 2;
    int twoOfAKindIndex = getSpecialCardCountIndex(tempStartIndex, stopIndex, twoOfAKindCardCount);
    if (twoOfAKindIndex > 0) {
        if (fourOfAKindIndex > twoOfAKindIndex) {
            resultCardArray[5] = resultCardArray[2];
            resultCardArray[6] = resultCardArray[3];
            resultCardArray[2] = resultCardArray[3] = 0;
            addSameCardsToResultCardArray(twoOfAKindIndex, twoOfAKindCardCount);
            addRestCardsToResultCardArray(1);
            setType(PHT_FOUR_OF_A_KIND, PHT_TWO_PAIR, PHT_ONE_PAIR);
        }
        else {
            addSameCardsToResultCardArray(twoOfAKindIndex, HIGH_HAND_CARD_COUNT);
            addRestCardsToResultCardArray(1);
            setType(PHT_FOUR_OF_A_KIND, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR);
        }

        return true;
    }

    //4+A+joker+1牌型
    if (!isNeedJoker && getCardCount(CARD_SPADE_ACE) == 1 && getCardCount(CARD_VALUE_JOKER) == 1) {
        addSpecialCardToResultCardArraySpecialIndex(stopIndex + 1, startIndex + 1, fourOfAKindCardCount);
        addRestCardsToResultCardArray(2);
        setType(PHT_FOUR_OF_A_KIND, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR);
        return true;
    }

    //使用两对规则决定是否拆分
    if (isSplitTwoPairs(fourOfAKindIndex, fourOfAKindIndex)) {
        resultCardArray[5] = resultCardArray[2];
        resultCardArray[6] = resultCardArray[3];
        resultCardArray[2] = resultCardArray[3] = 0;
        addRestCardsToResultCardArray(3);
        setType(PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR, PHT_ONE_PAIR);
    }
    else {
        addSpecialCardToResultCardArraySpecialIndex(stopIndex + 1, startIndex + 1, fourOfAKindCardCount);
        addRestCardsToResultCardArray(2);
        setType(PHT_FOUR_OF_A_KIND, PHT_FOUR_OF_A_KIND, PHT_HIGH_CARD);
    }

    return true;
}

bool CanterburyParkPokerHands::isSplitFullHouse() {
    return false;
}

bool CanterburyParkPokerHands::isSplitTwoPairs(int firstPairIndex, int secondPairIndex) {
    //[2,5]:low pair
    //[6,9]:medium pair
    //[10,K]:high pair
    int lowMaxIndex = getCardValueWithoutSuit(CARD_SPADE_5);
    int medMaxIndex = getCardValueWithoutSuit(CARD_SPADE_9);
    int higMinIndex = getCardValueWithoutSuit(CARD_SPADE_10);

    bool isSplit = true;
    bool hasAce = hasSingleAce();

    //因为对子是从大牌往小牌开始寻找的，所以默认有secondPairIndex < firstPairIndex
    if ((firstPairIndex <= lowMaxIndex) || (secondPairIndex <= lowMaxIndex && firstPairIndex <= medMaxIndex)) {
        //Low-Low or Low-Medium split. Exception: If an Ace or King can play in the Low Hand.
        isSplit = !(hasAce || getCardCount(CARD_SPADE_KING) == 1);
    }
    else if ((secondPairIndex <= medMaxIndex && firstPairIndex <= medMaxIndex) || (secondPairIndex <= lowMaxIndex && firstPairIndex >= higMinIndex)) {
        //Medium-Medium or Low-High split. Exception: If an Ace with a Face can play in the Low Hand.
        isSplit = !(hasAce && (getCardCount(CARD_SPADE_KING) == 1 || getCardCount(CARD_SPADE_QUEEN) == 1 || getCardCount(CARD_SPADE_JACK) == 1));
    }

    return isSplit;
}

PAI_GOW_NS_END