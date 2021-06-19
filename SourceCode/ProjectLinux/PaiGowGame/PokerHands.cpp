#include "PokerHands.h"

PAI_GOW_NS_BEGIN

PokerHands::PokerHands() {
    //（1）行坐标表示去除花色后的牌值，Ace下标是14，Joker下标是15。
    //     下标0存储对应花色的牌有几张，用CARD_BACKGROUND获取下标0。
    //     下标1存储的也是Ace，但不计入统计，只用于顺子。
    //     可以使用CARD_VALUE_COUNT获取下标的最大值，也可以直接使用数组长度。
    //（2）列坐标的前四列按照花色存储牌的值，最后一列表示这张牌值有几张。
    int row = getCardValueWithoutSuit(CARD_VALUE_COUNT);
    int column = CS_COUNT + 1;
    divideCardArray.resize(row, std::vector<int>(column, 0));

    resultCardArray.resize(DEALING_CARD_COUNT_EACH, 0);
    sevenCardsArray.resize(DEALING_CARD_COUNT_EACH, 0);
    twoCardsType = fiveCardsType = sevenCardsType = PHT_COUNT;
}

void PokerHands::clean() {
    cleanSevenCardArray();
    cleanResultCardArray();
    cleanDivideCardArray();
    twoCardsType = fiveCardsType = sevenCardsType = PHT_COUNT;
}

bool PokerHands::isEmpty() {
    return twoCardsType == PHT_COUNT && fiveCardsType == PHT_COUNT && resultCardArray[0] <= 0;
}

bool PokerHands::PokerHands::parseForTip(const std::vector<int> &cardsArray) {
    setType(PHT_COUNT, PHT_COUNT, PHT_COUNT);
    if (!checkCardsArray(cardsArray) || cardsArray.size() != DEALING_CARD_COUNT_EACH) {
        return false;
    }

    cleanSevenCardArray();
    cleanResultCardArray();
    cleanDivideCardArray();
    divideCardsArray(cardsArray);

    std::vector<int> tempSevenCardsArray;
    PokerHandsType tempSevenType = PHT_COUNT;
    if (hasSevenCardStraightFlush() || hasRoyalFlushWithSuitedRoyals() || hasSevenCardStraightFlushWithJoker()) {
        tempSevenCardsArray = resultCardArray;
        tempSevenType = sevenCardsType;
        cleanResultCardArray();
    }

    if (hasFiveAces() ||
        hasStraightFlush() ||
        hasFourOfAKind() ||
        hasFullHouse() ||
        hasFlush() ||
        hasStraight() ||
        hasThreeOfAKind() ||
        hasTwoPair() || hasOnePair()) {
        fillSevenCardArray();
    }
    else {
        processHighCard();
        sevenCardsArray = resultCardArray;
    }

    if (tempSevenType != PHT_COUNT) {
        sevenCardsArray = tempSevenCardsArray;
        sevenCardsType = tempSevenType;
    }

    return true;
}

PokerHandsType PokerHands::parseFiveCardsType(const std::vector<int> &cardsArray) {
    fiveCardsType = PHT_COUNT;
    if (!checkCardsArray(cardsArray) || cardsArray.size() != HIGH_HAND_CARD_COUNT) {
        return fiveCardsType;
    }

    std::vector<int> sevenCardsBackup;
    PokerHandsType sevenTypeBackup = PHT_COUNT;
    if (sevenCardsType != PHT_COUNT && sevenCardsArray[0] > 0) {
        sevenCardsBackup = sevenCardsArray;
        sevenTypeBackup = sevenCardsType;
    }

    cleanSevenCardArray();
    cleanResultCardArray();
    cleanDivideCardArray();
    divideCardsArray(cardsArray);

    if (!(hasFiveAces() ||
        hasStraightFlush() ||
        hasFourOfAKind() ||
        hasFullHouse() ||
        hasFlush() ||
        hasStraight() ||
        hasThreeOfAKind() ||
        hasTwoPair() || hasOnePair())) {
        processHighCard();
        sevenCardsArray = resultCardArray;
    }

    if (sevenTypeBackup != PHT_COUNT) {
        sevenCardsArray = sevenCardsBackup;
        sevenCardsType = sevenTypeBackup;
    }

    return fiveCardsType;
}

PokerHandsType PokerHands::parseTwoCardsType(const std::vector<int> &cardsArray) {
    twoCardsType = PHT_COUNT;
    if (!checkCardsArray(cardsArray) || cardsArray.size() != LOW_HAND_CARD_COUNT) {
        return twoCardsType;
    }

    twoCardsType = PHT_HIGH_CARD;
    CardValue firstCard = (CardValue)cardsArray[0];
    CardValue secondCard = (CardValue)cardsArray[1];
    int firstCardWithoutSuit = getCardValueWithoutSuit(firstCard);
    int secondCardWithoutSuit = getCardValueWithoutSuit(secondCard);

    if (firstCardWithoutSuit < secondCardWithoutSuit) {
        resultCardArray[5] = cardsArray[1];
        resultCardArray[6] = cardsArray[0];
        if (secondCard == CARD_VALUE_JOKER && firstCardWithoutSuit == getCardValueWithoutSuit(CARD_SPADE_ACE)) {
            resultCardArray[5] = cardsArray[0];
            resultCardArray[6] = cardsArray[1];
            twoCardsType = PHT_ONE_PAIR;
        }
    }
    else if (firstCardWithoutSuit > secondCardWithoutSuit) {
        resultCardArray[5] = cardsArray[0];
        resultCardArray[6] = cardsArray[1];
        if (firstCard == CARD_VALUE_JOKER && secondCardWithoutSuit == getCardValueWithoutSuit(CARD_SPADE_ACE)) {
            resultCardArray[5] = cardsArray[1];
            resultCardArray[6] = cardsArray[0];
            twoCardsType = PHT_ONE_PAIR;
        }
    }
    else {
        twoCardsType = PHT_ONE_PAIR;
        CardSuit firstSuit = getSuit(cardsArray[0]);
        CardSuit secondSuit = getSuit(cardsArray[1]);
        if (firstSuit < secondSuit) {
            resultCardArray[5] = cardsArray[0];
            resultCardArray[6] = cardsArray[1];
        }
        else {
            resultCardArray[5] = cardsArray[1];
            resultCardArray[6] = cardsArray[0];
        }
    }

    return twoCardsType;
}

