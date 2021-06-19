#pragma once
#include "../../PokerHandsUnitTest.h"
#include "TrumpPlazaPokerHands.h"

PAI_GOW_NS_BEGIN

class TrumpPlazaUnitTest : public PokerHandsUnitTest {
public:

    TrumpPlazaUnitTest() {
        m_test = std::make_shared<TrumpPlazaPokerHands>();
    }

private:

    void TestRoyalFlush7() {
        //SPADE的A+K+Q+J+10, A+A，三条A的情况
        std::vector<int> case1 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_ACE, CARD_CLUB_ACE};
        std::vector<int> result1 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_ACE, CARD_CLUB_ACE};
        assert (isCorrect(case1, result1, PHT_ROYAL_FLUSH, PHT_ONE_PAIR));

        //SPADE的A+K+Q+J+10, A+Joker，三条A的情况
        std::vector<int> case2 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_ACE, CARD_VALUE_JOKER};
        std::vector<int> result2 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_VALUE_JOKER, CARD_HEART_ACE};
        assert (isCorrect(case2, result2, PHT_ROYAL_FLUSH, PHT_ONE_PAIR));

        //SPADE的A+K+Q+J+10, Q+Q，三条Q的情况
        std::vector<int> case3 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_QUEEN, CARD_CLUB_QUEEN};
        std::vector<int> result3 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_QUEEN, CARD_CLUB_QUEEN};
        assert (isCorrect(case3, result3, PHT_ROYAL_FLUSH, PHT_ONE_PAIR));

        //SPADE的A+K+Q+J+10, 4+4，普通对子
        std::vector<int> case4 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_4, CARD_CLUB_4};
        std::vector<int> result4 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_4, CARD_CLUB_4};
        assert (isCorrect(case4, result4, PHT_ROYAL_FLUSH, PHT_ONE_PAIR));

        //SPADE的A+K+Q+J+10, K+10，使用两对规则
        std::vector<int> case5 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_KING, CARD_CLUB_10};
        std::vector<int> result5 = {CARD_SPADE_KING, CARD_HEART_KING, CARD_SPADE_ACE, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_CLUB_10};
        assert (isCorrect(case5, result5, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //SPADE的A+K+Q+J+10, K+4，判断能否使用单对规则
        std::vector<int> case6 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_KING, CARD_CLUB_4};
        std::vector<int> result6 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_KING, CARD_CLUB_4};
        assert (isCorrect(case6, result6, PHT_ROYAL_FLUSH, PHT_HIGH_CARD));

        //SPADE的A+K+Q+J+10, Joker+9，可以改为顺子+对子
        std::vector<int> case7 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_VALUE_JOKER, CARD_CLUB_9};
        std::vector<int> result7 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_CLUB_9, CARD_SPADE_ACE, CARD_VALUE_JOKER};
        assert (isCorrect(case7, result7, PHT_STRAIGHT, PHT_ONE_PAIR));

        //SPADE的A+K+Q+J+10, A+9，可以改为同花顺+对子
        std::vector<int> case8 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_CLUB_ACE, CARD_SPADE_9};
        std::vector<int> result8 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_ACE, CARD_CLUB_ACE};
        assert (isCorrect(case8, result8, PHT_STRAIGHT_FLUSH, PHT_ONE_PAIR));

        //SPADE的A+K+Q+J+10, K+4，可以改为同花色+对子
        std::vector<int> case9 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_KING, CARD_SPADE_4};
        std::vector<int> result9 = {CARD_SPADE_ACE, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_4, CARD_SPADE_KING, CARD_HEART_KING};
        assert (isCorrect(case9, result9, PHT_FLUSH, PHT_ONE_PAIR));

        //SPADE的A+K+Q+J+10+9+8，可以改为同花顺+高牌，七张牌顺子
        std::vector<int> case10 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_8};
        std::vector<int> result10 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_ACE, CARD_SPADE_8};
        assert (isCorrect(case10, result10, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //SPADE的A+K+Q+J+10+9+8，可以改为同花顺+高牌，六张牌顺子
        std::vector<int> case11 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_CLUB_3};
        std::vector<int> result11 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_ACE, CARD_CLUB_3};
        assert (isCorrect(case11, result11, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //SPADE的A+K+Q+J+10+9+8，可以改为顺子+高牌，七张牌顺子
        std::vector<int> case12 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_CLUB_9, CARD_CLUB_8};
        std::vector<int> result12 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_CLUB_9, CARD_SPADE_ACE, CARD_CLUB_8};
        assert (isCorrect(case12, result12, PHT_STRAIGHT, PHT_HIGH_CARD));

        //SPADE的A+K+Q+J+10+9+3
        std::vector<int> case13 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_CLUB_9, CARD_CLUB_3};
        std::vector<int> result13 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_CLUB_9, CARD_SPADE_ACE, CARD_CLUB_3};
        assert (isCorrect(case13, result13, PHT_STRAIGHT, PHT_HIGH_CARD));

        //SPADE的A+K+Q+J+10+3+2，可以改为同花+高牌
        std::vector<int> case14 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_3, CARD_SPADE_2};
        std::vector<int> result14 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_2, CARD_SPADE_ACE, CARD_SPADE_3};
        assert (isCorrect(case14, result14, PHT_FLUSH, PHT_HIGH_CARD));

        //SPADE的A+K+Q+J+10
        std::vector<int> case15 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_CLUB_2, CARD_CLUB_3};
        std::vector<int> result15 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_CLUB_3, CARD_CLUB_2};
        assert (isCorrect(case15, result15, PHT_ROYAL_FLUSH, PHT_HIGH_CARD));

        //HEART的A+K+Q+J+10
        std::vector<int> case16 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_SPADE_2, CARD_SPADE_3};
        std::vector<int> result16 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_SPADE_3, CARD_SPADE_2};
        assert (isCorrect(case16, result16, PHT_ROYAL_FLUSH, PHT_HIGH_CARD));

        //CLUB的A+K+Q+J+10
        std::vector<int> case17 = {CARD_CLUB_ACE, CARD_CLUB_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_CLUB_10, CARD_CLUB_2, CARD_DIAMOND_3};
        std::vector<int> result17 = {CARD_CLUB_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_CLUB_10, CARD_CLUB_2, CARD_CLUB_ACE, CARD_DIAMOND_3};
        assert (isCorrect(case17, result17, PHT_FLUSH, PHT_HIGH_CARD));

        //DIAMOND的A+K+Q+J+10
        std::vector<int> case18 = {CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_10, CARD_DIAMOND_2, CARD_HEART_3};
        std::vector<int> result18 = {CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_10, CARD_DIAMOND_2, CARD_DIAMOND_ACE, CARD_HEART_3};
        assert (isCorrect(case18, result18, PHT_FLUSH, PHT_HIGH_CARD));

        //缺10
        std::vector<int> case19 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_VALUE_JOKER, CARD_HEART_2, CARD_DIAMOND_3};
        std::vector<int> result19 = {CARD_VALUE_JOKER, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_2, CARD_HEART_ACE, CARD_DIAMOND_3};
        assert (isCorrect(case19, result19, PHT_FLUSH, PHT_HIGH_CARD));

        //缺J
        std::vector<int> case20 = {CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_VALUE_JOKER, CARD_DIAMOND_10, CARD_HEART_2, CARD_CLUB_9};
        std::vector<int> result20 = {CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_VALUE_JOKER, CARD_DIAMOND_10, CARD_CLUB_9, CARD_DIAMOND_ACE, CARD_HEART_2};
        assert (isCorrect(case20, result20, PHT_STRAIGHT, PHT_HIGH_CARD));

        //缺Q
        std::vector<int> case21 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_JACK, CARD_VALUE_JOKER, CARD_HEART_10, CARD_HEART_2, CARD_DIAMOND_4};
        std::vector<int> result21 = {CARD_VALUE_JOKER, CARD_HEART_KING, CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_2, CARD_HEART_ACE, CARD_DIAMOND_4};
        assert (isCorrect(case21, result21, PHT_FLUSH, PHT_HIGH_CARD));

        //缺K
        std::vector<int> case22 = {CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_CLUB_9, CARD_DIAMOND_4};
        std::vector<int> result22 = {CARD_VALUE_JOKER, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_CLUB_9, CARD_HEART_ACE, CARD_DIAMOND_4};
        assert (isCorrect(case22, result22, PHT_STRAIGHT, PHT_HIGH_CARD));

        //缺A
        std::vector<int> case23 = {CARD_VALUE_JOKER, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_10, CARD_CLUB_9, CARD_HEART_2};
        std::vector<int> result23 = {CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_10, CARD_CLUB_9, CARD_VALUE_JOKER, CARD_HEART_2};
        assert (isCorrect(case23, result23, PHT_STRAIGHT, PHT_HIGH_CARD));

        //缺10，但是有另一种花色的10
        std::vector<int> case24 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_VALUE_JOKER, CARD_HEART_2, CARD_DIAMOND_10};
        std::vector<int> result24 = {CARD_VALUE_JOKER, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_2, CARD_HEART_ACE, CARD_DIAMOND_10};
        assert (isCorrect(case24, result24, PHT_FLUSH, PHT_HIGH_CARD));

        //CLUB的A+K+Q+J+10+9+8
        std::vector<int> case25 = {CARD_CLUB_ACE, CARD_CLUB_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8};
        std::vector<int> result25 = {CARD_CLUB_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_ACE, CARD_CLUB_8};
        assert (isCorrect(case25, result25, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //HEART的A+K+Q+J+10+Joker
        std::vector<int> case26 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_VALUE_JOKER, CARD_SPADE_3};
        std::vector<int> result26 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_VALUE_JOKER, CARD_SPADE_3};
        assert (isCorrect(case26, result26, PHT_ROYAL_FLUSH, PHT_HIGH_CARD));

        //同花顺里包含A+Joker
        std::vector<int> case27 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_VALUE_JOKER, CARD_HEART_JACK, CARD_HEART_10, CARD_SPADE_3, CARD_SPADE_2};
        std::vector<int> result27 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_VALUE_JOKER, CARD_HEART_JACK, CARD_HEART_10, CARD_SPADE_3, CARD_SPADE_2};
        assert (isCorrect(case27, result27, PHT_ROYAL_FLUSH, PHT_HIGH_CARD));

        //同花顺里包含A+Joker时，要把它们打在左手牌，同时把接下来最大的两张牌放到右手牌，除非是5-4-3-2-1顺子
        std::vector<int> case28 = {CARD_HEART_5, CARD_HEART_4, CARD_VALUE_JOKER, CARD_HEART_2, CARD_HEART_ACE, CARD_SPADE_9, CARD_SPADE_8};
        std::vector<int> result28 = {CARD_HEART_5, CARD_HEART_4, CARD_VALUE_JOKER, CARD_HEART_2, CARD_HEART_ACE, CARD_SPADE_9, CARD_SPADE_8};
        assert (isCorrect(case28, result28, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //大于等于10的对子以及有A或Joker：对10+A
        std::vector<int> case29 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_SPADE_10, CARD_SPADE_5};
        std::vector<int> result29 ={CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_SPADE_10, CARD_SPADE_5};
        assert (isCorrect(case29, result29, PHT_ROYAL_FLUSH, PHT_HIGH_CARD));

        //大于等于10的对子以及有A或Joker：对J+A
        std::vector<int> case30 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_SPADE_JACK, CARD_SPADE_5};
        std::vector<int> result30 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_SPADE_JACK, CARD_SPADE_5};
        assert (isCorrect(case30, result30, PHT_ROYAL_FLUSH, PHT_HIGH_CARD));

        //大于等于10的对子以及有A或Joker：对Q+Joker
        std::vector<int> case31 = {CARD_VALUE_JOKER, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_SPADE_QUEEN, CARD_SPADE_5};
        std::vector<int> result31 = {CARD_VALUE_JOKER, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_SPADE_QUEEN, CARD_SPADE_5};
        assert (isCorrect(case31, result31, PHT_ROYAL_FLUSH, PHT_HIGH_CARD));

        //大于等于10的对子以及有A或Joker：对A
        std::vector<int> case32 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_SPADE_ACE, CARD_SPADE_5};
        std::vector<int> result32 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_SPADE_ACE, CARD_SPADE_5};
        assert (isCorrect(case32, result32, PHT_ROYAL_FLUSH, PHT_HIGH_CARD));

        //6顺：没有Joker
        std::vector<int> case33 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_CLUB_2};
        std::vector<int> result33 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_ACE, CARD_CLUB_2};
        assert (isCorrect(case33, result33, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //6顺：顺子最小值是Joker
        std::vector<int> case34 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_VALUE_JOKER, CARD_CLUB_2};
        std::vector<int> result34 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_VALUE_JOKER, CARD_CLUB_2};
        assert (isCorrect(case34, result34, PHT_ROYAL_FLUSH, PHT_HIGH_CARD));

        //6顺：Joker替换10
        std::vector<int> case35 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_VALUE_JOKER, CARD_SPADE_9, CARD_CLUB_3};
        std::vector<int> result35 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_VALUE_JOKER, CARD_SPADE_9, CARD_SPADE_ACE, CARD_CLUB_3};
        assert (isCorrect(case35, result35, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //6顺：Joker替换J
        std::vector<int> case36 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_VALUE_JOKER, CARD_SPADE_10, CARD_SPADE_9, CARD_CLUB_2};
        std::vector<int> result36 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_VALUE_JOKER, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_ACE, CARD_CLUB_2};
        assert (isCorrect(case36, result36, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //6顺：Joker替换Q
        std::vector<int> case37 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_VALUE_JOKER, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_CLUB_2};
        std::vector<int> result37 = {CARD_SPADE_KING, CARD_VALUE_JOKER, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_ACE, CARD_CLUB_2};
        assert (isCorrect(case37, result37, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //6顺：Joker替换K
        std::vector<int> case38 = {CARD_SPADE_ACE, CARD_VALUE_JOKER, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_CLUB_2};
        std::vector<int> result38 = {CARD_VALUE_JOKER, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_ACE, CARD_CLUB_2};
        assert (isCorrect(case38, result38, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //6顺：Joker替换A
        std::vector<int> case39 = {CARD_VALUE_JOKER, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_CLUB_2};
        std::vector<int> result39 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_VALUE_JOKER, CARD_CLUB_2};
        assert (isCorrect(case39, result39, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //7顺：没有Joker
        std::vector<int> case40 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_8};
        std::vector<int> result40 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_ACE, CARD_SPADE_8};
        assert (isCorrect(case40, result40, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //7顺：Joker替换8
        std::vector<int> case41 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_VALUE_JOKER};
        std::vector<int> result41 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_ACE, CARD_VALUE_JOKER};
        assert (isCorrect(case41, result41, PHT_STRAIGHT_FLUSH, PHT_ONE_PAIR));

        //7顺：Joker替换9
        std::vector<int> case42 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_VALUE_JOKER, CARD_SPADE_8};
        std::vector<int> result42 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_8, CARD_SPADE_ACE, CARD_VALUE_JOKER};
        assert (isCorrect(case42, result42, PHT_FLUSH, PHT_ONE_PAIR));

        //7顺：Joker替换10
        std::vector<int> case43 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_VALUE_JOKER, CARD_SPADE_9, CARD_SPADE_8};
        std::vector<int> result43 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_9, CARD_SPADE_8, CARD_SPADE_ACE, CARD_VALUE_JOKER};
        assert (isCorrect(case43, result43, PHT_FLUSH, PHT_ONE_PAIR));

        //7顺：Joker替换J
        std::vector<int> case44 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_VALUE_JOKER, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_8};
        std::vector<int> result44 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_8, CARD_SPADE_ACE, CARD_VALUE_JOKER};
        assert (isCorrect(case44, result44, PHT_FLUSH, PHT_ONE_PAIR));

        //7顺：Joker替换Q
        std::vector<int> case45 = {CARD_SPADE_ACE, CARD_SPADE_KING, CARD_VALUE_JOKER, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_8};
        std::vector<int> result45 = {CARD_SPADE_KING, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_8, CARD_SPADE_ACE, CARD_VALUE_JOKER};
        assert (isCorrect(case45, result45, PHT_FLUSH, PHT_ONE_PAIR));

        //7顺：Joker替换K
        std::vector<int> case46 = {CARD_SPADE_ACE, CARD_VALUE_JOKER, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_8};
        std::vector<int> result46 = {CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_8, CARD_SPADE_ACE, CARD_VALUE_JOKER};
        assert (isCorrect(case46, result46, PHT_STRAIGHT_FLUSH, PHT_ONE_PAIR));

        //7顺：Joker替换A
        std::vector<int> case47 = {CARD_VALUE_JOKER, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_8};
        std::vector<int> result47 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_VALUE_JOKER, CARD_SPADE_8};
        assert (isCorrect(case47, result47, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));
    }

    void TestStraightFlush7() {
        //SPADE的K+Q+J+10+9
        std::vector<int> case1 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_2, CARD_CLUB_4};
        std::vector<int> result1 = {CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_2, CARD_SPADE_KING, CARD_CLUB_4};
        assert (isCorrect(case1, result1, PHT_FLUSH, PHT_HIGH_CARD));

        //HEART的Q+J+10+9+8+7+6
        std::vector<int> case2 = {CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_9, CARD_HEART_8, CARD_HEART_7, CARD_HEART_6};
        std::vector<int> result2 = {CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_9, CARD_HEART_8, CARD_HEART_7, CARD_HEART_QUEEN, CARD_HEART_6};
        assert (isCorrect(case2, result2, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //CLUB的J+10+9+8+7
        std::vector<int> case3 = {CARD_CLUB_JACK, CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_7, CARD_VALUE_JOKER, CARD_SPADE_2};
        std::vector<int> result3 = {CARD_CLUB_JACK, CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_7, CARD_VALUE_JOKER, CARD_SPADE_2};
        assert (isCorrect(case3, result3, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //DIAMOND的10+9+8+7+6
        std::vector<int> case4 = {CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_7, CARD_DIAMOND_6, CARD_SPADE_2, CARD_CLUB_9};
        std::vector<int> result4 = {CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_7, CARD_DIAMOND_6, CARD_CLUB_9, CARD_SPADE_2};
        assert (isCorrect(case4, result4, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //A作为最小值
        std::vector<int> case5 = {CARD_SPADE_5, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_SPADE_ACE, CARD_CLUB_5, CARD_DIAMOND_KING};
        std::vector<int> result5 = {CARD_SPADE_5, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_SPADE_ACE, CARD_DIAMOND_KING, CARD_CLUB_5};
        assert (isCorrect(case5, result5, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //Joker变为A
        std::vector<int> case6 = {CARD_SPADE_5, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_VALUE_JOKER, CARD_CLUB_8, CARD_CLUB_10};
        std::vector<int> result6 = {CARD_VALUE_JOKER, CARD_SPADE_5, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_CLUB_10, CARD_CLUB_8};
        assert (isCorrect(case6, result6, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //Joker变为其它牌
        std::vector<int> case7 = {CARD_SPADE_9, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_VALUE_JOKER, CARD_HEART_2, CARD_CLUB_10};
        std::vector<int> result7 = {CARD_VALUE_JOKER, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_CLUB_10, CARD_HEART_2};
        assert (isCorrect(case7, result7, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //普通顺子：六张牌顺子，把最大牌放到后面
        std::vector<int> case8 = {CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_7, CARD_CLUB_6, CARD_CLUB_5, CARD_HEART_KING};
        std::vector<int> result8 = {CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_7, CARD_CLUB_6, CARD_HEART_KING, CARD_CLUB_5};
        assert (isCorrect(case8, result8, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //普通顺子：六张牌顺子，把最大牌放到后面
        std::vector<int> case9 = {CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_7, CARD_CLUB_6, CARD_CLUB_5, CARD_HEART_7};
        std::vector<int> result9 = {CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_6, CARD_CLUB_5, CARD_HEART_7, CARD_CLUB_7};
        assert (isCorrect(case9, result9, PHT_FLUSH, PHT_ONE_PAIR));

        //六张牌顺子，A作为最小值，不需要把移动到左手牌
        std::vector<int> case10 = {CARD_SPADE_6, CARD_SPADE_5, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_SPADE_ACE, CARD_DIAMOND_KING};
        std::vector<int> result10 = {CARD_SPADE_6, CARD_SPADE_5, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_SPADE_ACE, CARD_DIAMOND_KING};
        assert (isCorrect(case10, result10, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //六张牌顺子，A作为最小值，不需要把移动到左手牌
        std::vector<int> case11 = {CARD_SPADE_6, CARD_SPADE_5, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_SPADE_ACE, CARD_CLUB_7};
        std::vector<int> result11 = {CARD_SPADE_6, CARD_SPADE_5, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_SPADE_ACE, CARD_CLUB_7};
        assert (isCorrect(case11, result11, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));

        //鬼牌要调整位置：位置1
        std::vector<int> case12 = {CARD_VALUE_JOKER, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_CLUB_QUEEN, CARD_CLUB_8};
        std::vector<int> result12 = {CARD_VALUE_JOKER, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_CLUB_8, CARD_SPADE_QUEEN, CARD_CLUB_QUEEN};
        assert (isCorrect(case12, result12, PHT_STRAIGHT, PHT_ONE_PAIR));

        //鬼牌要调整位置：位置2
        std::vector<int> case13 = {CARD_SPADE_KING, CARD_VALUE_JOKER, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_CLUB_KING, CARD_CLUB_8};
        std::vector<int> result13 = {CARD_VALUE_JOKER, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_CLUB_8, CARD_SPADE_KING, CARD_CLUB_KING};
        assert (isCorrect(case13, result13, PHT_STRAIGHT, PHT_ONE_PAIR));

        //鬼牌要调整位置：位置3
        std::vector<int> case14 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_VALUE_JOKER, CARD_SPADE_10, CARD_SPADE_9, CARD_CLUB_KING, CARD_CLUB_8};
        std::vector<int> result14 = {CARD_SPADE_QUEEN, CARD_VALUE_JOKER, CARD_SPADE_10, CARD_SPADE_9, CARD_CLUB_8, CARD_SPADE_KING, CARD_CLUB_KING,};
        assert (isCorrect(case14, result14, PHT_STRAIGHT, PHT_ONE_PAIR));

        //鬼牌要调整位置：位置4
        std::vector<int> case15 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_VALUE_JOKER, CARD_SPADE_9, CARD_CLUB_KING, CARD_CLUB_8};
        std::vector<int> result15 = {CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_VALUE_JOKER, CARD_SPADE_9, CARD_CLUB_8, CARD_SPADE_KING, CARD_CLUB_KING};
        assert (isCorrect(case15, result15, PHT_STRAIGHT, PHT_ONE_PAIR));

        //鬼牌要调整位置：位置5
        std::vector<int> case16 = {CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_VALUE_JOKER, CARD_CLUB_KING, CARD_CLUB_8};
        std::vector<int> result16 = {CARD_VALUE_JOKER, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_CLUB_KING, CARD_CLUB_8};
        assert (isCorrect(case16, result16, PHT_ROYAL_FLUSH, PHT_HIGH_CARD));

        //同花顺里包含A+Joker
        std::vector<int> case28 = {CARD_HEART_5, CARD_HEART_4, CARD_VALUE_JOKER, CARD_HEART_2, CARD_HEART_ACE, CARD_SPADE_9, CARD_SPADE_8};
        std::vector<int> result28 = {CARD_HEART_5, CARD_HEART_4, CARD_VALUE_JOKER, CARD_HEART_2, CARD_HEART_ACE, CARD_SPADE_9, CARD_SPADE_8};
        assert (isCorrect(case28, result28, PHT_STRAIGHT_FLUSH, PHT_HIGH_CARD));
    }

    void TestFourOfAKind7() {
        //4+3: 2222+444
        std::vector<int> case1 = {CARD_SPADE_2, CARD_HEART_2, CARD_CLUB_2, CARD_DIAMOND_2, CARD_SPADE_4, CARD_HEART_4, CARD_DIAMOND_4};
        std::vector<int> result1 = {CARD_SPADE_2, CARD_HEART_2, CARD_CLUB_2, CARD_DIAMOND_2, CARD_SPADE_4, CARD_HEART_4, CARD_DIAMOND_4};
        assert (isCorrect(case1, result1, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR));

        //4+3: AAAA+555
        std::vector<int> case2 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_SPADE_5, CARD_HEART_5, CARD_DIAMOND_5};
        std::vector<int> result2 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_SPADE_5, CARD_HEART_5, CARD_DIAMOND_5};
        assert (isCorrect(case2, result2, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR));

        //4+3: AAA+Joker+555
        std::vector<int> case3 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_VALUE_JOKER, CARD_SPADE_5, CARD_HEART_5, CARD_DIAMOND_5};
        std::vector<int> result3 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_VALUE_JOKER, CARD_SPADE_5, CARD_HEART_5, CARD_DIAMOND_5};
        assert (isCorrect(case3, result3, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR));

        //4+3: KKKK+AAA
        std::vector<int> case4 = {CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_DIAMOND_KING, CARD_SPADE_ACE, CARD_HEART_ACE, CARD_DIAMOND_ACE};
        std::vector<int> result4 = {CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_DIAMOND_KING, CARD_SPADE_ACE, CARD_HEART_ACE, CARD_DIAMOND_ACE};
        assert (isCorrect(case4, result4, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR));

        //4+2+1: 2222+44+5
        std::vector<int> case5 = {CARD_SPADE_2, CARD_HEART_2, CARD_CLUB_2, CARD_DIAMOND_2, CARD_SPADE_4, CARD_HEART_4, CARD_DIAMOND_5};
        std::vector<int> result5 = {CARD_SPADE_2, CARD_HEART_2, CARD_CLUB_2, CARD_DIAMOND_2, CARD_DIAMOND_5, CARD_SPADE_4, CARD_HEART_4};
        assert (isCorrect(case5, result5, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR));

        //4+2+1: AAAA+77+5
        std::vector<int> case6 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_SPADE_7, CARD_HEART_7, CARD_DIAMOND_5};
        std::vector<int> result6 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_DIAMOND_5, CARD_SPADE_7, CARD_HEART_7};
        assert (isCorrect(case6, result6, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR));

        //4+2+1: AAA+Joker+77+5
        std::vector<int> case7 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_VALUE_JOKER, CARD_SPADE_7, CARD_HEART_7, CARD_DIAMOND_5};
        std::vector<int> result7 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_5, CARD_SPADE_7, CARD_HEART_7};
        assert (isCorrect(case7, result7, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR));

        //4+2+1: KKKK+AA+5
        std::vector<int> case8 = {CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_DIAMOND_KING, CARD_SPADE_ACE, CARD_HEART_ACE, CARD_DIAMOND_5};
        std::vector<int> result8 = {CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_DIAMOND_KING, CARD_DIAMOND_5, CARD_SPADE_ACE, CARD_HEART_ACE};
        assert (isCorrect(case8, result8, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR));

        //4+A+Joker+1: KKKK+A+Joker+5
        std::vector<int> case9 = {CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_DIAMOND_KING, CARD_SPADE_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_5};
        std::vector<int> result9 = {CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_DIAMOND_KING, CARD_DIAMOND_5, CARD_VALUE_JOKER, CARD_SPADE_ACE};
        assert (isCorrect(case9, result9, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR));

        //4+1+1+1: 2222+3+4+5
        std::vector<int> case10 = {CARD_SPADE_2, CARD_HEART_2, CARD_CLUB_2, CARD_DIAMOND_2, CARD_SPADE_3, CARD_HEART_4, CARD_DIAMOND_5};
        std::vector<int> result10 = {CARD_SPADE_2, CARD_HEART_2, CARD_CLUB_2, CARD_DIAMOND_2, CARD_SPADE_3, CARD_DIAMOND_5, CARD_HEART_4};
        assert (isCorrect(case10, result10, PHT_FOUR_OF_A_KIND, PHT_HIGH_CARD));

        //4+1+1+1: 6666+3+4+5
        std::vector<int> case11 = {CARD_SPADE_6, CARD_HEART_6, CARD_CLUB_6, CARD_DIAMOND_6, CARD_SPADE_3, CARD_HEART_4, CARD_DIAMOND_5};
        std::vector<int> result11 = {CARD_SPADE_6, CARD_HEART_6, CARD_CLUB_6, CARD_DIAMOND_6, CARD_SPADE_3, CARD_DIAMOND_5, CARD_HEART_4};
        assert (isCorrect(case11, result11, PHT_FOUR_OF_A_KIND, PHT_HIGH_CARD));

        //4+1+1+1: 7777+3+4+5
        std::vector<int> case12 = {CARD_SPADE_7, CARD_HEART_7, CARD_CLUB_7, CARD_DIAMOND_7, CARD_SPADE_3, CARD_HEART_4, CARD_DIAMOND_5};
        std::vector<int> result12 = {CARD_SPADE_7, CARD_HEART_7, CARD_DIAMOND_5, CARD_HEART_4, CARD_SPADE_3, CARD_CLUB_7, CARD_DIAMOND_7};
        assert (isCorrect(case12, result12, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //4+1+1+1: 7777+A+4+5
        std::vector<int> case13 = {CARD_SPADE_7, CARD_HEART_7, CARD_CLUB_7, CARD_DIAMOND_7, CARD_SPADE_ACE, CARD_HEART_4, CARD_DIAMOND_5};
        std::vector<int> result13 = {CARD_SPADE_7, CARD_HEART_7, CARD_CLUB_7, CARD_DIAMOND_7, CARD_HEART_4, CARD_SPADE_ACE, CARD_DIAMOND_5};
        assert (isCorrect(case13, result13, PHT_FOUR_OF_A_KIND, PHT_HIGH_CARD));

        //4+1+1+1: 10101010+K+4+5
        std::vector<int> case14 = {CARD_SPADE_10, CARD_HEART_10, CARD_CLUB_10, CARD_DIAMOND_10, CARD_SPADE_KING, CARD_HEART_4, CARD_DIAMOND_5};
        std::vector<int> result14 = {CARD_SPADE_10, CARD_HEART_10, CARD_CLUB_10, CARD_DIAMOND_10, CARD_HEART_4, CARD_SPADE_KING, CARD_DIAMOND_5};
        assert (isCorrect(case14, result14, PHT_FOUR_OF_A_KIND, PHT_HIGH_CARD));

        //4+1+1+1: JJJJ+3+4+5
        std::vector<int> case15 = {CARD_SPADE_JACK, CARD_HEART_JACK, CARD_CLUB_JACK, CARD_DIAMOND_JACK, CARD_SPADE_3, CARD_HEART_4, CARD_DIAMOND_5};
        std::vector<int> result15 = {CARD_SPADE_JACK, CARD_HEART_JACK, CARD_DIAMOND_5, CARD_HEART_4, CARD_SPADE_3, CARD_CLUB_JACK, CARD_DIAMOND_JACK};
        assert (isCorrect(case15, result15, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //4+1+1+1: JJJJ+Joker+4+5
        std::vector<int> case16 = {CARD_SPADE_JACK, CARD_HEART_JACK, CARD_CLUB_JACK, CARD_DIAMOND_JACK, CARD_VALUE_JOKER, CARD_HEART_4, CARD_DIAMOND_5};
        std::vector<int> result16 = {CARD_SPADE_JACK, CARD_HEART_JACK, CARD_CLUB_JACK, CARD_DIAMOND_JACK, CARD_HEART_4, CARD_VALUE_JOKER, CARD_DIAMOND_5};
        assert (isCorrect(case16, result16, PHT_FOUR_OF_A_KIND, PHT_HIGH_CARD));

        //4+1+1+1: KKKK+A+4+5
        std::vector<int> case17 = {CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_DIAMOND_KING, CARD_SPADE_ACE, CARD_HEART_4, CARD_DIAMOND_5};
        std::vector<int> result17 = {CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_DIAMOND_KING, CARD_HEART_4, CARD_SPADE_ACE, CARD_DIAMOND_5};
        assert (isCorrect(case17, result17, PHT_FOUR_OF_A_KIND, PHT_HIGH_CARD));

        //4+1+1+1: AAAA+3+4+5
        std::vector<int> case18 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_SPADE_3, CARD_HEART_4, CARD_DIAMOND_5};
        std::vector<int> result18 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_DIAMOND_5, CARD_HEART_4, CARD_SPADE_3, CARD_CLUB_ACE, CARD_DIAMOND_ACE};
        assert (isCorrect(case18, result18, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //4+1+1+1: AAA+Joker+3+4+5
        std::vector<int> case19 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_VALUE_JOKER, CARD_SPADE_3, CARD_HEART_4, CARD_DIAMOND_5};
        std::vector<int> result19 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_DIAMOND_5, CARD_HEART_4, CARD_SPADE_3, CARD_CLUB_ACE, CARD_VALUE_JOKER};
        assert (isCorrect(case19, result19, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //4+2+1: KKKK+AA+Joker
        std::vector<int> case20 = {CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_DIAMOND_KING, CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER};
        std::vector<int> result20 = {CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_DIAMOND_KING, CARD_VALUE_JOKER, CARD_SPADE_ACE, CARD_HEART_ACE};
        assert (isCorrect(case20, result20, PHT_FOUR_OF_A_KIND, PHT_ONE_PAIR));
    }

    void TestFlush7() {
        //SPADE：普通的同花+高牌
        std::vector<int> case1 = {CARD_SPADE_KING, CARD_SPADE_JACK, CARD_SPADE_3, CARD_SPADE_9, CARD_SPADE_2, CARD_CLUB_KING, CARD_DIAMOND_ACE};
        std::vector<int> result1 = {CARD_SPADE_KING, CARD_SPADE_JACK, CARD_SPADE_9, CARD_SPADE_3, CARD_SPADE_2, CARD_DIAMOND_ACE, CARD_CLUB_KING};
        assert (isCorrect(case1, result1, PHT_FLUSH, PHT_HIGH_CARD));

        //HEART：普通的同花+高牌
        std::vector<int> case2 = {CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_5, CARD_HEART_8, CARD_HEART_2, CARD_CLUB_9, CARD_DIAMOND_4};
        std::vector<int> result2 = {CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_8, CARD_HEART_5, CARD_HEART_2, CARD_CLUB_9, CARD_DIAMOND_4};
        assert (isCorrect(case2, result2, PHT_FLUSH, PHT_HIGH_CARD));

        //CLUB：使用两对规则
        std::vector<int> case3 = {CARD_CLUB_JACK, CARD_CLUB_KING, CARD_CLUB_9, CARD_CLUB_ACE, CARD_CLUB_2, CARD_SPADE_JACK, CARD_SPADE_ACE};
        std::vector<int> result3 = {CARD_SPADE_ACE, CARD_CLUB_ACE, CARD_CLUB_KING, CARD_CLUB_9, CARD_CLUB_2, CARD_SPADE_JACK, CARD_CLUB_JACK};
        assert (isCorrect(case3, result3, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //DIAMOND：七张牌同花色
        std::vector<int> case4 = {CARD_DIAMOND_ACE, CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_7, CARD_DIAMOND_4, CARD_DIAMOND_2};
        std::vector<int> result4 = {CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_7, CARD_DIAMOND_4, CARD_DIAMOND_ACE, CARD_DIAMOND_2};
        assert (isCorrect(case4, result4, PHT_FLUSH, PHT_HIGH_CARD));

        //SPADE+Joker：普通同花+高牌
        std::vector<int> case5 = {CARD_SPADE_KING, CARD_SPADE_JACK, CARD_SPADE_3, CARD_SPADE_9, CARD_VALUE_JOKER, CARD_CLUB_5, CARD_CLUB_9};
        std::vector<int> result5 = {CARD_VALUE_JOKER, CARD_SPADE_KING, CARD_SPADE_JACK, CARD_SPADE_9, CARD_SPADE_3, CARD_CLUB_9, CARD_CLUB_5};
        assert (isCorrect(case5, result5, PHT_FLUSH, PHT_HIGH_CARD));

        //HEART+Joker：普通同花+高牌
        std::vector<int> case6 = {CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_5, CARD_HEART_8, CARD_VALUE_JOKER, CARD_DIAMOND_4, CARD_DIAMOND_9};
        std::vector<int> result6 = {CARD_VALUE_JOKER, CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_8, CARD_HEART_5, CARD_DIAMOND_9, CARD_DIAMOND_4};
        assert (isCorrect(case6, result6, PHT_FLUSH, PHT_HIGH_CARD));

        //CLUB：普通同花+高牌
        std::vector<int> case7 = {CARD_CLUB_JACK, CARD_CLUB_KING, CARD_CLUB_9, CARD_CLUB_ACE, CARD_VALUE_JOKER, CARD_SPADE_ACE, CARD_HEART_7};
        std::vector<int> result7 = {CARD_VALUE_JOKER, CARD_CLUB_ACE, CARD_CLUB_KING, CARD_CLUB_JACK, CARD_CLUB_9, CARD_SPADE_ACE, CARD_HEART_7};
        assert (isCorrect(case7, result7, PHT_FLUSH, PHT_HIGH_CARD));

        //DIAMOND：两对
        std::vector<int> case8 = {CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_4, CARD_VALUE_JOKER, CARD_SPADE_ACE, CARD_CLUB_9};
        std::vector<int> result8 = {CARD_SPADE_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_10, CARD_DIAMOND_8, CARD_DIAMOND_4, CARD_CLUB_9, CARD_DIAMOND_9};
        assert (isCorrect(case8, result8, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //不同花色：四条
        std::vector<int> case9 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_CLUB_KING, CARD_SPADE_2, CARD_CLUB_9};
        std::vector<int> result9 = {CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_KING, CARD_CLUB_9, CARD_SPADE_2, CARD_CLUB_ACE, CARD_DIAMOND_ACE};
        assert (isCorrect(case9, result9, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //同花色+普通对子
        std::vector<int> case10 = {CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_7, CARD_CLUB_7, CARD_DIAMOND_ACE, CARD_DIAMOND_4};
        std::vector<int> result10 = {CARD_DIAMOND_ACE, CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_4, CARD_CLUB_7, CARD_DIAMOND_7};
        assert (isCorrect(case10, result10, PHT_FLUSH, PHT_ONE_PAIR));

        //鬼牌位置应该放在首位，五张同花色
        std::vector<int> case11 = {CARD_DIAMOND_KING, CARD_DIAMOND_JACK, CARD_DIAMOND_9, CARD_DIAMOND_5, CARD_VALUE_JOKER, CARD_HEART_JACK, CARD_SPADE_7};
        std::vector<int> result11 = {CARD_VALUE_JOKER, CARD_DIAMOND_KING, CARD_DIAMOND_JACK, CARD_DIAMOND_9, CARD_DIAMOND_5, CARD_HEART_JACK, CARD_SPADE_7};
        assert (isCorrect(case11, result11, PHT_FLUSH, PHT_HIGH_CARD));

        //DIAMOND：六张牌同花色
        std::vector<int> case12 = {CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_7, CARD_DIAMOND_2, CARD_DIAMOND_ACE, CARD_CLUB_4};
        std::vector<int> result12 = {CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_7, CARD_DIAMOND_2, CARD_DIAMOND_ACE, CARD_CLUB_4};
        assert (isCorrect(case12, result12, PHT_FLUSH, PHT_HIGH_CARD));

        //DIAMOND：三条,AAA
        std::vector<int> case13 = {CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_7, CARD_DIAMOND_5, CARD_DIAMOND_4, CARD_CLUB_ACE, CARD_HEART_ACE};
        std::vector<int> result13 = {CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_7, CARD_DIAMOND_5, CARD_DIAMOND_4, CARD_HEART_ACE, CARD_CLUB_ACE};
        assert (isCorrect(case13, result13, PHT_FLUSH, PHT_ONE_PAIR));

        //DIAMOND：三条,Joker+AA
        std::vector<int> case14 = {CARD_VALUE_JOKER, CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_7, CARD_DIAMOND_5, CARD_CLUB_ACE, CARD_HEART_4};
        std::vector<int> result14 = {CARD_VALUE_JOKER, CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_7, CARD_DIAMOND_5, CARD_CLUB_ACE, CARD_HEART_4};
        assert (isCorrect(case14, result14, PHT_FLUSH, PHT_HIGH_CARD));

        //DIAMOND：三条,QJ875+55
        std::vector<int> case15 = {CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_8, CARD_DIAMOND_7, CARD_DIAMOND_5, CARD_CLUB_5, CARD_HEART_5};
        std::vector<int> result15 = {CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_8, CARD_DIAMOND_7, CARD_DIAMOND_5, CARD_HEART_5, CARD_CLUB_5};
        assert (isCorrect(case15, result15, PHT_FLUSH, PHT_ONE_PAIR));

        //SPADE：六个同花+高牌
        std::vector<int> case16 = {CARD_SPADE_KING, CARD_SPADE_JACK, CARD_SPADE_9, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_DIAMOND_ACE};
        std::vector<int> result16 = {CARD_SPADE_KING, CARD_SPADE_JACK, CARD_SPADE_9, CARD_SPADE_4, CARD_SPADE_3, CARD_DIAMOND_ACE, CARD_SPADE_2};
        assert (isCorrect(case16, result16, PHT_FLUSH, PHT_HIGH_CARD));

        //SPADE：六个同花+高牌
        std::vector<int> case17 = {CARD_SPADE_KING, CARD_SPADE_JACK, CARD_SPADE_9, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_DIAMOND_8};
        std::vector<int> result17 = {CARD_SPADE_JACK, CARD_SPADE_9, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_SPADE_KING, CARD_DIAMOND_8};
        assert (isCorrect(case17, result17, PHT_FLUSH, PHT_HIGH_CARD));

        //SPADE：五个同花+底牌顺子
        std::vector<int> case18 = {CARD_SPADE_KING, CARD_SPADE_JACK, CARD_SPADE_9, CARD_SPADE_8, CARD_SPADE_7, CARD_CLUB_6, CARD_DIAMOND_5};
        std::vector<int> result18 = {CARD_SPADE_9, CARD_SPADE_8, CARD_SPADE_7, CARD_CLUB_6, CARD_DIAMOND_5, CARD_SPADE_KING, CARD_SPADE_JACK};
        assert (isCorrect(case18, result18, PHT_STRAIGHT, PHT_HIGH_CARD));
    }

    void TestStraight7() {
        //最大顺子：没有对子的普通顺子
        std::vector<int> case1 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_DIAMOND_10, CARD_VALUE_JOKER, CARD_SPADE_2};
        std::vector<int> result1 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_DIAMOND_10, CARD_VALUE_JOKER, CARD_SPADE_2};
        assert (isCorrect(case1, result1, PHT_STRAIGHT, PHT_HIGH_CARD));

        //最大顺子+Joker：两对
        std::vector<int> case2 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_VALUE_JOKER, CARD_CLUB_JACK, CARD_DIAMOND_10, CARD_HEART_JACK, CARD_SPADE_2};
        std::vector<int> result2 = {CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_HEART_KING, CARD_DIAMOND_10, CARD_SPADE_2, CARD_HEART_JACK, CARD_CLUB_JACK};
        assert (isCorrect(case2, result2, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //最大顺子+Joker：没有对子的普通顺子
        std::vector<int> case3 = {CARD_VALUE_JOKER, CARD_HEART_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_DIAMOND_10, CARD_HEART_2, CARD_CLUB_3};
        std::vector<int> result3 = {CARD_VALUE_JOKER, CARD_HEART_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_DIAMOND_10, CARD_CLUB_3, CARD_HEART_2};
        assert (isCorrect(case3, result3, PHT_STRAIGHT, PHT_HIGH_CARD));

        //最小顺子：没有对子
        std::vector<int> case4 = {CARD_CLUB_5, CARD_HEART_4, CARD_CLUB_3, CARD_CLUB_2, CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_SPADE_QUEEN};
        std::vector<int> result4 = {CARD_CLUB_5, CARD_HEART_4, CARD_CLUB_3, CARD_CLUB_2, CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_SPADE_QUEEN};
        assert (isCorrect(case4, result4, PHT_STRAIGHT, PHT_HIGH_CARD));

        //最小顺子+joker：可以是顺子+对子，也可以是两对，默认使用前者
        std::vector<int> case5 = {CARD_CLUB_5, CARD_HEART_4, CARD_CLUB_3, CARD_VALUE_JOKER, CARD_DIAMOND_ACE, CARD_HEART_10, CARD_DIAMOND_10};
        std::vector<int> result5 = {CARD_CLUB_5, CARD_HEART_4, CARD_CLUB_3, CARD_VALUE_JOKER, CARD_DIAMOND_ACE, CARD_HEART_10, CARD_DIAMOND_10};
        assert (isCorrect(case5, result5, PHT_STRAIGHT, PHT_ONE_PAIR));

        //最小顺子+joker：可以是顺子+对子，也可以是两对，默认使用前者
        std::vector<int> case6 = {CARD_CLUB_2, CARD_HEART_4, CARD_CLUB_3, CARD_VALUE_JOKER, CARD_DIAMOND_ACE, CARD_HEART_10, CARD_DIAMOND_10};
        std::vector<int> result6 = {CARD_VALUE_JOKER, CARD_HEART_4, CARD_CLUB_3, CARD_CLUB_2, CARD_DIAMOND_ACE, CARD_HEART_10, CARD_DIAMOND_10};
        assert (isCorrect(case6, result6, PHT_STRAIGHT, PHT_ONE_PAIR));

        //普通顺子：七张牌顺子，把最大的两张牌放到后面
        std::vector<int> case7 = {CARD_CLUB_10, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6, CARD_DIAMOND_5, CARD_HEART_4};
        std::vector<int> result7 = {CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6, CARD_DIAMOND_5, CARD_HEART_4, CARD_CLUB_10, CARD_HEART_9};
        assert (isCorrect(case7, result7, PHT_STRAIGHT, PHT_HIGH_CARD));

        //普通顺子+Joker
        std::vector<int> case8 = {CARD_CLUB_10, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_VALUE_JOKER, CARD_CLUB_2, CARD_HEART_3};
        std::vector<int> result8 = {CARD_VALUE_JOKER, CARD_CLUB_10, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_HEART_3, CARD_CLUB_2};
        assert (isCorrect(case8, result8, PHT_STRAIGHT, PHT_HIGH_CARD));

        //普通顺子+Joker
        std::vector<int> case9 = {CARD_VALUE_JOKER, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6, CARD_DIAMOND_KING, CARD_CLUB_QUEEN};
        std::vector<int> result9 = {CARD_VALUE_JOKER, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6, CARD_DIAMOND_KING, CARD_CLUB_QUEEN};
        assert (isCorrect(case9, result9, PHT_STRAIGHT, PHT_HIGH_CARD));

        //不是顺子
        std::vector<int> case10 = {CARD_HEART_8, CARD_HEART_3, CARD_CLUB_4, CARD_SPADE_KING, CARD_DIAMOND_QUEEN, CARD_SPADE_ACE, CARD_DIAMOND_3};
        std::vector<int> result10 = {CARD_HEART_3, CARD_DIAMOND_3, CARD_DIAMOND_QUEEN, CARD_HEART_8, CARD_CLUB_4, CARD_SPADE_ACE, CARD_SPADE_KING};
        assert (isCorrect(case10, result10, PHT_ONE_PAIR, PHT_HIGH_CARD));

        //普通顺子+Joker+A作为对子
        std::vector<int> case11 = {CARD_VALUE_JOKER, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6, CARD_CLUB_5, CARD_CLUB_ACE};
        std::vector<int> result11 = {CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6, CARD_CLUB_5, CARD_CLUB_ACE, CARD_VALUE_JOKER};
        assert (isCorrect(case11, result11, PHT_STRAIGHT, PHT_ONE_PAIR));

        //顺子+三条
        std::vector<int> case12 = {CARD_HEART_10, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6, CARD_HEART_6, CARD_CLUB_6};
        std::vector<int> result12 = {CARD_HEART_10, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_HEART_6, CARD_CLUB_6, CARD_DIAMOND_6};
        assert (isCorrect(case12, result12, PHT_STRAIGHT, PHT_ONE_PAIR));

        //普通顺子：六张牌顺子，把最大牌放到后面
        std::vector<int> case13 = {CARD_CLUB_10, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6, CARD_DIAMOND_5, CARD_HEART_KING};
        std::vector<int> result13 = {CARD_CLUB_10, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6, CARD_HEART_KING, CARD_DIAMOND_5};
        assert (isCorrect(case13, result13, PHT_STRAIGHT, PHT_HIGH_CARD));

        //普通顺子：六张牌顺子，把最大牌放到后面
        std::vector<int> case14 = {CARD_CLUB_10, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6, CARD_DIAMOND_5, CARD_HEART_7};
        std::vector<int> result14 = {CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6, CARD_DIAMOND_5, CARD_CLUB_10, CARD_HEART_7};
        assert (isCorrect(case14, result14, PHT_STRAIGHT, PHT_HIGH_CARD));

        //六张牌顺子，A作为最小值，不需要把移动到左手牌
        std::vector<int> case15 = {CARD_SPADE_6, CARD_HEART_5, CARD_CLUB_4, CARD_DIAMOND_3, CARD_SPADE_2, CARD_CLUB_ACE, CARD_DIAMOND_KING};
        std::vector<int> result15 = {CARD_SPADE_6, CARD_HEART_5, CARD_CLUB_4, CARD_DIAMOND_3, CARD_SPADE_2, CARD_CLUB_ACE, CARD_DIAMOND_KING};
        assert (isCorrect(case15, result15, PHT_STRAIGHT, PHT_HIGH_CARD));

        //顺子里包含A+Joker
        std::vector<int> case16 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_VALUE_JOKER, CARD_CLUB_JACK, CARD_CLUB_10, CARD_SPADE_3, CARD_SPADE_2};
        std::vector<int> result16 = {CARD_HEART_ACE, CARD_HEART_KING, CARD_VALUE_JOKER, CARD_CLUB_JACK, CARD_CLUB_10, CARD_SPADE_3, CARD_SPADE_2};
        assert (isCorrect(case16, result16, PHT_STRAIGHT, PHT_HIGH_CARD));

        //顺子里包含A+Joker
        std::vector<int> case17 = {CARD_HEART_5, CARD_HEART_4, CARD_VALUE_JOKER, CARD_CLUB_2, CARD_CLUB_ACE, CARD_SPADE_9, CARD_SPADE_8};
        std::vector<int> result17 = {CARD_HEART_5, CARD_HEART_4, CARD_VALUE_JOKER, CARD_CLUB_2, CARD_CLUB_ACE, CARD_SPADE_9, CARD_SPADE_8};
        assert (isCorrect(case17, result17, PHT_STRAIGHT, PHT_HIGH_CARD));
    }
};

PAI_GOW_NS_END