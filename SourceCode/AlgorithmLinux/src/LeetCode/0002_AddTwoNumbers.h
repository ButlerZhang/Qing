#pragma once
#include "LeetCodeCommon.h"

LEET_CODE_NS_BEGIN

class AddTwoNumbers {
public:

    //Definition for singly-linked list.
    struct ListNode {
        int val;
        ListNode *next;
        ListNode() : val(0), next(nullptr) {}
        ListNode(int x) : val(x), next(nullptr) {}
        ListNode(int x, ListNode *next) : val(x), next(next) {}
    };

    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode *resultHead = nullptr, *resultMove = nullptr;

        int carry = 0, base = 10;
        int value1 = 0, value2 = 0, resultValue = 0;

        ListNode *move1 = l1, *move2 = l2;
        while (move1 != nullptr || move2 != nullptr) {
            value1 = 0;
            if (move1 != nullptr) {
                value1 = move1->val;
                move1 = move1->next;
            }

            value2 = 0;
            if (move2 != nullptr) {
                value2 = move2->val;
                move2 = move2->next;
            }

            resultValue = value1 + value2 + carry;
            if (resultValue >= base) {
                resultValue -= base;
                carry = 1;
            }
            else {
                carry = 0;
            }

            ListNode *newNode = new ListNode(resultValue);
            if (resultHead == nullptr) {
                resultHead = newNode;
                resultMove = resultHead;
            }
            else {
                resultMove->next = newNode;
                resultMove = resultMove->next;
            }
        }

        if (carry != 0) {
            resultMove->next = new ListNode(carry);
        }

        return resultHead;
    }

    static void testCase() {
        AddTwoNumbers test;

        //l1 = [2,4,3], l2 = [5,6,4], out = [7,0,8]
        {
            ListNode *node1_2 = new ListNode(2);
            ListNode *node1_4 = new ListNode(4);
            ListNode *node1_3 = new ListNode(3);
            node1_2->next = node1_4;
            node1_4->next = node1_3;
            ListNode *l1 = node1_2;

            ListNode *node2_5 = new ListNode(5);
            ListNode *node2_6 = new ListNode(6);
            ListNode *node2_4 = new ListNode(4);
            node2_5->next = node2_6;
            node2_6->next = node2_4;
            ListNode *l2 = node2_5;

            ListNode *result = test.addTwoNumbers(l1, l2);
            assert(result->val == 7);
            assert(result->next->val == 0);
            assert(result->next->next->val == 8);
        }

        //l1 = [0], l2 = [0], out = [0]
        {
            ListNode *l1 = new ListNode(0);
            ListNode *l2 = new ListNode(0);
            ListNode *result = test.addTwoNumbers(l1, l2);
            assert(result->val == 0 && result->next == nullptr);
        }

        //l1 = [9,9,9,9,9,9,9], l2 = [9,9,9,9], out = [8,9,9,9,0,0,0,1]
        {
            ListNode *l1 = new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9)))))));
            ListNode *l2 = new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9))));

            ListNode *result = test.addTwoNumbers(l1, l2);
            assert(result->val == 8);
            result = result->next;

            assert(result->val == 9);
            result = result->next;

            assert(result->val == 9);
            result = result->next;

            assert(result->val == 9);
            result = result->next;

            assert(result->val == 0);
            result = result->next;

            assert(result->val == 0);
            result = result->next;

            assert(result->val == 0);
            result = result->next;

            assert(result->val == 1 && result->next == nullptr);
        }

        //l1 = nullptr, l2 = [1,2], out = [1,2]
        {
            ListNode *l1 = nullptr;
            ListNode *l2 = new ListNode(1, new ListNode(2));
            ListNode *result = test.addTwoNumbers(l1, l2);
            assert(result->val == 1 && result->next->val == 2 && result->next->next == nullptr);
        }

        //l1 = [1,2], l2 = nullptr, out = [1,2]
        {
            ListNode *l1 = new ListNode(1, new ListNode(2));
            ListNode *l2 = nullptr;
            ListNode *result = test.addTwoNumbers(l1, l2);
            assert(result->val == 1 && result->next->val == 2 && result->next->next == nullptr);
        }

        //l1 = nullptr, l2 = nullptr, out = nullptr
        {
            ListNode *result = test.addTwoNumbers(nullptr, nullptr);
            assert(result == nullptr);
        }
    }
};

LEET_CODE_NS_END