CardSuit PokerHands::getFlushSuit(bool isSeven) {
    const std::vector<int> &cardsArray = isSeven ? sevenCardsArray : resultCardArray;
    int joker = getCardValueWithoutSuit(CARD_VALUE_JOKER);
    for (int cardIndex = 0; cardIndex < HIGH_HAND_CARD_COUNT; cardIndex++) {
        if (cardsArray[cardIndex] != joker) {
            return getSuit(cardsArray[cardIndex]);
        }
    }

    return CS_COUNT;
}

int PokerHands::getStraightHighCard(bool isSeven) {
    const std::vector<int> &cardsArray = isSeven ? sevenCardsArray : resultCardArray;
    int aceIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);

    int firstCard = cardsArray[0];
    if (getCardValueWithoutSuit(firstCard) > aceIndex) {
        firstCard = getBiggerCardValue(cardsArray[1]);
    }

    return firstCard;
}

int PokerHands::getStraightLowCard(bool isSeven) {
    const std::vector<int> &cardsArray = isSeven ? sevenCardsArray : resultCardArray;
    int aceIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int lastCard;

    if (isSeven) {
        if (isStraight(cardsArray, (int)cardsArray.size())) {
            lastCard = cardsArray[6];
            if (getCardValueWithoutSuit(lastCard) > aceIndex) {
                lastCard = getSmallerCardValue(cardsArray[5]);
            }

            return lastCard;
        }

        if (isStraight(cardsArray, (int)(cardsArray.size() - 1))) {
            lastCard = cardsArray[5];
            if (getCardValueWithoutSuit(lastCard) > aceIndex) {
                lastCard = getSmallerCardValue(cardsArray[4]);
            }

            return lastCard;
        }
    }

    lastCard = cardsArray[4];
    if (getCardValueWithoutSuit(lastCard) > aceIndex) {
        lastCard = getSmallerCardValue(cardsArray[3]);
    }

    return lastCard;
}

int PokerHands::getFiveHighCard() {
    if (fiveCardsType == PHT_ROYAL_FLUSH || fiveCardsType == PHT_STRAIGHT_FLUSH || fiveCardsType == PHT_STRAIGHT) {
        return getStraightHighCard(false);
    }

    return getCardValueWithoutJoker(resultCardArray[0]);
}

int PokerHands::getTwoHighCard() {
    return getCardValueWithoutJoker(resultCardArray[5]);
}

bool PokerHands::isFiveCardsTypeGreaterThanTwoCardsType() {
    if (fiveCardsType < twoCardsType) {
        return false;
    }

    if (fiveCardsType == twoCardsType) {
        int aceValue = getCardValueWithoutSuit(CARD_SPADE_ACE);

        int firstHighCard = getCardValueWithoutSuit(resultCardArray[0]);
        if (firstHighCard > aceValue) firstHighCard = aceValue;

        int secondHighCard = getCardValueWithoutSuit(resultCardArray[5]);
        if (secondHighCard > aceValue) secondHighCard = aceValue;

        return firstHighCard >= secondHighCard;
    }

    return true;
}

void PokerHands::cleanResultCardArray() {
    for (std::vector<int>::size_type index = 0; index < resultCardArray.size(); index++) {
        resultCardArray[index] = CARD_BACKGROUND;
    }
}

void PokerHands::cleanSevenCardArray() {
    for (std::vector<int>::size_type index = 0; index < sevenCardsArray.size(); index++) {
        sevenCardsArray[index] = CARD_BACKGROUND;
    }
}

void PokerHands::cleanDivideCardArray() {
    for (std::vector<int>::size_type cardIndex = 0; cardIndex < divideCardArray.size(); cardIndex++) {
        for (std::vector<int>::size_type suitIndex = 0; suitIndex < divideCardArray[cardIndex].size(); suitIndex++) {
            divideCardArray[cardIndex][suitIndex] = CARD_BACKGROUND;
        }
    }
}

void PokerHands::fillSevenCardArray() {
    for (int cardValue : resultCardArray) {
        for (std::vector<int>::size_type sevenIndex = 0; sevenIndex < sevenCardsArray.size(); sevenIndex++) {
            if (cardValue != sevenCardsArray[sevenIndex]) {
                if (sevenCardsArray[sevenIndex] == CARD_BACKGROUND) {
                    sevenCardsArray[sevenIndex] = cardValue;
                    break;
                }
            }
        }
    }
}

