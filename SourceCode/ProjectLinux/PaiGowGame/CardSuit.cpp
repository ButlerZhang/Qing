#include "CardSuit.h"

PAI_GOW_NS_BEGIN

CardSuit getSuit(int cardValue) {
    if (cardValue <= CARD_BACKGROUND) {
        return CS_COUNT;
    }
    else if (cardValue <= CARD_SPADE_KING) {
        return CS_SPADE;
    }
    else if (cardValue <= CARD_HEART_KING) {
        return CS_HEART;
    }
    else if (cardValue <= CARD_CLUB_KING) {
        return CS_CLUB;
    }
    else if (cardValue <= CARD_DIAMOND_KING) {
        return CS_DIAMOND;
    }
    else {
        return CS_COUNT;
    }
}

std::string getSuitName(CardSuit suit, bool isEnglish) {
    switch (suit) {
    case CS_SPADE:
        return isEnglish ? "Spade" : "黑桃";
    case CS_HEART:
        return isEnglish ? "Heart" : "红心";
    case CS_CLUB:
        return isEnglish ? "Club" : "梅花";
    case CS_DIAMOND:
        return isEnglish ? "Diamond" : "方块";
    default:
        return "";
    }
}

PAI_GOW_NS_END