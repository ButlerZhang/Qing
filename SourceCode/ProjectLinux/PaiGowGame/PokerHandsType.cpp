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
        return "����";
    case PHT_ONE_PAIR:
        return "һ��";
    case PHT_TWO_PAIR:
        return "����";
    case PHT_THREE_OF_A_KIND:
        return "����";
    case PHT_STRAIGHT:
        return "˳��";
    case PHT_FLUSH:
        return "ͬ��";
    case PHT_FULL_HOUSE:
        return "���ú�";
    case PHT_FOUR_OF_A_KIND:
        return "����";
    case PHT_STRAIGHT_FLUSH:
        return "ͬ��˳";
    case PHT_ROYAL_FLUSH:
        return "�ʼ�ͬ��˳";
    case PHT_FIVE_ACES:
        return "����A";
    case PHT_SEVEN_CARD_STRAIGHT_FLUSH_WITH_JOKER:
        return "������ͬ��˳��������";
    case PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS:
        return "�ʼ�ͬ��˳�����ʼ�ͬ��";
    case PHT_SEVEN_CARD_STRAIGHT_FLUSH:
        return "������ͬ��˳��������";
    default:
        return "";
    }
}

std::string getPokerTypeName(PokerHandsType type, bool isEnglish) {
    return isEnglish ? getEnglishName(type) : getChineseName(type);
}

PAI_GOW_NS_END