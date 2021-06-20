﻿#pragma once
#include <memory>
#include <assert.h>
#include "PokerHands.h"

PAI_GOW_NS_BEGIN

class PokerHandsUnitTest {
public:

    void Test() {
        TestCardsValid();
        TestSevenCardStraightFlush();
        TestRoyalFlushWithSuitedRoyals();
        TestSevenCardStraightFlushWithJoker();
        TestFiveAces5();
        TestFiveAces7();
        TestRoyalFlush5();
        TestRoyalFlush7();
        TestStraightFlush5();
        TestStraightFlush7();
        TestFourOfAKind5();
        TestFourOfAKind7();
        TestFullHouse5();
        TestFullHouse7();
        TestFlush5();
        TestFlush7();
        TestStraight5();
        TestStraight7();
        TestThreeOfAKind5();
        TestThreeOfAKind7();
        TestTwoPair5();
        TestTwoPair7();
        TestOnePair5();
        TestOnePair7();
        TestHighCard();
    }

private:

    virtual void TestCardsValid() {
        //空数组
        std::vector<int> empty;
        assert(!m_test->parseForTip(empty));

        //牌值错误
        std::vector<int> case2 = { 0, 0, 0, 0, 0, 0, 0 };
        assert(!m_test->parseForTip(case2));

        //只有一张正确的牌
        std::vector<int> case3 = { CARD_DIAMOND_8, 0, 0, 0, 0, 0, 0 };
        assert(!m_test->parseForTip(case3));

        //两张完全一样的牌
        std::vector<int> case4 = { CARD_DIAMOND_8, CARD_CLUB_KING, CARD_CLUB_6, CARD_HEART_JACK, CARD_DIAMOND_8 };
        assert(!m_test->parseForTip(case4));
    }

