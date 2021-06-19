#include "PokerHandsType.h"

PAI_GOW_NS_BEGIN

std::string getEnglishName(PokerHandsType type) {
    switch (type) {
    case PHT_HIGH_CARD:
        return "High Card";
    case PHT_ONE_PAIR:
        return "One Pair";
    case PHT_TWO_PAIR:
        return "Two Pairs";
    case PHT_THREE_OF_A_KIND:
        return "Three Of A Kind";
    case PHT_STRAIGHT:
        return "Straight";
    case PHT_FLUSH:
        return "Flush";
    case PHT_FULL_HOUSE:
        return "Full House";
    case PHT_FOUR_OF_A_KIND:
        return "Four Of A Kind";
    case PHT_STRAIGHT_FLUSH:
        return "Straight Flush";
    case PHT_ROYAL_FLUSH:
        return "Royal Flush";
    case PHT_FIVE_ACES:
        return "Five Aces";
    case PHT_SEVEN_CARD_STRAIGHT_FLUSH_WITH_JOKER:
        return "Seven Cards Straight Flush With Joker";
    case PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS:
        return "Royal Flush With Suited Royals";
    case PHT_SEVEN_CARD_STRAIGHT_FLUSH:
        return "Seven Cards Straight Flush";
    default:
        return "";
    }
}

std::string getChineseName(PokerHandsType type) {
    switch (type) {
    case PHT_HIGH_CARD:
        return "高牌";
    case PHT_ONE_PAIR:
        return "一对";
    case PHT_TWO_PAIR:
        return "二对";
    case PHT_THREE_OF_A_KIND:
        return "三条";
    case PHT_STRAIGHT:
        return "顺子";
    case PHT_FLUSH:
        return "同花";
    case PHT_FULL_HOUSE:
        return "满堂红";
    case PHT_FOUR_OF_A_KIND:
        return "四条";
    case PHT_STRAIGHT_FLUSH:
        return "同花顺";
    case PHT_ROYAL_FLUSH:
        return "皇家同花顺";
    case PHT_FIVE_ACES:
        return "五条A";
    case PHT_SEVEN_CARD_STRAIGHT_FLUSH_WITH_JOKER:
        return "七张牌同花顺包含王牌";
    case PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS:
        return "皇家同花顺包含皇家同花";
    case PHT_SEVEN_CARD_STRAIGHT_FLUSH:
        return "七张牌同花顺不含王牌";
    default:
        return "";
    }
}

std::string getPokerTypeName(PokerHandsType type, bool isEnglish) {
    return isEnglish ? getEnglishName(type) : getChineseName(type);
}

PAI_GOW_NS_END