void PokerHands::divideCardsArray(const std::vector<int> &cardsArray) {
    for (int arrayCardValue : cardsArray) {
        CardValue cardValue = (CardValue)arrayCardValue;
        CardSuit cardSuit = getSuit(arrayCardValue);
        int cardIndex = getCardValueWithoutSuit(cardValue);

        //统计这张牌去除花色后的牌值出现的次数
        divideCardArray[cardIndex][CS_COUNT]++;

        if (cardSuit != CS_COUNT) {
            //将这张牌保存到对应花色的位置
            divideCardArray[cardIndex][cardSuit] = arrayCardValue;

            //这种花色的牌数加1
            divideCardArray[0][cardSuit]++;

        }
        else if (cardValue == CARD_VALUE_JOKER) {

            //鬼牌的每种花色位置都设置为鬼牌值，对应的花色张数也加1
            for (int suitIndex = 0; suitIndex < CS_COUNT; suitIndex++) {
                divideCardArray[cardIndex][suitIndex] = arrayCardValue;
                divideCardArray[0][suitIndex]++;
            }
        }
    }

    //把下标为14的Ace牌，拷贝一份到下标1，用于5/4/3/2/1顺子的判断，不能增加花色的统计数
    int aceIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    divideCardArray[1] = divideCardArray[aceIndex];

    //保存牌的张数
    divideCardArray[0][CS_COUNT] = static_cast<int>(cardsArray.size());
}

