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
        return isEnglish ? "Spade" : "ºÚÌÒ";
    case CS_HEART:
        return isEnglish ? "Heart" : "ºìÐÄ";
    case CS_CLUB:
        return isEnglish ? "Club" : "Ã·»¨";
    case CS_DIAMOND:
        return isEnglish ? "Diamond" : "·½¿é";
    default:
        return "";
    }
}

PAI_GOW_NS_END