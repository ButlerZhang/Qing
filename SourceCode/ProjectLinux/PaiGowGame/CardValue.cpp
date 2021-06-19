#include "CardValue.h"

PAI_GOW_NS_BEGIN

int getCardValueWithoutSuit(CardValue cardValue) {
    switch (cardValue) {
    case CARD_SPADE_2:
    case CARD_HEART_2:
    case CARD_CLUB_2:
    case CARD_DIAMOND_2:
        return 2;

    case CARD_SPADE_3:
    case CARD_HEART_3:
    case CARD_CLUB_3:
    case CARD_DIAMOND_3:
        return 3;

    case CARD_SPADE_4:
    case CARD_HEART_4:
    case CARD_CLUB_4:
    case CARD_DIAMOND_4:
        return 4;

    case CARD_SPADE_5:
    case CARD_HEART_5:
    case CARD_CLUB_5:
    case CARD_DIAMOND_5:
        return 5;

    case CARD_SPADE_6:
    case CARD_HEART_6:
    case CARD_CLUB_6:
    case CARD_DIAMOND_6:
        return 6;

    case CARD_SPADE_7:
    case CARD_HEART_7:
    case CARD_CLUB_7:
    case CARD_DIAMOND_7:
        return 7;

    case CARD_SPADE_8:
    case CARD_HEART_8:
    case CARD_CLUB_8:
    case CARD_DIAMOND_8:
        return 8;

    case CARD_SPADE_9:
    case CARD_HEART_9:
    case CARD_CLUB_9:
    case CARD_DIAMOND_9:
        return 9;

    case CARD_SPADE_10:
    case CARD_HEART_10:
    case CARD_CLUB_10:
    case CARD_DIAMOND_10:
        return 10;

    case CARD_SPADE_JACK:
    case CARD_HEART_JACK:
    case CARD_CLUB_JACK:
    case CARD_DIAMOND_JACK:
        return 11;

    case CARD_SPADE_QUEEN:
    case CARD_HEART_QUEEN:
    case CARD_CLUB_QUEEN:
    case CARD_DIAMOND_QUEEN:
        return 12;

    case CARD_SPADE_KING:
    case CARD_HEART_KING:
    case CARD_CLUB_KING:
    case CARD_DIAMOND_KING:
        return 13;

    case CARD_SPADE_ACE:
    case CARD_HEART_ACE:
    case CARD_CLUB_ACE:
    case CARD_DIAMOND_ACE:
        return 14;

    case CARD_VALUE_JOKER:
        return 15;

    case CARD_VALUE_COUNT:
        return 16;

    default:
        return CARD_BACKGROUND;
    }
}

int getCardValueWithoutSuit(int cardValue) {
    if (cardValue >= CARD_BACKGROUND && cardValue <= CARD_VALUE_COUNT) {
        CardValue tempCardValue = (CardValue)cardValue;
        return getCardValueWithoutSuit(tempCardValue);
    }

    return CARD_BACKGROUND;
}

int getCardValueWithoutJoker(int cardValue) {
    int aceCard = getCardValueWithoutSuit(CARD_SPADE_ACE);
    int currentCard = getCardValueWithoutSuit(cardValue);
    return currentCard <= aceCard ? currentCard : aceCard;
}

