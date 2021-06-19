#include "PokerHands.h"

PAI_GOW_NS_BEGIN

PokerHands::PokerHands() {
    //��1���������ʾȥ����ɫ�����ֵ��Ace�±���14��Joker�±���15��
    //     �±�0�洢��Ӧ��ɫ�����м��ţ���CARD_BACKGROUND��ȡ�±�0��
    //     �±�1�洢��Ҳ��Ace����������ͳ�ƣ�ֻ����˳�ӡ�
    //     ����ʹ��CARD_VALUE_COUNT��ȡ�±�����ֵ��Ҳ����ֱ��ʹ�����鳤�ȡ�
    //��2���������ǰ���а��ջ�ɫ�洢�Ƶ�ֵ�����һ�б�ʾ������ֵ�м��š�
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

        //ͳ��������ȥ����ɫ�����ֵ���ֵĴ���
        divideCardArray[cardIndex][CS_COUNT]++;

        if (cardSuit != CS_COUNT) {
            //�������Ʊ��浽��Ӧ��ɫ��λ��
            divideCardArray[cardIndex][cardSuit] = arrayCardValue;

            //���ֻ�ɫ��������1
            divideCardArray[0][cardSuit]++;

        }
        else if (cardValue == CARD_VALUE_JOKER) {

            //���Ƶ�ÿ�ֻ�ɫλ�ö�����Ϊ����ֵ����Ӧ�Ļ�ɫ����Ҳ��1
            for (int suitIndex = 0; suitIndex < CS_COUNT; suitIndex++) {
                divideCardArray[cardIndex][suitIndex] = arrayCardValue;
                divideCardArray[0][suitIndex]++;
            }
        }
    }

    //���±�Ϊ14��Ace�ƣ�����һ�ݵ��±�1������5/4/3/2/1˳�ӵ��жϣ��������ӻ�ɫ��ͳ����
    int aceIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    divideCardArray[1] = divideCardArray[aceIndex];

    //�����Ƶ�����
    divideCardArray[0][CS_COUNT] = static_cast<int>(cardsArray.size());
}

bool PokerHands::checkCardsArray(const std::vector<int> &cardsArray) {
    //�ж���ֵ�Ƿ�Ϸ�
    for (int arrayCardValue : cardsArray) {
        if (arrayCardValue <= CARD_BACKGROUND || arrayCardValue >= CARD_VALUE_COUNT) {
            return false;
        }
    }

    //�ж��Ƿ���������ȫһ������
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
    //�������4-3-2-1-K�����
    int realStopIndex = stopIndex - 1;
    if (getCardValueWithoutSuit(straightArray[0]) <= getCardValueWithoutJoker(straightArray[realStopIndex])) {
        return false;
    }

    //����ж�������ֵ
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
    //�Ѷ��ӷŵ�����
    cleanResultCardArray();
    resultCardArray[5] = onePairRuleResultCardArray[0];
    resultCardArray[6] = onePairRuleResultCardArray[1];

    //�������ư���ԭ˳���Ƶ�����
    int resultIndex = 0;
    int pairCard = getCardValueWithoutJoker(onePairRuleResultCardArray[0]);
    for (int card : resultCardsBackupArray) {
        if (getCardValueWithoutJoker(card) != pairCard) {
            resultCardArray[resultIndex++] = card;
        }
    }

    //�ж�ǰ�����Ƶ�����
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
        //������Ƽ����������Ҫ����˳��������Ҫ��
        int straightStopIndex = straightIndex - straightCardCount;
        if (straightStopIndex < 0) {
            return -1;
        }

        //ͳ��˳�����м���
        int cardCount = 0;
        for (int cardIndex = straightIndex; cardIndex > straightStopIndex; cardIndex--) {
            cardCount += divideCardArray[cardIndex][suit] > 0 ? 1 : 0;
        }

        //�ж��Ƿ�����˳�ӵ�����
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

            //��ʹ���ֻ�ɫ��û��ֵ��ҲҪ�ƶ�����һ��λ�ã���ǰλ����������
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
    //���ܰ���Joker
    if (getCardCount(CARD_VALUE_JOKER) > 0) {
        return false;
    }

    //������������
    int sevenCardCount = 7;

    //��������ͬ��ɫ
    int flushIndex = getFlushIndex(sevenCardCount);
    if (flushIndex < 0) {
        return false;
    }

    //������ͬ��ɫ��˳��
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_4);
    int straightIndex = getStraightIndex(startIndex, stopIndex, flushIndex, sevenCardCount, 0);
    if (straightIndex < 0) {
        return false;
    }

    //��װ�����
    addDifferentCardsToResultCardArray(straightIndex, straightIndex - sevenCardCount, flushIndex);
    sevenCardsType = PHT_SEVEN_CARD_STRAIGHT_FLUSH;
    return true;
}

