#pragma once
#include "CardValue.h"

PAI_GOW_NS_BEGIN

enum CardSuit {

    CS_SPADE,                      //黑桃
    CS_HEART,                      //红桃
    CS_CLUB,                       //梅花
    CS_DIAMOND,                    //方块
    CS_COUNT,                      //四种花色
};

CardSuit getSuit(int cardValue);
std::string getSuitName(CardSuit suit, bool isEnglish);

PAI_GOW_NS_END