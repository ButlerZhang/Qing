#pragma once
#include <string>
#include "PaiGowCommon.h"

PAI_GOW_NS_BEGIN

enum CardValue {

    CARD_BACKGROUND,

    CARD_SPADE_ACE,
    CARD_SPADE_2,
    CARD_SPADE_3,
    CARD_SPADE_4,
    CARD_SPADE_5,
    CARD_SPADE_6,
    CARD_SPADE_7,
    CARD_SPADE_8,
    CARD_SPADE_9,
    CARD_SPADE_10,
    CARD_SPADE_JACK,
    CARD_SPADE_QUEEN,
    CARD_SPADE_KING,

    CARD_HEART_ACE,
    CARD_HEART_2,
    CARD_HEART_3,
    CARD_HEART_4,
    CARD_HEART_5,
    CARD_HEART_6,
    CARD_HEART_7,
    CARD_HEART_8,
    CARD_HEART_9,
    CARD_HEART_10,
    CARD_HEART_JACK,
    CARD_HEART_QUEEN,
    CARD_HEART_KING,

    CARD_CLUB_ACE,
    CARD_CLUB_2,
    CARD_CLUB_3,
    CARD_CLUB_4,
    CARD_CLUB_5,
    CARD_CLUB_6,
    CARD_CLUB_7,
    CARD_CLUB_8,
    CARD_CLUB_9,
    CARD_CLUB_10,
    CARD_CLUB_JACK,
    CARD_CLUB_QUEEN,
    CARD_CLUB_KING,

    CARD_DIAMOND_ACE,
    CARD_DIAMOND_2,
    CARD_DIAMOND_3,
    CARD_DIAMOND_4,
    CARD_DIAMOND_5,
    CARD_DIAMOND_6,
    CARD_DIAMOND_7,
    CARD_DIAMOND_8,
    CARD_DIAMOND_9,
    CARD_DIAMOND_10,
    CARD_DIAMOND_JACK,
    CARD_DIAMOND_QUEEN,
    CARD_DIAMOND_KING,

    CARD_VALUE_JOKER,
    CARD_VALUE_COUNT,
};

int getCardValueWithoutSuit(CardValue cardValue);
int getCardValueWithoutSuit(int cardValue);
int getCardValueWithoutJoker(int cardValue);

int getBiggerCardValue(int cardValue);
int getSmallerCardValue(int cardValue);

std::string getCardName(CardValue cardValue);

PAI_GOW_NS_END