#pragma once
#include <vector>
#include "CardSuit.h"
#include "CardValue.h"
#include "PokerHandsType.h"

PAI_GOW_NS_BEGIN

class PokerHands {
public:

    PokerHands();

    void clean();
    bool isEmpty();

    bool parseForTip(const std::vector<int> &cardsArray);
    PokerHandsType parseFiveCardsType(const std::vector<int> &cardsArray);
    PokerHandsType parseTwoCardsType(const std::vector<int> &cardsArray);

    PokerHandsType getTwoCardsType() { return twoCardsType; }
    PokerHandsType getFiveCardsType() { return fiveCardsType; }
    PokerHandsType getSevenCardsType() { return sevenCardsType; }

    const std::vector<int>& getResultCardArray() { return resultCardArray; }
    const std::vector<int>& getSevenCardsArray() { return sevenCardsArray; }

    CardSuit getFlushSuit(bool isSeven);
    int getStraightHighCard(bool isSeven);
    int getStraightLowCard(bool isSeven);
    int getFiveHighCard();
    int getTwoHighCard();
    bool isFiveCardsTypeGreaterThanTwoCardsType();

protected:

    void cleanResultCardArray();
    void cleanSevenCardArray();
    void cleanDivideCardArray();
    void fillSevenCardArray();
    void divideCardsArray(const std::vector<int> &cardsArray);
    bool checkCardsArray(const std::vector<int> &cardsArray);
    bool isCardInResultCardArray(int cardValue);
    void setType(PokerHandsType sevenType, PokerHandsType fiveType, PokerHandsType twoType);

    bool isPair(int firstCard, int secondCard);
    bool isFlush(const std::vector<int> &flushArray, int stopIndex);
    bool isStraight(const std::vector<int> &straightArray, int stopIndex);
    bool isFiveCardsFlushOrStraightInResultCardArray();
    bool isRoyalFlushWhenResultCardArrayIsStraightFlush();
    bool isUseOnPairInStraightOrFlush(std::vector<int> onePairRuleResultCardArray, std::vector<int> &resultCardsBackupArray);

    virtual bool isSplitFullHouse() { return true; }
    virtual bool isSplitTwoPairs(int firstPairIndex, int secondPairIndex) { return false; }
    virtual bool isSplitFourOfAKind(int fourOfAKindIndex, int threeOfAKindIndex) { return false; }

    bool hasSingleAce();
    int getCardCount(CardValue cardValue);
    int getFlushIndex(int flushCardCount);
    int getLongestStraight(int suitIndex);
    int getStraightIndex(int startCardIndex, int stopCardIndex, int suit, int straightCardCount, int jokerCount);
    int getSpecialCardCountIndex(int startCardIndex, int stopCardIndex, int specialCardCount);

    void addJokerToResultCardArray(int cardStopIndex);
    void addRestCardsToResultCardArray(int restCardsCount);
    void addSameCardsToResultCardArray(int cardIndex, int resultStartIndex);
    void addStraightCardsToResultCardArray(int startCardIndex, int stopCardIndex, int suitIndex);
    int  addDifferentCardsToResultCardArray(int startCardIndex, int stopCardIndex, int suitIndex);
    void addSpecialCardToResultCardArraySpecialIndex(int startIndex, int stopIndex, int resultArrayIndex);
    void vectorCopy(std::vector<int> &src, int srcPos, std::vector<int> &dest, int destPos, int length);

    virtual bool hasSevenCardStraightFlush();
    virtual bool hasRoyalFlushWithSuitedRoyals();
    virtual bool hasSevenCardStraightFlushWithJoker();
    virtual bool hasFiveAces();
    virtual bool hasStraightFlush();
    virtual bool hasFourOfAKind();
    virtual bool hasFullHouse();
    virtual bool hasFlush();
    virtual bool hasStraight();
    virtual bool hasThreeOfAKind();
    virtual bool hasTwoPair();
    virtual bool hasOnePair();
    void processHighCard();

protected:

    PokerHandsType twoCardsType;                                           //两张牌牌型
    PokerHandsType fiveCardsType;                                          //五张牌牌型
    PokerHandsType sevenCardsType;                                         //七张牌牌型
    std::vector<int> resultCardArray;                                      //保存结果的牌数组
    std::vector<int> sevenCardsArray;                                      //保存七张牌最高牌型结果
    std::vector<std::vector<int>> divideCardArray;                         //将原始的牌划分成用于分析的二维数组
};

PAI_GOW_NS_END