bool PokerHands::checkCardsArray(const std::vector<int> &cardsArray) {
    //判断牌值是否合法
    for (int arrayCardValue : cardsArray) {
        if (arrayCardValue <= CARD_BACKGROUND || arrayCardValue >= CARD_VALUE_COUNT) {
            return false;
        }
    }

    //判断是否有两张完全一样的牌
    for (std::vector<int>::size_type firstIndex = 0; firstIndex < cardsArray.size(); firstIndex++) {
        for (std::vector<int>::size_type secondIndex = 0; secondIndex < cardsArray.size(); secondIndex++) {
            if (firstIndex != secondIndex) {
                if (cardsArray[firstIndex] == cardsArray[secondIndex]) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool PokerHands::isCardInResultCardArray(int cardValue) {
    if (cardValue > 0) {
        for (int resultCardValue : resultCardArray) {
            if (resultCardValue == cardValue) {
                return true;
            }
        }
    }

    return false;
}

void PokerHands::setType(PokerHandsType sevenType, PokerHandsType fiveType, PokerHandsType twoType) {
    sevenCardsType = sevenType;
    fiveCardsType = fiveType;
    twoCardsType = twoType;
}

bool PokerHands::isPair(int firstCard, int secondCard) {
    if (firstCard <= 0 || secondCard <= 0) return false;

    int firstCardIndex = getCardValueWithoutJoker(firstCard);
    int secondCardIndex = getCardValueWithoutJoker(secondCard);
    return firstCardIndex == secondCardIndex;
}

bool PokerHands::isFlush(const std::vector<int> &flushArray, int stopIndex) {
    CardSuit firstSuit = CS_COUNT;
    for (int cardIndex = 0; cardIndex < stopIndex; cardIndex++) {
        if (flushArray[cardIndex] == CARD_VALUE_JOKER) {
            continue;
        }

        if (firstSuit == CS_COUNT) {
            firstSuit = getSuit(flushArray[cardIndex]);
            continue;
        }

        if (getSuit(flushArray[cardIndex]) != firstSuit) {
            return false;
        }
    }

    return true;
}

bool PokerHands::isStraight(const std::vector<int> &straightArray, int stopIndex) {
    //避免出现4-3-2-1-K的情况
    int realStopIndex = stopIndex - 1;
    if (getCardValueWithoutSuit(straightArray[0]) <= getCardValueWithoutJoker(straightArray[realStopIndex])) {
        return false;
    }

    //逐个判断相邻牌值
    int joker = CARD_VALUE_JOKER;
    for (int cardIndex = 0; cardIndex < realStopIndex; cardIndex++) {
        if (straightArray[cardIndex] == joker || straightArray[cardIndex + 1] == joker) {
            continue;
        }

        if (getSmallerCardValue(straightArray[cardIndex]) != getCardValueWithoutSuit(straightArray[cardIndex + 1])) {
            return false;
        }
    }

    return true;
}

bool PokerHands::isFiveCardsFlushOrStraightInResultCardArray() {
    bool isFlushArray = isFlush(resultCardArray, HIGH_HAND_CARD_COUNT);
    bool isStraightArray = isStraight(resultCardArray, HIGH_HAND_CARD_COUNT);
    if (isStraightArray && isFlushArray) {
        fiveCardsType = isRoyalFlushWhenResultCardArrayIsStraightFlush() ? PHT_ROYAL_FLUSH : PHT_STRAIGHT_FLUSH;
        return true;
    }
    else if (isFlushArray) {
        fiveCardsType = PHT_FLUSH;
        return true;
    }
    else if (isStraightArray) {
        fiveCardsType = PHT_STRAIGHT;
        return true;
    }
    else {
        return false;
    }
}

bool PokerHands::isRoyalFlushWhenResultCardArrayIsStraightFlush() {
    int firstIndex = getCardValueWithoutSuit(resultCardArray[0]);
    int secondIndex = getCardValueWithoutSuit(resultCardArray[1]);

    int aceIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int kingIndex = getCardValueWithoutSuit(CARD_SPADE_KING);

    return (firstIndex >= aceIndex && secondIndex >= kingIndex);
}

bool PokerHands::isUseOnPairInStraightOrFlush(std::vector<int> onePairRuleResultCardArray, std::vector<int> &resultCardsBackupArray) {
    //把对子放到右手
    cleanResultCardArray();
    resultCardArray[5] = onePairRuleResultCardArray[0];
    resultCardArray[6] = onePairRuleResultCardArray[1];

    //把其他牌按照原顺序移到左手
    int resultIndex = 0;
    int pairCard = getCardValueWithoutJoker(onePairRuleResultCardArray[0]);
    for (int card : resultCardsBackupArray) {
        if (getCardValueWithoutJoker(card) != pairCard) {
            resultCardArray[resultIndex++] = card;
        }
    }

    //判断前五张牌的牌型
    return isFiveCardsFlushOrStraightInResultCardArray();
}

bool PokerHands::hasSingleAce() {
    return getCardCount(CARD_SPADE_ACE) == 1 || getCardCount(CARD_VALUE_JOKER) == 1;
}

int PokerHands::getCardCount(CardValue cardValue) {
    int cardIndex = getCardValueWithoutSuit(cardValue);
    int arrayIndex = cardIndex < static_cast<int>(divideCardArray.size()) ? cardIndex : 0;
    return divideCardArray[arrayIndex][CS_COUNT];
}

int PokerHands::getFlushIndex(int flushCardCount) {
    for (int suitIndex = 0; suitIndex < CS_COUNT; suitIndex++) {
        if (divideCardArray[0][suitIndex] >= flushCardCount) {
            return suitIndex;
        }
    }

    return -1;
}

int PokerHands::getLongestStraight(int suitIndex) {
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_4);
    int jokerCount = getCardCount(CARD_VALUE_JOKER);

    int straightCardsMaxCount = (getCardCount(CARD_VALUE_COUNT) == DEALING_CARD_COUNT_EACH) ? DEALING_CARD_COUNT_EACH : HIGH_HAND_CARD_COUNT;
    while (straightCardsMaxCount >= HIGH_HAND_CARD_COUNT) {
        int straightIndex = getStraightIndex(startIndex, stopIndex, suitIndex, straightCardsMaxCount, jokerCount);
        if (straightIndex < 0) {
            --straightCardsMaxCount;
            continue;
        }

        addStraightCardsToResultCardArray(straightIndex, straightIndex - straightCardsMaxCount, suitIndex);
        if (jokerCount > 0) addJokerToResultCardArray(straightCardsMaxCount);
        return straightCardsMaxCount;
    }

    return -1;
}

int PokerHands::getStraightIndex(int startCardIndex, int stopCardIndex, int suit, int straightCardCount, int jokerCount) {
    for (int straightIndex = startCardIndex; straightIndex > stopCardIndex; straightIndex--) {
        //传入的牌间隔的张数，要满足顺子张数的要求
        int straightStopIndex = straightIndex - straightCardCount;
        if (straightStopIndex < 0) {
            return -1;
        }

        //统计顺子牌有几张
        int cardCount = 0;
        for (int cardIndex = straightIndex; cardIndex > straightStopIndex; cardIndex--) {
            cardCount += divideCardArray[cardIndex][suit] > 0 ? 1 : 0;
        }

        //判断是否满足顺子的张数
        if ((cardCount + jokerCount) >= straightCardCount) {
            return straightIndex;
        }
    }

    return -1;
}

int PokerHands::getSpecialCardCountIndex(int startCardIndex, int stopCardIndex, int specialCardCount) {
    int step = startCardIndex >= stopCardIndex ? -1 : 1;
    for (int cardIndex = startCardIndex; cardIndex != stopCardIndex; cardIndex += step) {
        if (divideCardArray[cardIndex][CS_COUNT] == specialCardCount) {
            return cardIndex;
        }
    }

    return -1;
}

void PokerHands::addJokerToResultCardArray(int cardStopIndex) {
    for (int cardIndex = 0; cardIndex < cardStopIndex; cardIndex++) {
        if (resultCardArray[cardIndex] == 0) {
            resultCardArray[cardIndex] = CARD_VALUE_JOKER;
        }
    }
}

void PokerHands::addRestCardsToResultCardArray(int restCardsCount) {
    int startIndex = static_cast<int>(divideCardArray.size() - 1);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_2) - 1;

    for (int cardIndex = startIndex; restCardsCount > 0 && cardIndex != stopIndex; cardIndex--) {
        if (divideCardArray[cardIndex][CS_COUNT] <= 0) {
            continue;
        }

        for (int suitIndex = 0; suitIndex < CS_COUNT && restCardsCount > 0; suitIndex++) {
            if (divideCardArray[cardIndex][suitIndex] <= 0) {
                continue;
            }

            if (isCardInResultCardArray(divideCardArray[cardIndex][suitIndex])) {
                continue;
            }

            for (std::vector<int>::size_type resultIndex = 0; resultIndex < resultCardArray.size(); resultIndex++) {
                if (resultCardArray[resultIndex] == 0) {
                    resultCardArray[resultIndex] = divideCardArray[cardIndex][suitIndex];
                    --restCardsCount;
                    break;
                }
            }
        }
    }
}

void PokerHands::addSameCardsToResultCardArray(int cardIndex, int resultStartIndex) {
    for (int suitIndex = 0; suitIndex < CS_COUNT; suitIndex++) {
        if (divideCardArray[cardIndex][suitIndex] > 0) {
            if (!isCardInResultCardArray(divideCardArray[cardIndex][suitIndex])) {
                resultCardArray[resultStartIndex++] = divideCardArray[cardIndex][suitIndex];
            }
        }
    }
}

void PokerHands::addStraightCardsToResultCardArray(int startCardIndex, int stopCardIndex, int suitIndex) {
    int resultStartIndex = 0;
    if (suitIndex == CS_COUNT) {
        for (int cardIndex = startCardIndex; cardIndex > stopCardIndex; cardIndex--) {
            for (int index = 0; index < CS_COUNT; index++) {
                if (divideCardArray[cardIndex][index] > 0) {
                    resultCardArray[resultStartIndex] = divideCardArray[cardIndex][index];
                    break;
                }
            }

            //即使四种花色都没有值，也要移动到下一个位置，当前位置留给鬼牌
            ++resultStartIndex;
        }
    }
    else {
        for (int cardIndex = startCardIndex; cardIndex > stopCardIndex; cardIndex--) {
            resultCardArray[resultStartIndex++] = divideCardArray[cardIndex][suitIndex];
        }
    }
}

int PokerHands::addDifferentCardsToResultCardArray(int startCardIndex, int stopCardIndex, int suitIndex) {
    int resultNextIndex = 0;
    for (int cardIndex = startCardIndex; cardIndex > stopCardIndex; cardIndex--) {
        if (divideCardArray[cardIndex][suitIndex] > 0) {
            resultCardArray[resultNextIndex++] = divideCardArray[cardIndex][suitIndex];
        }
    }

    return resultNextIndex;
}

void PokerHands::addSpecialCardToResultCardArraySpecialIndex(int startIndex, int stopIndex, int resultArrayIndex) {
    int step = startIndex <= stopIndex ? 1 : -1;
    for (int cardIndex = startIndex; cardIndex != stopIndex; cardIndex += step) {
        if (divideCardArray[cardIndex][CS_COUNT] > 0) {
            for (int suitIndex = 0; suitIndex < CS_COUNT; suitIndex++) {
                if (divideCardArray[cardIndex][suitIndex] > 0) {
                    if (!isCardInResultCardArray(divideCardArray[cardIndex][suitIndex])) {
                        resultCardArray[resultArrayIndex] = divideCardArray[cardIndex][suitIndex];
                        return;
                    }
                }
            }
        }
    }
}

void PokerHands::vectorCopy(std::vector<int>& src, int srcPos, std::vector<int>& dest, int destPos, int length)
{
    for (int index = 0; index < length; index++) {
        dest[destPos++] = src[srcPos++];
    }
}

bool PokerHands::hasSevenCardStraightFlush() {
    //不能包含Joker
    if (getCardCount(CARD_VALUE_JOKER) > 0) {
        return false;
    }

    //必须是七张牌
    int sevenCardCount = 7;

    //必须是相同颜色
    int flushIndex = getFlushIndex(sevenCardCount);
    if (flushIndex < 0) {
        return false;
    }

    //必须是同花色的顺子
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_4);
    int straightIndex = getStraightIndex(startIndex, stopIndex, flushIndex, sevenCardCount, 0);
    if (straightIndex < 0) {
        return false;
    }

    //组装结果牌
    addDifferentCardsToResultCardArray(straightIndex, straightIndex - sevenCardCount, flushIndex);
    sevenCardsType = PHT_SEVEN_CARD_STRAIGHT_FLUSH;
    return true;
}

bool PokerHands::hasRoyalFlushWithSuitedRoyals() {
    //必须满足同花色
    int royalFlushCardCount = 5;
    int flushIndex = getFlushIndex(royalFlushCardCount);
    if (flushIndex < 0) {
        return false;
    }

    //能否凑成顺子
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_9);
    int jokerCount = getCardCount(CARD_VALUE_JOKER);
    int straightIndex = getStraightIndex(startIndex, stopIndex, flushIndex, royalFlushCardCount, jokerCount);
    if (straightIndex < 0) {
        return false;
    }

    addStraightCardsToResultCardArray(straightIndex, straightIndex - royalFlushCardCount, flushIndex);
    if (jokerCount > 0) addJokerToResultCardArray(royalFlushCardCount);
    addRestCardsToResultCardArray(2);

    //右手牌第一张牌必须是K
    if (getCardValueWithoutSuit(resultCardArray[5]) != getCardValueWithoutSuit(CARD_SPADE_KING)) {
        cleanResultCardArray();
        return false;
    }

    //右手牌第二张牌必须是Q
    if (getCardValueWithoutSuit(resultCardArray[6]) != getCardValueWithoutSuit(CARD_SPADE_QUEEN)) {
        cleanResultCardArray();
        return false;
    }

    //剩余两张牌花色必须相同
    if (getSuit(resultCardArray[5]) != getSuit(resultCardArray[6])) {
        cleanResultCardArray();
        return false;
    }

    sevenCardsType = PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS;
    return true;
}