int getBiggerCardValue(int cardValue) {
    if (cardValue >= CARD_BACKGROUND && cardValue < CARD_VALUE_COUNT) {
        CardValue currentCard = (CardValue)cardValue;
        switch (currentCard) {
        case CARD_BACKGROUND:
            return getCardValueWithoutSuit(CARD_SPADE_2);

        case CARD_SPADE_2:
        case CARD_HEART_2:
        case CARD_CLUB_2:
        case CARD_DIAMOND_2:
            return getCardValueWithoutSuit(CARD_SPADE_3);

        case CARD_SPADE_3:
        case CARD_HEART_3:
        case CARD_CLUB_3:
        case CARD_DIAMOND_3:
            return getCardValueWithoutSuit(CARD_SPADE_4);

        case CARD_SPADE_4:
        case CARD_HEART_4:
        case CARD_CLUB_4:
        case CARD_DIAMOND_4:
            return getCardValueWithoutSuit(CARD_SPADE_5);

        case CARD_SPADE_5:
        case CARD_HEART_5:
        case CARD_CLUB_5:
        case CARD_DIAMOND_5:
            return getCardValueWithoutSuit(CARD_SPADE_6);

        case CARD_SPADE_6:
        case CARD_HEART_6:
        case CARD_CLUB_6:
        case CARD_DIAMOND_6:
            return getCardValueWithoutSuit(CARD_SPADE_7);

        case CARD_SPADE_7:
        case CARD_HEART_7:
        case CARD_CLUB_7:
        case CARD_DIAMOND_7:
            return getCardValueWithoutSuit(CARD_SPADE_8);

        case CARD_SPADE_8:
        case CARD_HEART_8:
        case CARD_CLUB_8:
        case CARD_DIAMOND_8:
            return getCardValueWithoutSuit(CARD_SPADE_9);

        case CARD_SPADE_9:
        case CARD_HEART_9:
        case CARD_CLUB_9:
        case CARD_DIAMOND_9:
            return getCardValueWithoutSuit(CARD_SPADE_10);

        case CARD_SPADE_10:
        case CARD_HEART_10:
        case CARD_CLUB_10:
        case CARD_DIAMOND_10:
            return getCardValueWithoutSuit(CARD_SPADE_JACK);

        case CARD_SPADE_JACK:
        case CARD_HEART_JACK:
        case CARD_CLUB_JACK:
        case CARD_DIAMOND_JACK:
            return getCardValueWithoutSuit(CARD_SPADE_QUEEN);

        case CARD_SPADE_QUEEN:
        case CARD_HEART_QUEEN:
        case CARD_CLUB_QUEEN:
        case CARD_DIAMOND_QUEEN:
            return getCardValueWithoutSuit(CARD_SPADE_KING);

        case CARD_SPADE_KING:
        case CARD_HEART_KING:
        case CARD_CLUB_KING:
        case CARD_DIAMOND_KING:
            return getCardValueWithoutSuit(CARD_SPADE_ACE);

        case CARD_SPADE_ACE:
        case CARD_HEART_ACE:
        case CARD_CLUB_ACE:
        case CARD_DIAMOND_ACE:
        case CARD_VALUE_JOKER:
        default:
            return CARD_BACKGROUND;
        }
    }

    return CARD_BACKGROUND;
}

