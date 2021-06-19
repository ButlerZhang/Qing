#pragma once
#include "CardValue.h"

PAI_GOW_NS_BEGIN

enum CardSuit {

    CS_SPADE,                      //����
    CS_HEART,                      //����
    CS_CLUB,                       //÷��
    CS_DIAMOND,                    //����
    CS_COUNT,                      //���ֻ�ɫ
};

CardSuit getSuit(int cardValue);
std::string getSuitName(CardSuit suit, bool isEnglish);

PAI_GOW_NS_END