bool PokerHands::hasRoyalFlushWithSuitedRoyals() {
    //��������ͬ��ɫ
    int royalFlushCardCount = 5;
    int flushIndex = getFlushIndex(royalFlushCardCount);
    if (flushIndex < 0) {
        return false;
    }

    //�ܷ�ճ�˳��
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

    //�����Ƶ�һ���Ʊ�����K
    if (getCardValueWithoutSuit(resultCardArray[5]) != getCardValueWithoutSuit(CARD_SPADE_KING)) {
        cleanResultCardArray();
        return false;
    }

    //�����Ƶڶ����Ʊ�����Q
    if (getCardValueWithoutSuit(resultCardArray[6]) != getCardValueWithoutSuit(CARD_SPADE_QUEEN)) {
        cleanResultCardArray();
        return false;
    }

    //ʣ�������ƻ�ɫ������ͬ
    if (getSuit(resultCardArray[5]) != getSuit(resultCardArray[6])) {
        cleanResultCardArray();
        return false;
    }

    sevenCardsType = PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS;
    return true;
}

bool PokerHands::hasSevenCardStraightFlushWithJoker() {
    //�������Joker
    if (getCardCount(CARD_VALUE_JOKER) <= 0) {
        return false;
    }

    //������������
    int sevenCardCount = 7;

    //��������ͬ��ɫ
    int flushIndex = getFlushIndex(sevenCardCount);
    if (flushIndex < 0) {
        return false;
    }

    //������ͬ��ɫ��˳��
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_4);
    int straightIndex = getStraightIndex(startIndex, stopIndex, flushIndex, sevenCardCount, 1);
    if (straightIndex < 0) {
        return false;
    }

    //��װ�����
    addStraightCardsToResultCardArray(straightIndex, straightIndex - sevenCardCount, flushIndex);
    addJokerToResultCardArray(sevenCardCount);
    sevenCardsType = PHT_SEVEN_CARD_STRAIGHT_FLUSH_WITH_JOKER;
    return true;
}

bool PokerHands::hasFiveAces() {
    //����Ҫ�й��ƣ���Ace����������
    if (getCardCount(CARD_VALUE_JOKER) != 1 || getCardCount(CARD_SPADE_ACE) != 4) {
        return false;
    }

    //����װ���
    resultCardArray[0] = CARD_SPADE_ACE;
    resultCardArray[1] = CARD_HEART_ACE;
    resultCardArray[2] = CARD_CLUB_ACE;
    resultCardArray[3] = CARD_DIAMOND_ACE;
    resultCardArray[4] = CARD_VALUE_JOKER;

    //���ֻ�ж������ƣ���ôֱ�ӷ��ؽ��
    if (getCardCount(CARD_VALUE_COUNT) == HIGH_HAND_CARD_COUNT) {
        fiveCardsType = PHT_FIVE_ACES;
        return true;
    }

    //�����������
    sevenCardsArray = resultCardArray;

    //�������һ��K����ô��K�ŵ�������
    int kingIndex = getCardValueWithoutSuit(CARD_SPADE_KING);
    if (divideCardArray[kingIndex][CS_COUNT] == 2) {
        addSameCardsToResultCardArray(kingIndex, HIGH_HAND_CARD_COUNT);
        setType(PHT_FIVE_ACES, PHT_FIVE_ACES, PHT_ONE_PAIR);
        return true;
    }

    //��������ڶ�K��������A���������ƣ�һ��A����������
    resultCardArray[2] = CARD_VALUE_JOKER;
    resultCardArray[5] = CARD_CLUB_ACE;
    resultCardArray[6] = CARD_DIAMOND_ACE;
    resultCardArray[3] = resultCardArray[4] = 0;
    addRestCardsToResultCardArray(2);

    //��Ҫ�ж��������Ƿ���ڶ���
    PokerHandsType fiveType = isPair(resultCardArray[3], resultCardArray[4]) ? PHT_FULL_HOUSE : PHT_THREE_OF_A_KIND;
    setType(PHT_FIVE_ACES, fiveType, PHT_ONE_PAIR);
    return true;
}

bool PokerHands::hasStraightFlush() {
    return false;
}