bool PokerHands::hasSevenCardStraightFlushWithJoker() {
    //必须包含Joker
    if (getCardCount(CARD_VALUE_JOKER) <= 0) {
        return false;
    }

    //必须是七张牌
    int sevenCardCount = 7;

    //必须是相同颜色
    int flushIndex = getFlushIndex(sevenCardCount);
    if (flushIndex < 0) {
        return false;
    }

    //必须是同花色的顺子
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_4);
    int straightIndex = getStraightIndex(startIndex, stopIndex, flushIndex, sevenCardCount, 1);
    if (straightIndex < 0) {
        return false;
    }

    //组装结果牌
    addStraightCardsToResultCardArray(straightIndex, straightIndex - sevenCardCount, flushIndex);
    addJokerToResultCardArray(sevenCardCount);
    sevenCardsType = PHT_SEVEN_CARD_STRAIGHT_FLUSH_WITH_JOKER;
    return true;
}

bool PokerHands::hasFiveAces() {
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

    //如果存在一对K，那么把K放到右手牌
    int kingIndex = getCardValueWithoutSuit(CARD_SPADE_KING);
    if (divideCardArray[kingIndex][CS_COUNT] == 2) {
        addSameCardsToResultCardArray(kingIndex, HIGH_HAND_CARD_COUNT);
        setType(PHT_FIVE_ACES, PHT_FIVE_ACES, PHT_ONE_PAIR);
        return true;
    }

    //如果不存在对K，则三条A放在左手牌，一对A放在右手牌
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

bool PokerHands::hasStraightFlush() {
    return false;
}

bool PokerHands::hasFourOfAKind() {
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
        if (isSplitFourOfAKind(fourOfAKindIndex, threeOfAKindIndex)) {
            cleanResultCardArray();
            addSameCardsToResultCardArray(threeOfAKindIndex, 0);
            addSameCardsToResultCardArray(fourOfAKindIndex, 3);
            if (isNeedJoker) addJokerToResultCardArray(DEALING_CARD_COUNT_EACH);
            setType(PHT_FOUR_OF_A_KIND, PHT_FULL_HOUSE, PHT_ONE_PAIR);
        }
        else {
            addRestCardsToResultCardArray(3);
            setType(PHT_FOUR_OF_A_KIND, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR);
        }

        return true;
    }

    //4+2+1牌型，这里不要需要考虑对子A+Joker
    int twoOfAKindCardCount = 2;
    int twoOfAKindIndex = getSpecialCardCountIndex(tempStartIndex, stopIndex, twoOfAKindCardCount);
    if (twoOfAKindIndex > 0) {
        addSameCardsToResultCardArray(twoOfAKindIndex, HIGH_HAND_CARD_COUNT);
        addRestCardsToResultCardArray(1);
        setType(PHT_FOUR_OF_A_KIND, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR);
        return true;
    }

    //4+A+joker+1牌型
    if (!isNeedJoker && getCardCount(CARD_SPADE_ACE) == 1 && getCardCount(CARD_VALUE_JOKER) == 1) {
        addSpecialCardToResultCardArraySpecialIndex(stopIndex + 1, startIndex + 1, fourOfAKindCardCount);
        addRestCardsToResultCardArray(2);
        setType(PHT_FOUR_OF_A_KIND, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR);
        return true;
    }

    //根据具体的拆分规则拆分四条
    if (isSplitFourOfAKind(fourOfAKindIndex, threeOfAKindIndex)) {
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

bool PokerHands::hasFullHouse() {
    //三带二
    int threeCardCount = 3;
    bool isNeedJoker = false;

    //寻找普通的相同的三张牌
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_2) - 1;
    int threeCardIndex = getSpecialCardCountIndex(startIndex, stopIndex, threeCardCount);
    if (threeCardIndex < 0) {
        if (getCardCount(CARD_VALUE_JOKER) <= 0) {
            return false;
        }

        //有Joker的情况下，两张A也满足条件
        isNeedJoker = true;
        threeCardIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
        if (divideCardArray[threeCardIndex][CS_COUNT] != 2) {
            return false;
        }
    }

    //如果只判断五张牌
    if (getCardCount(CARD_VALUE_COUNT) == HIGH_HAND_CARD_COUNT) {
        addSameCardsToResultCardArray(threeCardIndex, 0);
        if (isNeedJoker) addJokerToResultCardArray(threeCardCount);
        addRestCardsToResultCardArray(2);
        fiveCardsType = isPair(resultCardArray[3], resultCardArray[4]) ? PHT_FULL_HOUSE : PHT_THREE_OF_A_KIND;
        return true;
    }

    //3+3+1，始终把较大的三条抽出对子放到右手牌
    if (!isNeedJoker) {
        int secondThreeCardIndex = getSpecialCardCountIndex(threeCardIndex - 1, stopIndex, threeCardCount);
        if (secondThreeCardIndex > 0) {
            addSameCardsToResultCardArray(threeCardIndex, 0);
            sevenCardsArray = resultCardArray;
            resultCardArray[5] = resultCardArray[0];
            resultCardArray[6] = resultCardArray[1];
            addSameCardsToResultCardArray(secondThreeCardIndex, 0);
            addRestCardsToResultCardArray(2);
            setType(PHT_FULL_HOUSE, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR);
            sevenCardsArray[3] = resultCardArray[0];
            sevenCardsArray[4] = resultCardArray[1];
            sevenCardsArray[5] = resultCardArray[2];
            return true;
        }
    }

    //3+2+2或3+2+1+1
    int twoCardCount = 2;
    int tempStartIndex = isNeedJoker ? (startIndex - 1) : startIndex;
    int firstPairCardIndex = getSpecialCardCountIndex(tempStartIndex, stopIndex, twoCardCount);
    if (firstPairCardIndex > 0) {

        //先把三条保存好
        addSameCardsToResultCardArray(threeCardIndex, 0);
        if (isNeedJoker) addJokerToResultCardArray(threeCardCount);

        //3+2+2，把最大的对子放到右手牌
        int secondPairCardIndex = getSpecialCardCountIndex(firstPairCardIndex - 1, stopIndex, twoCardCount);
        if (secondPairCardIndex > 0) {
            sevenCardsArray = resultCardArray;
            addSameCardsToResultCardArray(secondPairCardIndex, threeCardCount);
            addSameCardsToResultCardArray(firstPairCardIndex, HIGH_HAND_CARD_COUNT);
            setType(PHT_FULL_HOUSE, PHT_FULL_HOUSE, PHT_ONE_PAIR);
            sevenCardsArray[3] = resultCardArray[5];
            sevenCardsArray[4] = resultCardArray[6];
            return true;
        }

        //3+2+A+Joker
        if (!isNeedJoker && getCardCount(CARD_SPADE_ACE) == 1 && getCardCount(CARD_VALUE_JOKER) == 1) {
            addSameCardsToResultCardArray(firstPairCardIndex, threeCardCount);
            addRestCardsToResultCardArray(2);
            setType(PHT_FULL_HOUSE, PHT_FULL_HOUSE, PHT_ONE_PAIR);
            sevenCardsArray = resultCardArray;
            return true;
        }

        //3+2+1+1，如果对子是2而且有一张(Ace或Joker)和一张King，则不需要把对子放在右手牌
        if (isSplitFullHouse()) {
            if (firstPairCardIndex == getCardValueWithoutSuit(CARD_SPADE_2) && hasSingleAce() && getCardCount(CARD_SPADE_KING) == 1) {
                addSameCardsToResultCardArray(firstPairCardIndex, threeCardCount);
                addRestCardsToResultCardArray(2);
                setType(PHT_FULL_HOUSE, PHT_FULL_HOUSE, PHT_HIGH_CARD);
                sevenCardsArray = resultCardArray;
                return true;
            }
        }

        //3+2+1+1，其它所有情况都需要把对子分到右手牌
        sevenCardsArray = resultCardArray;
        addSameCardsToResultCardArray(firstPairCardIndex, HIGH_HAND_CARD_COUNT);
        addRestCardsToResultCardArray(2);
        setType(PHT_FULL_HOUSE, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR);
        sevenCardsArray[3] = resultCardArray[5];
        sevenCardsArray[4] = resultCardArray[6];
        return true;
    }

    //3+A+Joker+1+1
    if (!isNeedJoker && getCardCount(CARD_VALUE_JOKER) == 1 && getCardCount(CARD_SPADE_ACE) == 1) {
        addSameCardsToResultCardArray(threeCardIndex, 0);
        sevenCardsArray = resultCardArray;
        resultCardArray[5] = CARD_VALUE_JOKER;
        addSameCardsToResultCardArray(startIndex, 6); //aceIndex == startIndex
        addRestCardsToResultCardArray(2);
        setType(PHT_FULL_HOUSE, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR);
        sevenCardsArray[3] = resultCardArray[6];
        sevenCardsArray[4] = resultCardArray[5];
        return true;
    }

    return false;
}

bool PokerHands::hasFlush() {
    return false;
}

bool PokerHands::hasStraight() {
    return false;
}

bool PokerHands::hasThreeOfAKind() {
    //三条有三张牌
    int threeOfAKindCardCount = 3;
    int aceIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);

    //寻找普通情况的三张牌
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_KING);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_2) - 1;
    int threeOfAKindIndex = getSpecialCardCountIndex(startIndex, stopIndex, threeOfAKindCardCount);
    if (threeOfAKindIndex > 0) {
        addSameCardsToResultCardArray(threeOfAKindIndex, 0);
    }
    else {
        //如果有三条A，把其中一条A放到右手牌，或者把鬼牌放到右手牌
        int aceCount = divideCardArray[aceIndex][CS_COUNT];
        if (aceCount == threeOfAKindCardCount) {
            addSameCardsToResultCardArray(aceIndex, 0);
            resultCardArray[5] = resultCardArray[2];
            resultCardArray[2] = 0;
        }
        else if (aceCount == (threeOfAKindCardCount - 1) && getCardCount(CARD_VALUE_JOKER) == 1) {
            addSameCardsToResultCardArray(aceIndex, 0);
            resultCardArray[5] = CARD_VALUE_JOKER;
        }
    }

    //无法凑成三条
    if (resultCardArray[0] <= 0) {
        return false;
    }

    //如果只判断五张牌
    if (getCardCount(CARD_VALUE_COUNT) == HIGH_HAND_CARD_COUNT) {
        //把右手牌的A或Joker放回左手牌
        if (aceIndex <= getCardValueWithoutSuit(resultCardArray[5])) {
            resultCardArray[2] = resultCardArray[5];
            resultCardArray[5] = 0;
        }

        addRestCardsToResultCardArray(2);
        fiveCardsType = PHT_THREE_OF_A_KIND;
        return true;
    }

    //保存最高牌型
    sevenCardsArray = resultCardArray;

    //把最大的和第二大的单牌放到右手牌
    startIndex = getCardValueWithoutSuit(CARD_VALUE_JOKER);
    if (threeOfAKindIndex < 0) {
        addSpecialCardToResultCardArraySpecialIndex(startIndex, stopIndex, 6);
        addRestCardsToResultCardArray(3);
        setType(PHT_THREE_OF_A_KIND, PHT_ONE_PAIR, PHT_HIGH_CARD);
    }
    else {
        addSpecialCardToResultCardArraySpecialIndex(startIndex, stopIndex, 5);
        startIndex = getCardValueWithoutSuit(resultCardArray[5]) - 1;
        addSpecialCardToResultCardArraySpecialIndex(startIndex, stopIndex, 6);
        addRestCardsToResultCardArray(2);
        setType(PHT_THREE_OF_A_KIND, PHT_THREE_OF_A_KIND, PHT_HIGH_CARD);
    }

    return true;
}

