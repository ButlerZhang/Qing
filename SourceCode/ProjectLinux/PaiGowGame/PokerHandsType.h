#pragma once
#include <string>
#include "PaiGowCommon.h"

PAI_GOW_NS_BEGIN

enum PokerHandsType {
    PHT_HIGH_CARD,                                  //single highest card in hand: A>K>Q>J>10>9>8>7>6>5>4>3>2
    PHT_ONE_PAIR,                                   //contains two cards of the same value
    PHT_TWO_PAIR,                                   //contains two cards of the same value, plus two more of another matching value
    PHT_THREE_OF_A_KIND,                            //contains three cards of the same value
    PHT_STRAIGHT,                                   //contains five cards of sequential value in at least two different suits
    PHT_FLUSH,                                      //contains five cards of the same suit
    PHT_FULL_HOUSE,                                 //contains three matching cards of one value and two matching cards of another value
    PHT_FOUR_OF_A_KIND,                             //contains four cards of the same value
    PHT_STRAIGHT_FLUSH,                             //contains five cards in sequential value of the same suit
    PHT_ROYAL_FLUSH,                                //contains the following cards of same suit: A,K,Q,J,10
    PHT_FIVE_ACES,                                  //contains the following cards: A,A,A,A,with Joker
    PHT_SEVEN_CARD_STRAIGHT_FLUSH_WITH_JOKER,       //all cards are in sequential value and of the same suit
    PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS,             //contains A-K-Q-J-10 of the same suit in addition to a  K-Q of the same suit, with or without joker
    PHT_SEVEN_CARD_STRAIGHT_FLUSH,                  //all cards are in sequential value and of the same suit - without joker
    PHT_COUNT,
};

std::string getEnglishName(PokerHandsType type);
std::string getChineseName(PokerHandsType type);
std::string getPokerTypeName(PokerHandsType type, bool isEnglish);

PAI_GOW_NS_END