bool PokerHands::hasFourOfAKind() {
    //������������
    int fourOfAKindCardCount = 4;
    bool isNeedJoker = false;

    //Ѱ����ͨ����������ƣ��Ӵ�С����Ѱ��
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_2) - 1;
    int fourOfAKindIndex = getSpecialCardCountIndex(startIndex, stopIndex, fourOfAKindCardCount);
    if (fourOfAKindIndex < 0) {
        if (getCardCount(CARD_VALUE_JOKER) <= 0) {
            return false;
        }

        //��Joker������£�����AҲ��������
        isNeedJoker = true;
        fourOfAKindIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
        if (divideCardArray[fourOfAKindIndex][CS_COUNT] != 3) {
            return false;
        }
    }

    //��������
    addSameCardsToResultCardArray(fourOfAKindIndex, 0);
    if (isNeedJoker) addJokerToResultCardArray(fourOfAKindCardCount);

    //�����������
    sevenCardsArray = resultCardArray;

    //���ֻ�ж������ƣ�����ֱ�ӷ��ؽ����
    if (getCardCount(CARD_VALUE_COUNT) == HIGH_HAND_CARD_COUNT) {
        addRestCardsToResultCardArray(1);
        fiveCardsType = PHT_FOUR_OF_A_KIND;
        return true;
    }

    //4+3���ͣ����ﲻ��Ҫ����AA+Joker����Ϊ�����Ե���4+2+1����
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

    //4+2+1���ͣ����ﲻҪ��Ҫ���Ƕ���A+Joker
    int twoOfAKindCardCount = 2;
    int twoOfAKindIndex = getSpecialCardCountIndex(tempStartIndex, stopIndex, twoOfAKindCardCount);
    if (twoOfAKindIndex > 0) {
        addSameCardsToResultCardArray(twoOfAKindIndex, HIGH_HAND_CARD_COUNT);
        addRestCardsToResultCardArray(1);
        setType(PHT_FOUR_OF_A_KIND, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR);
        return true;
    }

    //4+A+joker+1����
    if (!isNeedJoker && getCardCount(CARD_SPADE_ACE) == 1 && getCardCount(CARD_VALUE_JOKER) == 1) {
        addSpecialCardToResultCardArraySpecialIndex(stopIndex + 1, startIndex + 1, fourOfAKindCardCount);
        addRestCardsToResultCardArray(2);
        setType(PHT_FOUR_OF_A_KIND, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR);
        return true;
    }

    //���ݾ���Ĳ�ֹ���������
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
    //������
    int threeCardCount = 3;
    bool isNeedJoker = false;

    //Ѱ����ͨ����ͬ��������
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_2) - 1;
    int threeCardIndex = getSpecialCardCountIndex(startIndex, stopIndex, threeCardCount);
    if (threeCardIndex < 0) {
        if (getCardCount(CARD_VALUE_JOKER) <= 0) {
            return false;
        }

        //��Joker������£�����AҲ��������
        isNeedJoker = true;
        threeCardIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
        if (divideCardArray[threeCardIndex][CS_COUNT] != 2) {
            return false;
        }
    }

    //���ֻ�ж�������
    if (getCardCount(CARD_VALUE_COUNT) == HIGH_HAND_CARD_COUNT) {
        addSameCardsToResultCardArray(threeCardIndex, 0);
        if (isNeedJoker) addJokerToResultCardArray(threeCardCount);
        addRestCardsToResultCardArray(2);
        fiveCardsType = isPair(resultCardArray[3], resultCardArray[4]) ? PHT_FULL_HOUSE : PHT_THREE_OF_A_KIND;
        return true;
    }

    //3+3+1��ʼ�հѽϴ������������ӷŵ�������
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

    //3+2+2��3+2+1+1
    int twoCardCount = 2;
    int tempStartIndex = isNeedJoker ? (startIndex - 1) : startIndex;
    int firstPairCardIndex = getSpecialCardCountIndex(tempStartIndex, stopIndex, twoCardCount);
    if (firstPairCardIndex > 0) {

        //�Ȱ����������
        addSameCardsToResultCardArray(threeCardIndex, 0);
        if (isNeedJoker) addJokerToResultCardArray(threeCardCount);

        //3+2+2�������Ķ��ӷŵ�������
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

        //3+2+1+1�����������2������һ��(Ace��Joker)��һ��King������Ҫ�Ѷ��ӷ���������
        if (isSplitFullHouse()) {
            if (firstPairCardIndex == getCardValueWithoutSuit(CARD_SPADE_2) && hasSingleAce() && getCardCount(CARD_SPADE_KING) == 1) {
                addSameCardsToResultCardArray(firstPairCardIndex, threeCardCount);
                addRestCardsToResultCardArray(2);
                setType(PHT_FULL_HOUSE, PHT_FULL_HOUSE, PHT_HIGH_CARD);
                sevenCardsArray = resultCardArray;
                return true;
            }
        }

        //3+2+1+1�����������������Ҫ�Ѷ��ӷֵ�������
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
    //������������
    int threeOfAKindCardCount = 3;
    int aceIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);

    //Ѱ����ͨ�����������
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_KING);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_2) - 1;
    int threeOfAKindIndex = getSpecialCardCountIndex(startIndex, stopIndex, threeOfAKindCardCount);
    if (threeOfAKindIndex > 0) {
        addSameCardsToResultCardArray(threeOfAKindIndex, 0);
    }
    else {
        //���������A��������һ��A�ŵ������ƣ����߰ѹ��Ʒŵ�������
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

    //�޷��ճ�����
    if (resultCardArray[0] <= 0) {
        return false;
    }

    //���ֻ�ж�������
    if (getCardCount(CARD_VALUE_COUNT) == HIGH_HAND_CARD_COUNT) {
        //�������Ƶ�A��Joker�Ż�������
        if (aceIndex <= getCardValueWithoutSuit(resultCardArray[5])) {
            resultCardArray[2] = resultCardArray[5];
            resultCardArray[5] = 0;
        }

        addRestCardsToResultCardArray(2);
        fiveCardsType = PHT_THREE_OF_A_KIND;
        return true;
    }

    //�����������
    sevenCardsArray = resultCardArray;

    //�����ĺ͵ڶ���ĵ��Ʒŵ�������
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
    //����������
    int onePairCardCount = 2;
    int cardTotalCount = getCardCount(CARD_VALUE_COUNT);

    //����Ҫ��һ�����ӣ���һ�����Ӳ�����A+Joker
    int startIndex = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int stopIndex = getCardValueWithoutSuit(CARD_SPADE_2) - 1;
    int firstPairIndex = getSpecialCardCountIndex(startIndex, stopIndex, onePairCardCount);
    if (firstPairIndex < 0) {
        return false;
    }

    //2+A+Joker+1+1+1���������߸�һ������
    int secondPairIndex = getSpecialCardCountIndex(firstPairIndex - 1, stopIndex, onePairCardCount);
    if (secondPairIndex < 0) {
        if (getCardCount(CARD_VALUE_JOKER) == 1 && getCardCount(CARD_SPADE_ACE) == 1) {

            //��Ace+Joker�ŵ�������
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

        //����ֵ���ж��ӣ�����û�ж���
        return resultCardArray[0] > 0;
    }

    //���ֻ���ж�������
    if (cardTotalCount == HIGH_HAND_CARD_COUNT) {
        addSameCardsToResultCardArray(firstPairIndex, 0);
        addSameCardsToResultCardArray(secondPairIndex, onePairCardCount);
        addRestCardsToResultCardArray(1);
        fiveCardsType = PHT_TWO_PAIR;
        return true;
    }

    //2+2+2+1�������Ķ��ӷŵ�������
    int thirdPairIndex = getSpecialCardCountIndex(secondPairIndex - 1, stopIndex, onePairCardCount);
    if (thirdPairIndex > 0) {
        addSameCardsToResultCardArray(secondPairIndex, 0);
        addSameCardsToResultCardArray(thirdPairIndex, onePairCardCount);
        addSameCardsToResultCardArray(firstPairIndex, HIGH_HAND_CARD_COUNT);
        addRestCardsToResultCardArray(1);
        setType(PHT_COUNT, PHT_TWO_PAIR, PHT_ONE_PAIR);
        return true;
    }

    //2+2+A+Joker+1����A+Joker�ŵ�������
    if (getCardCount(CARD_VALUE_JOKER) == 1 && getCardCount(CARD_SPADE_ACE) == 1) {
        addSameCardsToResultCardArray(firstPairIndex, 0);
        addSameCardsToResultCardArray(secondPairIndex, onePairCardCount);
        addSameCardsToResultCardArray(startIndex, HIGH_HAND_CARD_COUNT);
        resultCardArray[6] = CARD_VALUE_JOKER;
        addRestCardsToResultCardArray(1);
        setType(PHT_COUNT, PHT_TWO_PAIR, PHT_ONE_PAIR);
        return true;
    }

    //���ݾ���Ĳ�ֹ�������Ƿ��ֶ���
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
    //����������
    int onePairCardCount = 2;

    //Ѱ����ͨ����Ķ���
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

    //�Ҳ�������
    if (resultCardArray[0] <= 0) {
        return false;
    }

    //���ֻ�ж�������
    if (getCardCount(CARD_VALUE_COUNT) == HIGH_HAND_CARD_COUNT) {
        addRestCardsToResultCardArray(3);
        fiveCardsType = PHT_ONE_PAIR;
        return true;
    }

    //�����ĺ͵ڶ���ĵ��Ʒŵ�������
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