bool PokerHands::hasTwoPair() {
    //对子两张牌
    int onePairCardCount = 2;
    int cardTotalCount = getCardCount(CARD_VALUE_COUNT);

    //至少要有一个对子，第一个对子不能是A+Joker
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_2) - 1;
    int firstPairIndex = getSpecialCardCountIndex(startIndex, stopIndex, onePairCardCount);
    if (firstPairIndex < 0) {
        return false;
    }

    //2+A+Joker+1+1+1，左右两边各一个对子
    int secondPairIndex = getSpecialCardCountIndex(firstPairIndex - 1, stopIndex, onePairCardCount);
    if (secondPairIndex < 0) {
        if (getCardCount(CARD_VALUE_JOKER) == 1 && getCardCount(CARD_SPADE_ACE) == 1) {

            //把Ace+Joker放到左手牌
            addSameCardsToResultCardArray(startIndex, 0);
            resultCardArray[1] = CARD_VALUE_JOKER;

            if (cardTotalCount == HIGH_HAND_CARD_COUNT) {
                addSameCardsToResultCardArray(firstPairIndex, onePairCardCount);
                addRestCardsToResultCardArray(1);
                fiveCardsType = PHT_TWO_PAIR;
            }
            else {
                addSameCardsToResultCardArray(firstPairIndex, HIGH_HAND_CARD_COUNT);
                addRestCardsToResultCardArray(3);
                setType(PHT_COUNT, PHT_ONE_PAIR, PHT_ONE_PAIR);
            }
        }

        //有牌值就有对子，否则没有对子
        return resultCardArray[0] > 0;
    }

    //如果只是判断五张牌
    if (cardTotalCount == HIGH_HAND_CARD_COUNT) {
        addSameCardsToResultCardArray(firstPairIndex, 0);
        addSameCardsToResultCardArray(secondPairIndex, onePairCardCount);
        addRestCardsToResultCardArray(1);
        fiveCardsType = PHT_TWO_PAIR;
        return true;
    }

    //2+2+2+1：把最大的对子放到右手牌
    int thirdPairIndex = getSpecialCardCountIndex(secondPairIndex - 1, stopIndex, onePairCardCount);
    if (thirdPairIndex > 0) {
        addSameCardsToResultCardArray(secondPairIndex, 0);
        addSameCardsToResultCardArray(thirdPairIndex, onePairCardCount);
        addSameCardsToResultCardArray(firstPairIndex, HIGH_HAND_CARD_COUNT);
        addRestCardsToResultCardArray(1);
        setType(PHT_COUNT, PHT_TWO_PAIR, PHT_ONE_PAIR);
        return true;
    }

    //2+2+A+Joker+1，把A+Joker放到右手牌
    if (getCardCount(CARD_VALUE_JOKER) == 1 && getCardCount(CARD_SPADE_ACE) == 1) {
        addSameCardsToResultCardArray(firstPairIndex, 0);
        addSameCardsToResultCardArray(secondPairIndex, onePairCardCount);
        addSameCardsToResultCardArray(startIndex, HIGH_HAND_CARD_COUNT);
        resultCardArray[6] = CARD_VALUE_JOKER;
        addRestCardsToResultCardArray(1);
        setType(PHT_COUNT, PHT_TWO_PAIR, PHT_ONE_PAIR);
        return true;
    }

    //根据具体的拆分规则决定是否拆分对子
    if (isSplitTwoPairs(firstPairIndex, secondPairIndex)) {
        addSameCardsToResultCardArray(firstPairIndex, 0);
        addSameCardsToResultCardArray(secondPairIndex, HIGH_HAND_CARD_COUNT);
        addRestCardsToResultCardArray(3);
        setType(PHT_COUNT, PHT_ONE_PAIR, PHT_ONE_PAIR);
    }
    else {
        addSameCardsToResultCardArray(firstPairIndex, 0);
        addSameCardsToResultCardArray(secondPairIndex, onePairCardCount);
        addSpecialCardToResultCardArraySpecialIndex(stopIndex + 1, startIndex + 1, 4);
        addRestCardsToResultCardArray(2);
        setType(PHT_COUNT, PHT_TWO_PAIR, PHT_HIGH_CARD);
    }

    return true;
}