    virtual void TestSevenCardStraightFlush() {
        //SPADE的A+K+Q+J+10+9+8
        std::vector<int> case1 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_8 };
        std::vector<int> result1 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_8 };
        assert(isCorrect(case1, result1, PHT_SEVEN_CARD_STRAIGHT_FLUSH));

        //HEART的K+Q+J+10+9+8+7
        std::vector<int> case2 = { CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_9, CARD_HEART_8, CARD_HEART_7 };
        std::vector<int> result2 = { CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_9, CARD_HEART_8, CARD_HEART_7 };
        assert(isCorrect(case2, result2, PHT_SEVEN_CARD_STRAIGHT_FLUSH));

        //CLUB的10+9+8+7+6+5+4
        std::vector<int> case3 = { CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_7, CARD_CLUB_6, CARD_CLUB_5, CARD_CLUB_4 };
        std::vector<int> result3 = { CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_7, CARD_CLUB_6, CARD_CLUB_5, CARD_CLUB_4 };
        assert(isCorrect(case3, result3, PHT_SEVEN_CARD_STRAIGHT_FLUSH));

        //DIAMOND的7+6+5+4+3+2+A
        std::vector<int> case4 = { CARD_DIAMOND_7, CARD_DIAMOND_6, CARD_DIAMOND_5, CARD_DIAMOND_4, CARD_DIAMOND_3, CARD_DIAMOND_2, CARD_DIAMOND_ACE };
        std::vector<int> result4 = { CARD_DIAMOND_7, CARD_DIAMOND_6, CARD_DIAMOND_5, CARD_DIAMOND_4, CARD_DIAMOND_3, CARD_DIAMOND_2, CARD_DIAMOND_ACE };
        assert(isCorrect(case4, result4, PHT_SEVEN_CARD_STRAIGHT_FLUSH));

        //CLUB的10+9+8+7+6+5+Joker，用来测试不能包含Joker的条件
        std::vector<int> case5 = { CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_7, CARD_CLUB_6, CARD_CLUB_5, CARD_VALUE_JOKER };
        std::vector<int> result5 = { CARD_VALUE_JOKER, CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_7, CARD_CLUB_6, CARD_CLUB_5 };
        assert(isCorrect(case5, result5, PHT_SEVEN_CARD_STRAIGHT_FLUSH_WITH_JOKER));
    }

    virtual void TestRoyalFlushWithSuitedRoyals() {
        //SPADE的A+K+Q+J+10+K+Q
        std::vector<int> case1 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_KING, CARD_HEART_QUEEN };
        std::vector<int> result1 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_KING, CARD_HEART_QUEEN };
        assert(isCorrect(case1, result1, PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS));

        //HEART的A+K+Q+J+10+K+Q
        std::vector<int> case2 = { CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_CLUB_KING, CARD_CLUB_QUEEN };
        std::vector<int> result2 = { CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_CLUB_KING, CARD_CLUB_QUEEN };
        assert(isCorrect(case2, result2, PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS));

        //CLUB的A+K+Q+J+10+K+Q
        std::vector<int> case3 = { CARD_CLUB_ACE, CARD_CLUB_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_CLUB_10, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN };
        std::vector<int> result3 = { CARD_CLUB_ACE, CARD_CLUB_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_CLUB_10, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN };
        assert(isCorrect(case3, result3, PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS));

        //DIAMOND的A+K+Q+J+10+K+Q
        std::vector<int> case4 = { CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_10, CARD_SPADE_KING, CARD_SPADE_QUEEN };
        std::vector<int> result4 = { CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_10, CARD_SPADE_KING, CARD_SPADE_QUEEN };
        assert(isCorrect(case4, result4, PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS));

        //缺10
        std::vector<int> case5 = { CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_VALUE_JOKER, CARD_CLUB_KING, CARD_CLUB_QUEEN };
        std::vector<int> result5 = { CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_VALUE_JOKER, CARD_CLUB_KING, CARD_CLUB_QUEEN };
        assert(isCorrect(case5, result5, PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS));

        //缺J
        std::vector<int> case6 = { CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_VALUE_JOKER, CARD_DIAMOND_10, CARD_CLUB_KING, CARD_CLUB_QUEEN };
        std::vector<int> result6 = { CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_VALUE_JOKER, CARD_DIAMOND_10, CARD_CLUB_KING, CARD_CLUB_QUEEN };
        assert(isCorrect(case6, result6, PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS));

        //缺Q
        std::vector<int> case7 = { CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_JACK, CARD_VALUE_JOKER, CARD_HEART_10, CARD_CLUB_KING, CARD_CLUB_QUEEN };
        std::vector<int> result7 = { CARD_HEART_ACE, CARD_HEART_KING, CARD_VALUE_JOKER, CARD_HEART_JACK, CARD_HEART_10, CARD_CLUB_KING, CARD_CLUB_QUEEN };
        assert(isCorrect(case7, result7, PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS));

        //缺K
        std::vector<int> case8 = { CARD_HEART_ACE, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_VALUE_JOKER, CARD_HEART_10, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN };
        std::vector<int> result8 = { CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN };
        assert(isCorrect(case8, result8, PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS));

        //缺A
        std::vector<int> case9 = { CARD_DIAMOND_JACK, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_VALUE_JOKER, CARD_DIAMOND_10, CARD_CLUB_KING, CARD_CLUB_QUEEN };
        std::vector<int> result9 = { CARD_VALUE_JOKER, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_10, CARD_CLUB_KING, CARD_CLUB_QUEEN };
        assert(isCorrect(case9, result9, PHT_ROYAL_FLUSH_WITH_SUITED_ROYALS));

        //SPADE的A+K+Q+J+10+Q+Q，右手牌不是K
        std::vector<int> case10 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_QUEEN, CARD_CLUB_QUEEN };
        std::vector<int> result10 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_QUEEN, CARD_CLUB_QUEEN };
        assert(isCorrect(case10, result10, PHT_ROYAL_FLUSH));

        //SPADE的A+K+Q+J+10+K+K，右手牌不是Q
        std::vector<int> case11 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_KING, CARD_CLUB_KING };
        std::vector<int> result11 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_KING, CARD_CLUB_KING };
        assert(isCorrect(case11, result11, PHT_ROYAL_FLUSH));

        //SPADE的A+K+Q+J+10+K+Q，右手牌花色不同，变成两对
        std::vector<int> case12 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_KING, CARD_CLUB_QUEEN };
        std::vector<int> result12 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_HEART_KING, CARD_CLUB_QUEEN };
        assert(isCorrect(case12, result12, PHT_ROYAL_FLUSH));
    }

    virtual void TestSevenCardStraightFlushWithJoker() {
        //SPADE的Joker+K+Q+J+10+9+8
        std::vector<int> case1 = { CARD_VALUE_JOKER, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_8 };
        std::vector<int> result1 = { CARD_VALUE_JOKER, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_SPADE_8 };
        assert(isCorrect(case1, result1, PHT_SEVEN_CARD_STRAIGHT_FLUSH_WITH_JOKER));

        //HEART的K+Joker+J+10+9+8+7
        std::vector<int> case2 = { CARD_HEART_KING, CARD_VALUE_JOKER, CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_9, CARD_HEART_8, CARD_HEART_7 };
        std::vector<int> result2 = { CARD_HEART_KING, CARD_VALUE_JOKER, CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_9, CARD_HEART_8, CARD_HEART_7 };
        assert(isCorrect(case2, result2, PHT_SEVEN_CARD_STRAIGHT_FLUSH_WITH_JOKER));

        //CLUB的10+9+Joker+7+6+5+4
        std::vector<int> case3 = { CARD_CLUB_10, CARD_CLUB_9, CARD_VALUE_JOKER, CARD_CLUB_7, CARD_CLUB_6, CARD_CLUB_5, CARD_CLUB_4 };
        std::vector<int> result3 = { CARD_CLUB_10, CARD_CLUB_9, CARD_VALUE_JOKER, CARD_CLUB_7, CARD_CLUB_6, CARD_CLUB_5, CARD_CLUB_4 };
        assert(isCorrect(case3, result3, PHT_SEVEN_CARD_STRAIGHT_FLUSH_WITH_JOKER));

        //DIAMOND的7+6+5+Joker+3+2+A
        std::vector<int> case4 = { CARD_DIAMOND_7, CARD_DIAMOND_6, CARD_DIAMOND_5, CARD_VALUE_JOKER, CARD_DIAMOND_3, CARD_DIAMOND_2, CARD_DIAMOND_ACE };
        std::vector<int> result4 = { CARD_DIAMOND_7, CARD_DIAMOND_6, CARD_DIAMOND_5, CARD_VALUE_JOKER, CARD_DIAMOND_3, CARD_DIAMOND_2, CARD_DIAMOND_ACE };
        assert(isCorrect(case4, result4, PHT_SEVEN_CARD_STRAIGHT_FLUSH_WITH_JOKER));

        //DIAMOND的7+6+5+4+3+2+JOKER
        std::vector<int> case5 = { CARD_DIAMOND_7, CARD_DIAMOND_6, CARD_DIAMOND_5, CARD_DIAMOND_4, CARD_DIAMOND_3, CARD_DIAMOND_2, CARD_VALUE_JOKER };
        std::vector<int> result5 = { CARD_VALUE_JOKER, CARD_DIAMOND_7, CARD_DIAMOND_6, CARD_DIAMOND_5, CARD_DIAMOND_4, CARD_DIAMOND_3, CARD_DIAMOND_2 };
        assert(isCorrect(case5, result5, PHT_SEVEN_CARD_STRAIGHT_FLUSH_WITH_JOKER));

        //无鬼
        std::vector<int> case6 = { CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_7, CARD_CLUB_6, CARD_CLUB_5, CARD_CLUB_4 };
        std::vector<int> result6 = { CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_7, CARD_CLUB_6, CARD_CLUB_5, CARD_CLUB_4 };
        assert(isCorrect(case6, result6, PHT_SEVEN_CARD_STRAIGHT_FLUSH));

        //SPADE的A+K+Q+J+10+9+8
        std::vector<int> case7 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_VALUE_JOKER };
        std::vector<int> result7 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9, CARD_VALUE_JOKER };
        assert(isCorrect(case7, result7, PHT_SEVEN_CARD_STRAIGHT_FLUSH_WITH_JOKER));
    }

    virtual void TestFiveAces5() {
        //四张Ace和一张Joker
        std::vector<int> case1 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_VALUE_JOKER };
        std::vector<int> result1 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_VALUE_JOKER };
        assert(isCorrect(case1, result1, PHT_FIVE_ACES, PHT_COUNT));

        //三张Ace和一张Joker
        std::vector<int> case2 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_VALUE_JOKER, CARD_SPADE_QUEEN };
        std::vector<int> result2 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_VALUE_JOKER, CARD_SPADE_QUEEN };
        assert(isCorrect(case2, result2, PHT_FOUR_OF_A_KIND, PHT_COUNT));

        //四张Ace，缺少Joker
        std::vector<int> case3 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_SPADE_QUEEN };
        std::vector<int> result3 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_SPADE_QUEEN };
        assert(isCorrect(case3, result3, PHT_FOUR_OF_A_KIND, PHT_COUNT));
    }

    virtual void TestFiveAces7() {
        //四张Ace和一张Joker，没有对K
        std::vector<int> case1 = { CARD_VALUE_JOKER, CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_SPADE_QUEEN, CARD_HEART_2 };
        std::vector<int> result1 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_SPADE_QUEEN, CARD_HEART_2, CARD_CLUB_ACE, CARD_DIAMOND_ACE };
        assert(isCorrect(case1, result1, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));

        //四张Ace和一张Joker，有对K
        std::vector<int> case2 = { CARD_VALUE_JOKER, CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_SPADE_KING, CARD_HEART_KING };
        std::vector<int> result2 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_VALUE_JOKER, CARD_SPADE_KING, CARD_HEART_KING };
        assert(isCorrect(case2, result2, PHT_FIVE_ACES, PHT_ONE_PAIR));

        //四张Ace和一张Joker，有对2
        std::vector<int> case3 = { CARD_VALUE_JOKER, CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_SPADE_2, CARD_HEART_2 };
        std::vector<int> result3 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_SPADE_2, CARD_HEART_2, CARD_CLUB_ACE, CARD_DIAMOND_ACE };
        assert(isCorrect(case3, result3, PHT_FULL_HOUSE, PHT_ONE_PAIR));

        //四张Ace和一张Joker，其它单牌
        std::vector<int> case4 = { CARD_VALUE_JOKER, CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_SPADE_3, CARD_HEART_2 };
        std::vector<int> result4 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_SPADE_3, CARD_HEART_2, CARD_CLUB_ACE, CARD_DIAMOND_ACE };
        assert(isCorrect(case4, result4, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));
    }

    virtual void TestRoyalFlush5() {
        //SPADE的A+K+Q+J+10
        std::vector<int> case1 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10 };
        std::vector<int> result1 = { CARD_SPADE_ACE, CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10 };
        assert(isCorrect(case1, result1, PHT_ROYAL_FLUSH, PHT_COUNT));

        //HEART的A+K+Q+J+10
        std::vector<int> case2 = { CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10 };
        std::vector<int> result2 = { CARD_HEART_ACE, CARD_HEART_KING, CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10 };
        assert(isCorrect(case2, result2, PHT_ROYAL_FLUSH, PHT_COUNT));

        //CLUB的A+K+Q+J+10
        std::vector<int> case3 = { CARD_CLUB_ACE, CARD_CLUB_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_CLUB_10 };
        std::vector<int> result3 = { CARD_CLUB_ACE, CARD_CLUB_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_CLUB_10 };
        assert(isCorrect(case3, result3, PHT_ROYAL_FLUSH, PHT_COUNT));

        //DIAMOND的A+K+Q+J+10
        std::vector<int> case4 = { CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_10 };
        std::vector<int> result4 = { CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_10 };
        assert(isCorrect(case4, result4, PHT_ROYAL_FLUSH, PHT_COUNT));

        //缺10
        std::vector<int> case5 = { CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_VALUE_JOKER };
        std::vector<int> result5 = { CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_VALUE_JOKER };
        assert(isCorrect(case5, result5, PHT_ROYAL_FLUSH, PHT_COUNT));

        //缺J
        std::vector<int> case6 = { CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_VALUE_JOKER, CARD_DIAMOND_10 };
        std::vector<int> result6 = { CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_VALUE_JOKER, CARD_DIAMOND_10 };
        assert(isCorrect(case6, result6, PHT_ROYAL_FLUSH, PHT_COUNT));

        //缺Q
        std::vector<int> case7 = { CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_VALUE_JOKER, CARD_DIAMOND_JACK, CARD_DIAMOND_10 };
        std::vector<int> result7 = { CARD_DIAMOND_ACE, CARD_DIAMOND_KING, CARD_VALUE_JOKER, CARD_DIAMOND_JACK, CARD_DIAMOND_10 };
        assert(isCorrect(case7, result7, PHT_ROYAL_FLUSH, PHT_COUNT));

        //缺K
        std::vector<int> case8 = { CARD_DIAMOND_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_10 };
        std::vector<int> result8 = { CARD_DIAMOND_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_10 };
        assert(isCorrect(case8, result8, PHT_ROYAL_FLUSH, PHT_COUNT));

        //缺A
        std::vector<int> case9 = { CARD_VALUE_JOKER, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_10 };
        std::vector<int> result9 = { CARD_VALUE_JOKER, CARD_DIAMOND_KING, CARD_DIAMOND_QUEEN, CARD_DIAMOND_JACK, CARD_DIAMOND_10 };
        assert(isCorrect(case9, result9, PHT_ROYAL_FLUSH, PHT_COUNT));
    }

    virtual void TestRoyalFlush7() {
    }

    virtual void TestStraightFlush5() {
        //SPADE的K+Q+J+10+9
        std::vector<int> case1 = { CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9 };
        std::vector<int> result1 = { CARD_SPADE_KING, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9 };
        assert(isCorrect(case1, result1, PHT_STRAIGHT_FLUSH, PHT_COUNT));

        //HEART的Q+J+10+9+8
        std::vector<int> case2 = { CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_9, CARD_HEART_8 };
        std::vector<int> result2 = { CARD_HEART_QUEEN, CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_9, CARD_HEART_8 };
        assert(isCorrect(case2, result2, PHT_STRAIGHT_FLUSH, PHT_COUNT));

        //CLUB的J+10+9+8+7
        std::vector<int> case3 = { CARD_CLUB_JACK, CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_7 };
        std::vector<int> result3 = { CARD_CLUB_JACK, CARD_CLUB_10, CARD_CLUB_9, CARD_CLUB_8, CARD_CLUB_7 };
        assert(isCorrect(case3, result3, PHT_STRAIGHT_FLUSH, PHT_COUNT));

        //DIAMOND的10+9+8+7+6
        std::vector<int> case4 = { CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_7, CARD_DIAMOND_6 };
        std::vector<int> result4 = { CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_7, CARD_DIAMOND_6 };
        assert(isCorrect(case4, result4, PHT_STRAIGHT_FLUSH, PHT_COUNT));

        //A作为最小值
        std::vector<int> case5 = { CARD_SPADE_5, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_SPADE_ACE };
        std::vector<int> result5 = { CARD_SPADE_5, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_SPADE_ACE };
        assert(isCorrect(case5, result5, PHT_STRAIGHT_FLUSH, PHT_COUNT));

        //Joker变为A
        std::vector<int> case6 = { CARD_SPADE_5, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2, CARD_VALUE_JOKER };
        std::vector<int> result6 = { CARD_VALUE_JOKER, CARD_SPADE_5, CARD_SPADE_4, CARD_SPADE_3, CARD_SPADE_2 };
        assert(isCorrect(case6, result6, PHT_STRAIGHT_FLUSH, PHT_COUNT));

        //Joker变为其它牌
        std::vector<int> case7 = { CARD_SPADE_9, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_VALUE_JOKER };
        std::vector<int> result7 = { CARD_VALUE_JOKER, CARD_SPADE_QUEEN, CARD_SPADE_JACK, CARD_SPADE_10, CARD_SPADE_9 };
        assert(isCorrect(case7, result7, PHT_STRAIGHT_FLUSH, PHT_COUNT));
    }

    virtual void TestStraightFlush7() {
    }

    virtual void TestFourOfAKind5() {
        //四张A
        std::vector<int> case1 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_DIAMOND_KING };
        std::vector<int> result1 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_DIAMOND_KING };
        assert(isCorrect(case1, result1, PHT_FOUR_OF_A_KIND, PHT_COUNT));

        //三张A+Joker
        std::vector<int> case2 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_KING };
        std::vector<int> result2 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_KING };
        assert(isCorrect(case2, result2, PHT_FOUR_OF_A_KIND, PHT_COUNT));

        //两张A+Joker
        std::vector<int> case3 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_2, CARD_VALUE_JOKER, CARD_DIAMOND_KING };
        std::vector<int> result3 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_KING, CARD_CLUB_2 };
        assert(isCorrect(case3, result3, PHT_THREE_OF_A_KIND, PHT_COUNT));

        //四张K和一张Joker
        std::vector<int> case4 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_DIAMOND_KING, CARD_VALUE_JOKER };
        std::vector<int> result4 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_DIAMOND_KING, CARD_VALUE_JOKER };
        assert(isCorrect(case4, result4, PHT_FOUR_OF_A_KIND, PHT_COUNT));
    }

    virtual void TestFourOfAKind7() {
    }

    virtual void TestFullHouse5() {
        //3+2：Ace
        std::vector<int> case1 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_2, CARD_CLUB_2 };
        std::vector<int> result1 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_CLUB_2, CARD_DIAMOND_2 };
        assert(isCorrect(case1, result1, PHT_FULL_HOUSE, PHT_COUNT));

        //3+2：Queen
        std::vector<int> case2 = { CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_CLUB_QUEEN, CARD_DIAMOND_2, CARD_CLUB_2 };
        std::vector<int> result2 = { CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_CLUB_QUEEN, CARD_CLUB_2, CARD_DIAMOND_2 };
        assert(isCorrect(case2, result2, PHT_FULL_HOUSE, PHT_COUNT));

        //3+A+joker
        std::vector<int> case3 = { CARD_SPADE_4, CARD_HEART_4, CARD_CLUB_4, CARD_DIAMOND_ACE, CARD_VALUE_JOKER };
        std::vector<int> result3 = { CARD_SPADE_4, CARD_HEART_4, CARD_CLUB_4, CARD_VALUE_JOKER, CARD_DIAMOND_ACE };
        assert(isCorrect(case3, result3, PHT_FULL_HOUSE, PHT_COUNT));

        //A+A+Joker+2
        std::vector<int> case4 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_2, CARD_CLUB_2 };
        std::vector<int> result4 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_CLUB_2, CARD_DIAMOND_2 };
        assert(isCorrect(case4, result4, PHT_FULL_HOUSE, PHT_COUNT));
    }

    virtual void TestFullHouse7() {
        //3+3+1: AAA+222+10
        std::vector<int> case1 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_SPADE_2, CARD_HEART_2, CARD_CLUB_2, CARD_HEART_10 };
        std::vector<int> result1 = { CARD_SPADE_2, CARD_HEART_2, CARD_CLUB_2, CARD_CLUB_ACE, CARD_HEART_10, CARD_SPADE_ACE, CARD_HEART_ACE };
        assert(isCorrect(case1, result1, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));

        //3+3+1: KKK+QQQ+10
        std::vector<int> case2 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_CLUB_QUEEN, CARD_HEART_10 };
        std::vector<int> result2 = { CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_CLUB_QUEEN, CARD_CLUB_KING, CARD_HEART_10, CARD_SPADE_KING, CARD_HEART_KING };
        assert(isCorrect(case2, result2, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));

        //3+AA+Joker+1: KKK+AA+Joker+2
        std::vector<int> case3 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_VALUE_JOKER, CARD_CLUB_2 };
        std::vector<int> result3 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_VALUE_JOKER, CARD_CLUB_2, CARD_HEART_ACE, CARD_CLUB_ACE };
        assert(isCorrect(case3, result3, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));

        //3+2+2: AAA+QQ+22
        std::vector<int> case4 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_CLUB_2, CARD_HEART_2 };
        std::vector<int> result4 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_HEART_2, CARD_CLUB_2, CARD_SPADE_QUEEN, CARD_HEART_QUEEN };
        assert(isCorrect(case4, result4, PHT_FULL_HOUSE, PHT_ONE_PAIR));

        //3+2+2: 999+33+22
        std::vector<int> case5 = { CARD_SPADE_9, CARD_HEART_9, CARD_CLUB_9, CARD_SPADE_3, CARD_HEART_3, CARD_CLUB_2, CARD_HEART_2 };
        std::vector<int> result5 = { CARD_SPADE_9, CARD_HEART_9, CARD_CLUB_9, CARD_HEART_2, CARD_CLUB_2, CARD_SPADE_3, CARD_HEART_3 };
        assert(isCorrect(case5, result5, PHT_FULL_HOUSE, PHT_ONE_PAIR));

        //3+2+A+Joker: KKK+A+Joker+22
        std::vector<int> case6 = { CARD_SPADE_KING, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_HEART_KING, CARD_CLUB_KING, CARD_HEART_2, CARD_CLUB_2 };
        std::vector<int> result6 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_HEART_2, CARD_CLUB_2, CARD_VALUE_JOKER, CARD_HEART_ACE };
        assert(isCorrect(case6, result6, PHT_FULL_HOUSE, PHT_ONE_PAIR));

        //3+2+1+1: AAA+KK+3+2
        std::vector<int> case7 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_HEART_KING, CARD_CLUB_KING, CARD_HEART_3, CARD_CLUB_2 };
        std::vector<int> result7 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_HEART_3, CARD_CLUB_2, CARD_HEART_KING, CARD_CLUB_KING };
        assert(isCorrect(case7, result7, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));

        //3+A+Joker+1+1: KKK+A+Joker+2+3
        std::vector<int> case8 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_HEART_ACE, CARD_CLUB_3, CARD_VALUE_JOKER, CARD_CLUB_2 };
        std::vector<int> result8 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_CLUB_KING, CARD_CLUB_3, CARD_CLUB_2, CARD_VALUE_JOKER, CARD_HEART_ACE };
        assert(isCorrect(case8, result8, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));

        //AA+Joker+2+2: AA+Joker+33+22
        std::vector<int> case9 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_SPADE_3, CARD_HEART_3, CARD_CLUB_2, CARD_HEART_2 };
        std::vector<int> result9 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_HEART_2, CARD_CLUB_2, CARD_SPADE_3, CARD_HEART_3 };
        assert(isCorrect(case9, result9, PHT_FULL_HOUSE, PHT_ONE_PAIR));

        //AA+Joker+2+1+1: AA+Joker+KK+3+2
        std::vector<int> case10 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_HEART_KING, CARD_CLUB_KING, CARD_HEART_3, CARD_CLUB_2 };
        std::vector<int> result10 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_HEART_3, CARD_CLUB_2, CARD_HEART_KING, CARD_CLUB_KING };
        assert(isCorrect(case10, result10, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));

        //3+2+1+1: JJJ+22+A+K
        std::vector<int> case11 = { CARD_SPADE_JACK, CARD_HEART_JACK, CARD_CLUB_JACK, CARD_HEART_2, CARD_CLUB_2, CARD_SPADE_ACE, CARD_CLUB_KING };
        std::vector<int> result11 = { CARD_SPADE_JACK, CARD_HEART_JACK, CARD_CLUB_JACK, CARD_HEART_2, CARD_CLUB_2, CARD_SPADE_ACE, CARD_CLUB_KING };
        assert(isCorrect(case11, result11, PHT_FULL_HOUSE, PHT_HIGH_CARD));

        //3+2+1+1: JJJ+22+Joker+K
        std::vector<int> case12 = { CARD_SPADE_JACK, CARD_HEART_JACK, CARD_CLUB_JACK, CARD_HEART_2, CARD_CLUB_2, CARD_VALUE_JOKER, CARD_CLUB_KING };
        std::vector<int> result12 = { CARD_SPADE_JACK, CARD_HEART_JACK, CARD_CLUB_JACK, CARD_HEART_2, CARD_CLUB_2, CARD_VALUE_JOKER, CARD_CLUB_KING };
        assert(isCorrect(case12, result12, PHT_FULL_HOUSE, PHT_HIGH_CARD));

        //3+3+1:AAA+222+9
        std::vector<int> case13 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_2, CARD_CLUB_9, CARD_CLUB_2, CARD_HEART_2 };
        std::vector<int> result13 = { CARD_HEART_2, CARD_CLUB_2, CARD_DIAMOND_2, CARD_CLUB_ACE, CARD_CLUB_9, CARD_SPADE_ACE, CARD_HEART_ACE };
        assert(isCorrect(case13, result13, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));

        //3+3+1:QQQ+222+9
        std::vector<int> case14 = { CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_CLUB_QUEEN, CARD_DIAMOND_2, CARD_CLUB_9, CARD_CLUB_2, CARD_HEART_2 };
        std::vector<int> result14 = { CARD_HEART_2, CARD_CLUB_2, CARD_DIAMOND_2, CARD_CLUB_QUEEN, CARD_CLUB_9, CARD_SPADE_QUEEN, CARD_HEART_QUEEN };
        assert(isCorrect(case14, result14, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));

        //3+2+2:QQQ+99+22
        std::vector<int> case15 = { CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_CLUB_QUEEN, CARD_DIAMOND_2, CARD_CLUB_9, CARD_CLUB_2, CARD_HEART_9 };
        std::vector<int> result15 = { CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_CLUB_QUEEN, CARD_CLUB_2, CARD_DIAMOND_2, CARD_HEART_9, CARD_CLUB_9 };
        assert(isCorrect(case15, result15, PHT_FULL_HOUSE, PHT_ONE_PAIR));

        //3+2+1+1:QQQ+99+3+2
        std::vector<int> case16 = { CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_CLUB_QUEEN, CARD_DIAMOND_3, CARD_CLUB_9, CARD_CLUB_2, CARD_HEART_9 };
        std::vector<int> result16 = { CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_CLUB_QUEEN, CARD_DIAMOND_3, CARD_CLUB_2, CARD_HEART_9, CARD_CLUB_9 };
        assert(isCorrect(case16, result16, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));

        //3+A+Joker+1+1: QQQ+A+Joker+3+2
        std::vector<int> case17 = { CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_CLUB_QUEEN, CARD_DIAMOND_3, CARD_CLUB_ACE, CARD_CLUB_2, CARD_VALUE_JOKER };
        std::vector<int> result17 = { CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_CLUB_QUEEN, CARD_DIAMOND_3, CARD_CLUB_2, CARD_VALUE_JOKER, CARD_CLUB_ACE };
        assert(isCorrect(case17, result17, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));

        //A+A+Joker+2+2: AA+Joker+99+22
        std::vector<int> case18 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_2, CARD_CLUB_9, CARD_CLUB_2, CARD_HEART_9 };
        std::vector<int> result18 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_CLUB_2, CARD_DIAMOND_2, CARD_HEART_9, CARD_CLUB_9 };
        assert(isCorrect(case18, result18, PHT_FULL_HOUSE, PHT_ONE_PAIR));

        //A+A+Joker+2+1+1: AA+Joker+22+9+8
        std::vector<int> case19 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_2, CARD_CLUB_9, CARD_CLUB_2, CARD_HEART_8 };
        std::vector<int> result19 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_CLUB_9, CARD_HEART_8, CARD_CLUB_2, CARD_DIAMOND_2 };
        assert(isCorrect(case19, result19, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));

        //普通三带二: 222+JJ+A+10
        std::vector<int> case20 = { CARD_SPADE_JACK, CARD_SPADE_ACE, CARD_HEART_JACK, CARD_CLUB_2, CARD_SPADE_2, CARD_HEART_2, CARD_HEART_10 };
        std::vector<int> result20 = { CARD_SPADE_2, CARD_HEART_2, CARD_CLUB_2, CARD_SPADE_ACE, CARD_HEART_10, CARD_SPADE_JACK, CARD_HEART_JACK };
        assert(isCorrect(case20, result20, PHT_THREE_OF_A_KIND, PHT_ONE_PAIR));
    }

    virtual void TestFlush5() {
        //SPADE
        std::vector<int> case1 = { CARD_SPADE_KING, CARD_SPADE_JACK, CARD_SPADE_3, CARD_SPADE_9, CARD_SPADE_2 };
        std::vector<int> result1 = { CARD_SPADE_KING, CARD_SPADE_JACK, CARD_SPADE_9, CARD_SPADE_3, CARD_SPADE_2 };
        assert(isCorrect(case1, result1, PHT_FLUSH, PHT_COUNT));

        //HEART
        std::vector<int> case2 = { CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_5, CARD_HEART_8, CARD_HEART_2 };
        std::vector<int> result2 = { CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_8, CARD_HEART_5, CARD_HEART_2 };
        assert(isCorrect(case2, result2, PHT_FLUSH, PHT_COUNT));

        //CLUB
        std::vector<int> case3 = { CARD_CLUB_JACK, CARD_CLUB_KING, CARD_CLUB_9, CARD_CLUB_ACE, CARD_CLUB_2 };
        std::vector<int> result3 = { CARD_CLUB_ACE, CARD_CLUB_KING, CARD_CLUB_JACK, CARD_CLUB_9, CARD_CLUB_2 };
        assert(isCorrect(case3, result3, PHT_FLUSH, PHT_COUNT));

        //DIAMOND
        std::vector<int> case4 = { CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_7, CARD_DIAMOND_2 };
        std::vector<int> result4 = { CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_7, CARD_DIAMOND_2 };
        assert(isCorrect(case4, result4, PHT_FLUSH, PHT_COUNT));

        //SPADE+Joker
        std::vector<int> case5 = { CARD_SPADE_KING, CARD_SPADE_JACK, CARD_SPADE_3, CARD_SPADE_9, CARD_VALUE_JOKER };
        std::vector<int> result5 = { CARD_VALUE_JOKER, CARD_SPADE_KING, CARD_SPADE_JACK, CARD_SPADE_9, CARD_SPADE_3 };
        assert(isCorrect(case5, result5, PHT_FLUSH, PHT_COUNT));

        //HEART+Joker
        std::vector<int> case6 = { CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_5, CARD_HEART_8, CARD_VALUE_JOKER };
        std::vector<int> result6 = { CARD_VALUE_JOKER, CARD_HEART_JACK, CARD_HEART_10, CARD_HEART_8, CARD_HEART_5 };
        assert(isCorrect(case6, result6, PHT_FLUSH, PHT_COUNT));

        //CLUB
        std::vector<int> case7 = { CARD_CLUB_JACK, CARD_CLUB_KING, CARD_CLUB_9, CARD_CLUB_ACE, CARD_VALUE_JOKER };
        std::vector<int> result7 = { CARD_VALUE_JOKER, CARD_CLUB_ACE, CARD_CLUB_KING, CARD_CLUB_JACK, CARD_CLUB_9 };
        assert(isCorrect(case7, result7, PHT_FLUSH, PHT_COUNT));

        //DIAMOND
        std::vector<int> case8 = { CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_5, CARD_VALUE_JOKER };
        std::vector<int> result8 = { CARD_VALUE_JOKER, CARD_DIAMOND_10, CARD_DIAMOND_9, CARD_DIAMOND_8, CARD_DIAMOND_5 };
        assert(isCorrect(case8, result8, PHT_FLUSH, PHT_COUNT));

        //不同花色
        std::vector<int> case9 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_CLUB_KING };
        std::vector<int> result9 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_ACE, CARD_DIAMOND_ACE, CARD_CLUB_KING };
        assert(isCorrect(case9, result9, PHT_FOUR_OF_A_KIND, PHT_COUNT));
    }

    virtual void TestFlush7() {
    }

    virtual void TestStraight5() {
        //最大顺子
        std::vector<int> case1 = { CARD_HEART_ACE, CARD_HEART_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_DIAMOND_10 };
        std::vector<int> result1 = { CARD_HEART_ACE, CARD_HEART_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_DIAMOND_10 };
        assert(isCorrect(case1, result1, PHT_STRAIGHT, PHT_COUNT));

        //最大顺子+Joker
        std::vector<int> case2 = { CARD_HEART_ACE, CARD_HEART_KING, CARD_VALUE_JOKER, CARD_CLUB_JACK, CARD_DIAMOND_10 };
        std::vector<int> result2 = { CARD_HEART_ACE, CARD_HEART_KING, CARD_VALUE_JOKER, CARD_CLUB_JACK, CARD_DIAMOND_10 };
        assert(isCorrect(case2, result2, PHT_STRAIGHT, PHT_COUNT));

        //最大顺子+Joker
        std::vector<int> case3 = { CARD_VALUE_JOKER, CARD_HEART_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_DIAMOND_10 };
        std::vector<int> result3 = { CARD_VALUE_JOKER, CARD_HEART_KING, CARD_CLUB_QUEEN, CARD_CLUB_JACK, CARD_DIAMOND_10 };
        assert(isCorrect(case3, result3, PHT_STRAIGHT, PHT_COUNT));

        //最小顺子
        std::vector<int> case4 = { CARD_CLUB_5, CARD_HEART_4, CARD_CLUB_3, CARD_CLUB_2, CARD_DIAMOND_ACE };
        std::vector<int> result4 = { CARD_CLUB_5, CARD_HEART_4, CARD_CLUB_3, CARD_CLUB_2, CARD_DIAMOND_ACE };
        assert(isCorrect(case4, result4, PHT_STRAIGHT, PHT_COUNT));

        //最小顺子+joker
        std::vector<int> case5 = { CARD_CLUB_5, CARD_HEART_4, CARD_CLUB_3, CARD_VALUE_JOKER, CARD_DIAMOND_ACE };
        std::vector<int> result5 = { CARD_CLUB_5, CARD_HEART_4, CARD_CLUB_3, CARD_VALUE_JOKER, CARD_DIAMOND_ACE };
        assert(isCorrect(case5, result5, PHT_STRAIGHT, PHT_COUNT));

        //最小顺子+joker
        std::vector<int> case6 = { CARD_CLUB_2, CARD_HEART_4, CARD_CLUB_3, CARD_VALUE_JOKER, CARD_DIAMOND_ACE };
        std::vector<int> result6 = { CARD_VALUE_JOKER, CARD_HEART_4, CARD_CLUB_3, CARD_CLUB_2, CARD_DIAMOND_ACE };
        assert(isCorrect(case6, result6, PHT_STRAIGHT, PHT_COUNT));

        //普通顺子
        std::vector<int> case7 = { CARD_CLUB_10, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6 };
        std::vector<int> result7 = { CARD_CLUB_10, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6 };
        assert(isCorrect(case7, result7, PHT_STRAIGHT, PHT_COUNT));

        //普通顺子+Joker
        std::vector<int> case8 = { CARD_CLUB_10, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_VALUE_JOKER };
        std::vector<int> result8 = { CARD_VALUE_JOKER, CARD_CLUB_10, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7 };
        assert(isCorrect(case8, result8, PHT_STRAIGHT, PHT_COUNT));

        //普通顺子+Joker
        std::vector<int> case9 = { CARD_VALUE_JOKER, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6 };
        std::vector<int> result9 = { CARD_VALUE_JOKER, CARD_HEART_9, CARD_CLUB_8, CARD_SPADE_7, CARD_DIAMOND_6 };
        assert(isCorrect(case9, result9, PHT_STRAIGHT, PHT_COUNT));

        //不是顺子
        std::vector<int> case10 = { CARD_HEART_8, CARD_HEART_3, CARD_CLUB_4, CARD_SPADE_KING, CARD_DIAMOND_QUEEN };
        std::vector<int> result10 = { CARD_SPADE_KING, CARD_DIAMOND_QUEEN, CARD_HEART_8, CARD_CLUB_4, CARD_HEART_3 };
        assert(isCorrect(case10, result10, PHT_HIGH_CARD, PHT_COUNT));
    }

    virtual void TestStraight7() {
    }

    virtual void TestThreeOfAKind5() {
        //3+1+1: AAA+3+2
        std::vector<int> case1 = { CARD_HEART_ACE, CARD_SPADE_ACE, CARD_DIAMOND_ACE, CARD_SPADE_2, CARD_CLUB_3 };
        std::vector<int> result1 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_DIAMOND_ACE, CARD_CLUB_3, CARD_SPADE_2 };
        assert(isCorrect(case1, result1, PHT_THREE_OF_A_KIND, PHT_COUNT));

        //3+1+1: 555+2+3
        std::vector<int> case2 = { CARD_HEART_5, CARD_SPADE_5, CARD_DIAMOND_5, CARD_SPADE_2, CARD_CLUB_3 };
        std::vector<int> result2 = { CARD_SPADE_5, CARD_HEART_5, CARD_DIAMOND_5, CARD_CLUB_3, CARD_SPADE_2 };
        assert(isCorrect(case2, result2, PHT_THREE_OF_A_KIND, PHT_COUNT));

        //A+A+joker+1+1
        std::vector<int> case3 = { CARD_HEART_ACE, CARD_SPADE_ACE, CARD_VALUE_JOKER, CARD_SPADE_2, CARD_CLUB_3 };
        std::vector<int> result3 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_CLUB_3, CARD_SPADE_2 };
        assert(isCorrect(case3, result3, PHT_THREE_OF_A_KIND, PHT_COUNT));
    }

    virtual void TestThreeOfAKind7() {
        //3+1+1+1+1: KKK+2+3+4+5
        std::vector<int> case1 = { CARD_HEART_KING, CARD_SPADE_KING, CARD_DIAMOND_KING, CARD_SPADE_2, CARD_CLUB_3, CARD_CLUB_4, CARD_DIAMOND_5 };
        std::vector<int> result1 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_DIAMOND_KING, CARD_CLUB_3, CARD_SPADE_2, CARD_DIAMOND_5, CARD_CLUB_4 };
        assert(isCorrect(case1, result1, PHT_THREE_OF_A_KIND, PHT_HIGH_CARD));

        //3+1+1+1+1: AAA+2+3+Q+K
        std::vector<int> case2 = { CARD_HEART_ACE, CARD_SPADE_ACE, CARD_DIAMOND_ACE, CARD_SPADE_2, CARD_CLUB_3, CARD_CLUB_QUEEN, CARD_DIAMOND_KING };
        std::vector<int> result2 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_QUEEN, CARD_CLUB_3, CARD_SPADE_2, CARD_DIAMOND_ACE, CARD_DIAMOND_KING };
        assert(isCorrect(case2, result2, PHT_ONE_PAIR, PHT_HIGH_CARD));

        //A+A+joker+2+3+Q+K
        std::vector<int> case3 = { CARD_HEART_ACE, CARD_SPADE_ACE, CARD_VALUE_JOKER, CARD_SPADE_2, CARD_CLUB_3, CARD_CLUB_QUEEN, CARD_DIAMOND_KING };
        std::vector<int> result3 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_CLUB_QUEEN, CARD_CLUB_3, CARD_SPADE_2, CARD_VALUE_JOKER, CARD_DIAMOND_KING };
        assert(isCorrect(case3, result3, PHT_ONE_PAIR, PHT_HIGH_CARD));
    }

    virtual void TestTwoPair5() {
        //2+2+1
        std::vector<int> case1 = { CARD_HEART_ACE, CARD_SPADE_ACE, CARD_DIAMOND_5, CARD_SPADE_5, CARD_CLUB_2 };
        std::vector<int> result1 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_SPADE_5, CARD_DIAMOND_5, CARD_CLUB_2 };
        assert(isCorrect(case1, result1, PHT_TWO_PAIR, PHT_COUNT));

        //2+A+joker+1
        std::vector<int> case2 = { CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_5, CARD_SPADE_5, CARD_CLUB_2 };
        std::vector<int> result2 = { CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_SPADE_5, CARD_DIAMOND_5, CARD_CLUB_2 };
        assert(isCorrect(case2, result2, PHT_TWO_PAIR, PHT_COUNT));

        //2+2+1
        std::vector<int> case3 = { CARD_HEART_10, CARD_SPADE_10, CARD_DIAMOND_5, CARD_SPADE_5, CARD_CLUB_2 };
        std::vector<int> result3 = { CARD_SPADE_10, CARD_HEART_10, CARD_SPADE_5, CARD_DIAMOND_5, CARD_CLUB_2 };
        assert(isCorrect(case3, result3, PHT_TWO_PAIR, PHT_COUNT));
    }

    virtual void TestTwoPair7() {
        //2+2+2+1: AA+55+22+8
        std::vector<int> case1 = { CARD_HEART_ACE, CARD_SPADE_ACE, CARD_DIAMOND_5, CARD_SPADE_5, CARD_CLUB_2, CARD_SPADE_2, CARD_DIAMOND_8 };
        std::vector<int> result1 = { CARD_SPADE_5, CARD_DIAMOND_5, CARD_SPADE_2, CARD_CLUB_2, CARD_DIAMOND_8, CARD_SPADE_ACE, CARD_HEART_ACE };
        assert(isCorrect(case1, result1, PHT_TWO_PAIR, PHT_ONE_PAIR));

        //2+2+2+1: 1010+55+66+A
        std::vector<int> case2 = { CARD_HEART_10, CARD_CLUB_10, CARD_DIAMOND_5, CARD_SPADE_5, CARD_CLUB_6, CARD_SPADE_6, CARD_DIAMOND_ACE };
        std::vector<int> result2 = { CARD_SPADE_6, CARD_CLUB_6, CARD_SPADE_5, CARD_DIAMOND_5, CARD_DIAMOND_ACE, CARD_HEART_10, CARD_CLUB_10 };
        assert(isCorrect(case2, result2, PHT_TWO_PAIR, PHT_ONE_PAIR));

        //2+2+A+joker+1: A+joker+55+22+8
        std::vector<int> case3 = { CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_5, CARD_SPADE_5, CARD_CLUB_2, CARD_SPADE_2, CARD_DIAMOND_8 };
        std::vector<int> result3 = { CARD_SPADE_5, CARD_DIAMOND_5, CARD_SPADE_2, CARD_CLUB_2, CARD_DIAMOND_8, CARD_HEART_ACE, CARD_VALUE_JOKER };
        assert(isCorrect(case3, result3, PHT_TWO_PAIR, PHT_ONE_PAIR));

        //2+A+joker+1+1+1
        std::vector<int> case4 = { CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_5, CARD_SPADE_5, CARD_CLUB_10, CARD_SPADE_4, CARD_DIAMOND_6 };
        std::vector<int> result4 = { CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_CLUB_10, CARD_DIAMOND_6, CARD_SPADE_4, CARD_SPADE_5, CARD_DIAMOND_5 };
        assert(isCorrect(case4, result4, PHT_ONE_PAIR, PHT_ONE_PAIR));

        /*==========================low+low==========================*/

        //2+2+1+1+1： low+low, 22+66+3+4+8
        std::vector<int> case5 = { CARD_HEART_2, CARD_SPADE_2, CARD_DIAMOND_6, CARD_SPADE_6, CARD_CLUB_3, CARD_SPADE_4, CARD_DIAMOND_8 };
        std::vector<int> result5 = { CARD_SPADE_6, CARD_DIAMOND_6, CARD_DIAMOND_8, CARD_SPADE_4, CARD_CLUB_3, CARD_SPADE_2, CARD_HEART_2 };
        assert(isCorrect(case5, result5, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //2+2+1+1+1： low+low, 33+55+K+4+6
        std::vector<int> case6 = { CARD_HEART_3, CARD_SPADE_3, CARD_DIAMOND_5, CARD_SPADE_5, CARD_CLUB_KING, CARD_SPADE_4, CARD_DIAMOND_6 };
        std::vector<int> result6 = { CARD_SPADE_5, CARD_DIAMOND_5, CARD_SPADE_3, CARD_HEART_3, CARD_SPADE_4, CARD_CLUB_KING, CARD_DIAMOND_6 };
        assert(isCorrect(case6, result6, PHT_TWO_PAIR, PHT_HIGH_CARD));

        //2+2+1+1+1： low+low, 44+55+A+7+6
        std::vector<int> case7 = { CARD_HEART_4, CARD_SPADE_4, CARD_DIAMOND_5, CARD_SPADE_5, CARD_CLUB_ACE, CARD_SPADE_7, CARD_DIAMOND_6 };
        std::vector<int> result7 = { CARD_SPADE_5, CARD_DIAMOND_5, CARD_SPADE_4, CARD_HEART_4, CARD_DIAMOND_6, CARD_CLUB_ACE, CARD_SPADE_7 };
        assert(isCorrect(case7, result7, PHT_TWO_PAIR, PHT_HIGH_CARD));

        //2+2+1+1+1： low+low, 33+22+Joker+4+6
        std::vector<int> case8 = { CARD_HEART_3, CARD_SPADE_3, CARD_DIAMOND_2, CARD_SPADE_2, CARD_VALUE_JOKER, CARD_SPADE_4, CARD_DIAMOND_6 };
        std::vector<int> result8 = { CARD_SPADE_3, CARD_HEART_3, CARD_SPADE_2, CARD_DIAMOND_2, CARD_SPADE_4, CARD_VALUE_JOKER, CARD_DIAMOND_6 };
        assert(isCorrect(case8, result8, PHT_TWO_PAIR, PHT_HIGH_CARD));

        /*==========================low+med==========================*/

        //2+2+1+1+1： low+med, 44+77+3+5+9
        std::vector<int> case9 = { CARD_HEART_4, CARD_SPADE_4, CARD_DIAMOND_7, CARD_SPADE_7, CARD_CLUB_3, CARD_SPADE_5, CARD_DIAMOND_9 };
        std::vector<int> result9 = { CARD_SPADE_7, CARD_DIAMOND_7, CARD_DIAMOND_9, CARD_SPADE_5, CARD_CLUB_3, CARD_SPADE_4, CARD_HEART_4 };
        assert(isCorrect(case9, result9, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //2+2+1+1+1： low+med, 55+1010+K+3+6
        std::vector<int> case10 = { CARD_HEART_5, CARD_SPADE_5, CARD_DIAMOND_10, CARD_SPADE_10, CARD_CLUB_3, CARD_SPADE_KING, CARD_DIAMOND_6 };
        std::vector<int> result10 = { CARD_SPADE_10, CARD_DIAMOND_10, CARD_SPADE_5, CARD_HEART_5, CARD_CLUB_3, CARD_SPADE_KING, CARD_DIAMOND_6 };
        assert(isCorrect(case10, result10, PHT_TWO_PAIR, PHT_HIGH_CARD));

        //2+2+1+1+1： low+med, 22+88+A+3+6
        std::vector<int> case11 = { CARD_SPADE_8, CARD_HEART_8, CARD_SPADE_2, CARD_DIAMOND_2, CARD_CLUB_3, CARD_SPADE_ACE, CARD_DIAMOND_6 };
        std::vector<int> result11 = { CARD_SPADE_8, CARD_HEART_8, CARD_SPADE_2, CARD_DIAMOND_2, CARD_CLUB_3, CARD_SPADE_ACE, CARD_DIAMOND_6 };
        assert(isCorrect(case11, result11, PHT_TWO_PAIR, PHT_HIGH_CARD));

        //2+2+1+1+1： low+med, 66+99+Joker+3+7
        std::vector<int> case12 = { CARD_SPADE_9, CARD_HEART_9, CARD_SPADE_6, CARD_DIAMOND_6, CARD_CLUB_3, CARD_VALUE_JOKER, CARD_DIAMOND_7 };
        std::vector<int> result12 = { CARD_SPADE_9, CARD_HEART_9, CARD_SPADE_6, CARD_DIAMOND_6, CARD_CLUB_3, CARD_VALUE_JOKER, CARD_DIAMOND_7 };
        assert(isCorrect(case12, result12, PHT_TWO_PAIR, PHT_HIGH_CARD));

        /*==========================low+hig==========================*/

        //2+2+1+1+1： low+Hig, 66+JJ+3+4+5
        std::vector<int> case13 = { CARD_HEART_6, CARD_SPADE_6, CARD_DIAMOND_JACK, CARD_SPADE_JACK, CARD_CLUB_3, CARD_SPADE_4, CARD_DIAMOND_5 };
        std::vector<int> result13 = { CARD_SPADE_JACK, CARD_DIAMOND_JACK, CARD_DIAMOND_5, CARD_SPADE_4, CARD_CLUB_3, CARD_SPADE_6, CARD_HEART_6 };
        assert(isCorrect(case13, result13, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //2+2+1+1+1： low+Hig, 55+QQ+3+4+A
        std::vector<int> case14 = { CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_SPADE_5, CARD_DIAMOND_5, CARD_CLUB_3, CARD_SPADE_4, CARD_DIAMOND_ACE };
        std::vector<int> result14 = { CARD_SPADE_QUEEN, CARD_HEART_QUEEN, CARD_SPADE_5, CARD_DIAMOND_5, CARD_CLUB_3, CARD_DIAMOND_ACE, CARD_SPADE_4 };
        assert(isCorrect(case14, result14, PHT_TWO_PAIR, PHT_HIGH_CARD));

        //2+2+1+1+1： low+Hig, 22+KK+3+4+JOKER
        std::vector<int> case15 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_SPADE_2, CARD_DIAMOND_2, CARD_CLUB_3, CARD_SPADE_4, CARD_VALUE_JOKER };
        std::vector<int> result15 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_SPADE_2, CARD_DIAMOND_2, CARD_CLUB_3, CARD_VALUE_JOKER, CARD_SPADE_4 };
        assert(isCorrect(case15, result15, PHT_TWO_PAIR, PHT_HIGH_CARD));

        /*==========================med+med==========================*/

        //2+2+1+1+1： med+med, 77+99+3+4+5
        std::vector<int> case16 = { CARD_SPADE_7, CARD_HEART_7, CARD_SPADE_9, CARD_DIAMOND_9, CARD_CLUB_3, CARD_SPADE_4, CARD_DIAMOND_5 };
        std::vector<int> result16 = { CARD_SPADE_9, CARD_DIAMOND_9, CARD_DIAMOND_5, CARD_SPADE_4, CARD_CLUB_3, CARD_SPADE_7, CARD_HEART_7 };
        assert(isCorrect(case16, result16, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //2+2+1+1+1： med+med, 88+1010+A+4+5
        std::vector<int> case17 = { CARD_SPADE_8, CARD_HEART_8, CARD_SPADE_10, CARD_DIAMOND_10, CARD_CLUB_ACE, CARD_SPADE_4, CARD_DIAMOND_5 };
        std::vector<int> result17 = { CARD_SPADE_10, CARD_DIAMOND_10, CARD_SPADE_8, CARD_HEART_8, CARD_SPADE_4, CARD_CLUB_ACE, CARD_DIAMOND_5 };
        assert(isCorrect(case17, result17, PHT_TWO_PAIR, PHT_HIGH_CARD));

        //2+2+1+1+1： med+med, 88+99+Joker+4+5
        std::vector<int> case18 = { CARD_SPADE_8, CARD_HEART_8, CARD_SPADE_9, CARD_DIAMOND_9, CARD_VALUE_JOKER, CARD_SPADE_4, CARD_DIAMOND_5 };
        std::vector<int> result18 = { CARD_SPADE_9, CARD_DIAMOND_9, CARD_SPADE_8, CARD_HEART_8, CARD_SPADE_4, CARD_VALUE_JOKER, CARD_DIAMOND_5 };
        assert(isCorrect(case18, result18, PHT_TWO_PAIR, PHT_HIGH_CARD));

        /*==========================med+hig==========================*/

        //2+2+1+1+1： med+hig, 77+JJ+3+4+5
        std::vector<int> case19 = { CARD_SPADE_7, CARD_HEART_7, CARD_SPADE_JACK, CARD_DIAMOND_JACK, CARD_CLUB_3, CARD_SPADE_4, CARD_DIAMOND_5 };
        std::vector<int> result19 = { CARD_SPADE_JACK, CARD_DIAMOND_JACK, CARD_DIAMOND_5, CARD_SPADE_4, CARD_CLUB_3, CARD_SPADE_7, CARD_HEART_7 };
        assert(isCorrect(case19, result19, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //2+2+1+1+1： med+hig, 1010+KK+3+4+5
        std::vector<int> case20 = { CARD_SPADE_10, CARD_HEART_10, CARD_SPADE_KING, CARD_DIAMOND_KING, CARD_CLUB_3, CARD_SPADE_4, CARD_DIAMOND_5 };
        std::vector<int> result20 = { CARD_SPADE_KING, CARD_DIAMOND_KING, CARD_DIAMOND_5, CARD_SPADE_4, CARD_CLUB_3, CARD_SPADE_10, CARD_HEART_10 };
        assert(isCorrect(case20, result20, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //2+2+1+1+1： med+hig, 88+QQ+3+4+A
        std::vector<int> case21 = { CARD_SPADE_8, CARD_HEART_8, CARD_SPADE_QUEEN, CARD_DIAMOND_QUEEN, CARD_CLUB_3, CARD_SPADE_4, CARD_DIAMOND_ACE };
        std::vector<int> result21 = { CARD_SPADE_QUEEN, CARD_DIAMOND_QUEEN, CARD_DIAMOND_ACE, CARD_SPADE_4, CARD_CLUB_3, CARD_SPADE_8, CARD_HEART_8 };
        assert(isCorrect(case21, result21, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //2+2+1+1+1： med+hig, 99+QQ+3+4+Joker
        std::vector<int> case22 = { CARD_SPADE_9, CARD_HEART_9, CARD_SPADE_QUEEN, CARD_DIAMOND_QUEEN, CARD_CLUB_3, CARD_SPADE_4, CARD_VALUE_JOKER };
        std::vector<int> result22 = { CARD_SPADE_QUEEN, CARD_DIAMOND_QUEEN, CARD_VALUE_JOKER, CARD_SPADE_4, CARD_CLUB_3, CARD_SPADE_9, CARD_HEART_9 };
        assert(isCorrect(case22, result22, PHT_ONE_PAIR, PHT_ONE_PAIR));

        /*==========================hig+hig==========================*/

        //2+2+1+1+1： hig+hig, JJ+QQ+3+4+5
        std::vector<int> case23 = { CARD_SPADE_JACK, CARD_HEART_JACK, CARD_SPADE_QUEEN, CARD_DIAMOND_QUEEN, CARD_CLUB_3, CARD_SPADE_4, CARD_DIAMOND_5 };
        std::vector<int> result23 = { CARD_SPADE_QUEEN, CARD_DIAMOND_QUEEN, CARD_DIAMOND_5, CARD_SPADE_4, CARD_CLUB_3, CARD_SPADE_JACK, CARD_HEART_JACK };
        assert(isCorrect(case23, result23, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //2+2+1+1+1： hig+hig, KK+QQ+3+4+A
        std::vector<int> case24 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_SPADE_QUEEN, CARD_DIAMOND_QUEEN, CARD_CLUB_3, CARD_SPADE_4, CARD_DIAMOND_ACE };
        std::vector<int> result24 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_DIAMOND_ACE, CARD_SPADE_4, CARD_CLUB_3, CARD_SPADE_QUEEN, CARD_DIAMOND_QUEEN };
        assert(isCorrect(case24, result24, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //2+2+1+1+1： hig+hig, KK+QQ+3+4+Joker
        std::vector<int> case25 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_SPADE_QUEEN, CARD_DIAMOND_QUEEN, CARD_CLUB_3, CARD_SPADE_4, CARD_VALUE_JOKER };
        std::vector<int> result25 = { CARD_SPADE_KING, CARD_HEART_KING, CARD_VALUE_JOKER, CARD_SPADE_4, CARD_CLUB_3, CARD_SPADE_QUEEN, CARD_DIAMOND_QUEEN };
        assert(isCorrect(case25, result25, PHT_ONE_PAIR, PHT_ONE_PAIR));

        /*==========================Ace+any==========================*/

        //2+2+1+1+1：AA+22+6+7+K
        std::vector<int> case26 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_SPADE_2, CARD_DIAMOND_2, CARD_CLUB_6, CARD_SPADE_7, CARD_SPADE_KING };
        std::vector<int> result26 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_SPADE_KING, CARD_SPADE_7, CARD_CLUB_6, CARD_SPADE_2, CARD_DIAMOND_2 };
        assert(isCorrect(case26, result26, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //2+2+1+1+1：AA+66+3+7+K
        std::vector<int> case27 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_SPADE_6, CARD_DIAMOND_6, CARD_CLUB_3, CARD_SPADE_7, CARD_SPADE_KING };
        std::vector<int> result27 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_SPADE_KING, CARD_SPADE_7, CARD_CLUB_3, CARD_SPADE_6, CARD_DIAMOND_6 };
        assert(isCorrect(case27, result27, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //2+2+1+1+1：AA+1010+6+7+K
        std::vector<int> case28 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_SPADE_10, CARD_DIAMOND_10, CARD_CLUB_6, CARD_SPADE_7, CARD_SPADE_KING };
        std::vector<int> result28 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_SPADE_KING, CARD_SPADE_7, CARD_CLUB_6, CARD_SPADE_10, CARD_DIAMOND_10 };
        assert(isCorrect(case28, result28, PHT_ONE_PAIR, PHT_ONE_PAIR));

        //2+2+1+1+1：AA+KK+6+7+8
        std::vector<int> case29 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_SPADE_KING, CARD_DIAMOND_KING, CARD_CLUB_6, CARD_SPADE_7, CARD_SPADE_8 };
        std::vector<int> result29 = { CARD_SPADE_ACE, CARD_HEART_ACE, CARD_SPADE_8, CARD_SPADE_7, CARD_CLUB_6, CARD_SPADE_KING, CARD_DIAMOND_KING };
        assert(isCorrect(case29, result29, PHT_ONE_PAIR, PHT_ONE_PAIR));
    }

    virtual void TestOnePair5() {
        //2+1+1+1
        std::vector<int> case1 = { CARD_HEART_ACE, CARD_SPADE_KING, CARD_DIAMOND_5, CARD_SPADE_5, CARD_CLUB_2 };
        std::vector<int> result1 = { CARD_SPADE_5, CARD_DIAMOND_5, CARD_HEART_ACE, CARD_SPADE_KING, CARD_CLUB_2 };
        assert(isCorrect(case1, result1, PHT_ONE_PAIR, PHT_COUNT));

        //A+joker+1+1+1
        std::vector<int> case2 = { CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_5, CARD_SPADE_9, CARD_CLUB_2 };
        std::vector<int> result2 = { CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_SPADE_9, CARD_DIAMOND_5, CARD_CLUB_2 };
        assert(isCorrect(case2, result2, PHT_ONE_PAIR, PHT_COUNT));
    }

    virtual void TestOnePair7() {
        //2+1+1+1+1+1
        std::vector<int> case1 = { CARD_HEART_ACE, CARD_SPADE_KING, CARD_DIAMOND_5, CARD_SPADE_5, CARD_CLUB_6, CARD_SPADE_2, CARD_DIAMOND_8 };
        std::vector<int> result1 = { CARD_SPADE_5, CARD_DIAMOND_5, CARD_DIAMOND_8, CARD_CLUB_6, CARD_SPADE_2, CARD_HEART_ACE, CARD_SPADE_KING, };
        assert(isCorrect(case1, result1, PHT_ONE_PAIR, PHT_HIGH_CARD));

        //A+joker+1+1+1+1+1
        std::vector<int> case2 = { CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_10, CARD_SPADE_5, CARD_CLUB_2, CARD_SPADE_KING, CARD_DIAMOND_8 };
        std::vector<int> result2 = { CARD_HEART_ACE, CARD_VALUE_JOKER, CARD_DIAMOND_8, CARD_SPADE_5, CARD_CLUB_2, CARD_SPADE_KING, CARD_DIAMOND_10 };
        assert(isCorrect(case2, result2, PHT_ONE_PAIR, PHT_HIGH_CARD));
    }

    virtual void TestHighCard() {
        std::vector<int> case1 = { CARD_HEART_4, CARD_SPADE_10, CARD_CLUB_7, CARD_HEART_QUEEN, CARD_DIAMOND_5, CARD_CLUB_6, CARD_HEART_ACE };
        std::vector<int> result1 = { CARD_HEART_ACE, CARD_CLUB_7, CARD_CLUB_6, CARD_DIAMOND_5, CARD_HEART_4, CARD_HEART_QUEEN, CARD_SPADE_10 };
        assert(isCorrect(case1, result1, PHT_HIGH_CARD, PHT_HIGH_CARD));
    }

protected:

    bool isCorrect(const std::vector<int> &input, const std::vector<int> &output, PokerHandsType fiveType, PokerHandsType twoType) {
        if (input.size() == DEALING_CARD_COUNT_EACH) {
            if (!m_test->parseForTip(input)) {
                return false;
            }

            if (m_test->getFiveCardsType() != fiveType) {
                return false;
            }

            if (m_test->getTwoCardsType() != twoType) {
                return false;
            }

        }
        else {
            if (m_test->parseFiveCardsType(input) != fiveType) {
                return false;
            }
        }

        return isSameResult(m_test->getResultCardArray(), output);
    }

    bool isCorrect(const std::vector<int> &input, const std::vector<int> &output, PokerHandsType sevenType) {
        if (!m_test->parseForTip(input) || m_test->getSevenCardsType() != sevenType) {
            return false;
        }

        return isSameResult(m_test->getSevenCardsArray(), output);
    }

    bool isSameResult(const std::vector<int> &parseResult, const std::vector<int> &hopeResult) {
        std::vector<int>::size_type minLength = parseResult.size() > hopeResult.size() ? hopeResult.size() : parseResult.size();
        for (std::vector<int>::size_type index = 0; index < minLength; index++) {
            if (parseResult[index] != hopeResult[index]) {
                return false;
            }
        }

        return true;
    }

protected:

    std::shared_ptr<PokerHands> m_test;
};

PAI_GOW_NS_END