int getSmallerCardValue(int cardValue) {
    if (cardValue >= CARD_BACKGROUND && cardValue < CARD_VALUE_COUNT) {
        CardValue currentCard = (CardValue)cardValue;
        switch (currentCard) {
        case CARD_SPADE_2:
        case CARD_HEART_2:
        case CARD_CLUB_2:
        case CARD_DIAMOND_2:
            return getCardValueWithoutSuit(CARD_SPADE_ACE);

        case CARD_SPADE_3:
        case CARD_HEART_3:
        case CARD_CLUB_3:
        case CARD_DIAMOND_3:
            return getCardValueWithoutSuit(CARD_SPADE_2);

        case CARD_SPADE_4:
        case CARD_HEART_4:
        case CARD_CLUB_4:
        case CARD_DIAMOND_4:
            return getCardValueWithoutSuit(CARD_SPADE_3);

        case CARD_SPADE_5:
        case CARD_HEART_5:
        case CARD_CLUB_5:
        case CARD_DIAMOND_5:
            return getCardValueWithoutSuit(CARD_SPADE_4);

        case CARD_SPADE_6:
        case CARD_HEART_6:
        case CARD_CLUB_6:
        case CARD_DIAMOND_6:
            return getCardValueWithoutSuit(CARD_SPADE_5);

        case CARD_SPADE_7:
        case CARD_HEART_7:
        case CARD_CLUB_7:
        case CARD_DIAMOND_7:
            return getCardValueWithoutSuit(CARD_SPADE_6);

        case CARD_SPADE_8:
        case CARD_HEART_8:
        case CARD_CLUB_8:
        case CARD_DIAMOND_8:
            return getCardValueWithoutSuit(CARD_SPADE_7);

        case CARD_SPADE_9:
        case CARD_HEART_9:
        case CARD_CLUB_9:
        case CARD_DIAMOND_9:
            return getCardValueWithoutSuit(CARD_SPADE_8);

        case CARD_SPADE_10:
        case CARD_HEART_10:
        case CARD_CLUB_10:
        case CARD_DIAMOND_10:
            return getCardValueWithoutSuit(CARD_SPADE_9);

        case CARD_SPADE_JACK:
        case CARD_HEART_JACK:
        case CARD_CLUB_JACK:
        case CARD_DIAMOND_JACK:
            return getCardValueWithoutSuit(CARD_SPADE_10);

        case CARD_SPADE_QUEEN:
        case CARD_HEART_QUEEN:
        case CARD_CLUB_QUEEN:
        case CARD_DIAMOND_QUEEN:
            return getCardValueWithoutSuit(CARD_SPADE_JACK);

        case CARD_SPADE_KING:
        case CARD_HEART_KING:
        case CARD_CLUB_KING:
        case CARD_DIAMOND_KING:
            return getCardValueWithoutSuit(CARD_SPADE_QUEEN);

        case CARD_SPADE_ACE:
        case CARD_HEART_ACE:
        case CARD_CLUB_ACE:
        case CARD_DIAMOND_ACE:
        case CARD_VALUE_JOKER:
            return getCardValueWithoutSuit(CARD_SPADE_KING);

        default:
            return CARD_BACKGROUND;
        }
    }

    return CARD_BACKGROUND;
}

std::string getCardName(CardValue cardValue) {
    switch (cardValue) {
    case CARD_SPADE_2:
    case CARD_HEART_2:
    case CARD_CLUB_2:
    case CARD_DIAMOND_2:
        return "2";

    case CARD_SPADE_3:
    case CARD_HEART_3:
    case CARD_CLUB_3:
    case CARD_DIAMOND_3:
        return "3";

    case CARD_SPADE_4:
    case CARD_HEART_4:
    case CARD_CLUB_4:
    case CARD_DIAMOND_4:
        return "4";

    case CARD_SPADE_5:
    case CARD_HEART_5:
    case CARD_CLUB_5:
    case CARD_DIAMOND_5:
        return "5";

    case CARD_SPADE_6:
    case CARD_HEART_6:
    case CARD_CLUB_6:
    case CARD_DIAMOND_6:
        return "6";

    case CARD_SPADE_7:
    case CARD_HEART_7:
    case CARD_CLUB_7:
    case CARD_DIAMOND_7:
        return "7";

    case CARD_SPADE_8:
    case CARD_HEART_8:
    case CARD_CLUB_8:
    case CARD_DIAMOND_8:
        return "8";

    case CARD_SPADE_9:
    case CARD_HEART_9:
    case CARD_CLUB_9:
    case CARD_DIAMOND_9:
        return "9";

    case CARD_SPADE_10:
    case CARD_HEART_10:
    case CARD_CLUB_10:
    case CARD_DIAMOND_10:
        return "10";

    case CARD_SPADE_JACK:
    case CARD_HEART_JACK:
    case CARD_CLUB_JACK:
    case CARD_DIAMOND_JACK:
        return "J";

    case CARD_SPADE_QUEEN:
    case CARD_HEART_QUEEN:
    case CARD_CLUB_QUEEN:
    case CARD_DIAMOND_QUEEN:
        return "Q";

    case CARD_SPADE_KING:
    case CARD_HEART_KING:
    case CARD_CLUB_KING:
    case CARD_DIAMOND_KING:
        return "K";

    case CARD_SPADE_ACE:
    case CARD_HEART_ACE:
    case CARD_CLUB_ACE:
    case CARD_DIAMOND_ACE:
        return "A";

    case CARD_VALUE_JOKER:
        return "Joker";

    default:
        return "";
    }
}

PAI_GOW_NS_END