bool PokerHands::hasOnePair() {
    //对子两张牌
    int onePairCardCount = 2;

    //寻找普通情况的对子
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_2) - 1;
    int onePairIndex = getSpecialCardCountIndex(startIndex, stopIndex, onePairCardCount);
    if (onePairIndex > 0) {
        addSameCardsToResultCardArray(onePairIndex, 0);
    }
    else {
        if (getCardCount(CARD_VALUE_JOKER) == 1 && getCardCount(CARD_SPADE_ACE) == 1) {
            addSameCardsToResultCardArray(startIndex, 0);
            resultCardArray[1] = CARD_VALUE_JOKER;
        }
    }

    //找不到对子
    if (resultCardArray[0] <= 0) {
        return false;
    }

    //如果只判断五张牌
    if (getCardCount(CARD_VALUE_COUNT) == HIGH_HAND_CARD_COUNT) {
        addRestCardsToResultCardArray(3);
        fiveCardsType = PHT_ONE_PAIR;
        return true;
    }

    //把最大的和第二大的单牌放到右手牌
    startIndex = getCardValueWithoutSuit(CARD_VALUE_JOKER);
    addSpecialCardToResultCardArraySpecialIndex(startIndex, stopIndex, 5);

    startIndex = getCardValueWithoutSuit(resultCardArray[5]) - 1;
    addSpecialCardToResultCardArraySpecialIndex(startIndex, stopIndex, 6);

    addRestCardsToResultCardArray(3);
    setType(PHT_COUNT, PHT_ONE_PAIR, PHT_HIGH_CARD);
    return true;
}

void PokerHands::processHighCard() {
    int resultArrayIndex = 0;
    bool isFillTowCardsArray = getCardCount(CARD_VALUE_COUNT) == HIGH_HAND_CARD_COUNT;

    int startIndex = getCardValueWithoutSuit(CARD_VALUE_JOKER);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_2) - 1;
    int arraySize = static_cast<int>(resultCardArray.size());
    for (int cardIndex = startIndex; cardIndex > stopIndex && resultArrayIndex < arraySize; cardIndex--) {
        if (divideCardArray[cardIndex][CS_COUNT] <= 0) {
            continue;
        }

        for (int suitIndex = 0; suitIndex < CS_COUNT; suitIndex++) {
            if (divideCardArray[cardIndex][suitIndex] > 0) {
                resultCardArray[resultArrayIndex] = divideCardArray[cardIndex][suitIndex];
                break;
            }
        }

        if (isFillTowCardsArray) {
            ++resultArrayIndex;
            continue;
        }

        if (resultArrayIndex == 0) {
            resultArrayIndex = 5;
        }
        else if (resultArrayIndex == 5) {
            resultArrayIndex = 6;
        }
        else {
            resultArrayIndex = 1;
            isFillTowCardsArray = true;
        }
    }

    setType(PHT_COUNT, PHT_HIGH_CARD, getCardCount(CARD_VALUE_COUNT) == DEALING_CARD_COUNT_EACH ? PHT_HIGH_CARD : PHT_COUNT);
}

PAI_GOW_NS_END