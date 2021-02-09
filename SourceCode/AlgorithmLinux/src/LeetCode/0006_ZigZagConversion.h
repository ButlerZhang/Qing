#pragma once
#include <string>
#include <vector>
#include "LeetCodeCommon.h"

LEET_CODE_NS_BEGIN

class ZigZagConversion {
public:

    string convert(string s, int numRows) {
        return bruteForceSearch(s, numRows);
    }

    string bruteForceSearch(string s, int numRows) {
        if (numRows <= 1 || s.size() <= numRows) {
            return s;
        }

        bool isDown = true;
        int row = 0, column = 0;
        std::vector<std::vector<char>> zigZag(numRows, std::vector<char>(s.size()));

        for (int charIndex = 0; charIndex < s.size(); charIndex++) {
            zigZag[row][column] = s[charIndex];

            if (isDown) {
                ++row;
                if (row == numRows) {
                    isDown = false;
                    row -= 2;
                    ++column;
                }
            }
            else {
                --row;
                ++column;
                if (row < 0) {
                    isDown = true;
                    row += 2;
                    --column;
                }
            }
        }

        std::string result;
        for (int r = 0; r < numRows; r++) {
            for (int c = 0; c <= column; c++) {
                if (zigZag[r][c] != '\0') {
                    result += zigZag[r][c];
                }
            }
        }

        return result;
    }

    static void testCase() {
        ZigZagConversion test;

        assert(test.convert("PAYPALISHIRING", 3) == "PAHNAPLSIIGYIR");
        assert(test.convert("PAYPALISHIRING", 4) == "PINALSIGYAHRPI");
        assert(test.convert("A", 1) == "A");

        assert(test.convert("A", 2) == "A");
        assert(test.convert("Aa", 2) == "Aa");
        assert(test.convert("Aax", 2) == "Axa");
        assert(test.convert("Aaxx", 2) == "Axax");
        assert(test.convert("Aaxx", 3) == "Aaxx");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 1) == "ABCDEFGHIJ,.KLMN");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 2) == "ACEGI,KMBDFHJ.LN");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 3) == "AEIKBDFHJ.LNCG,M");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 4) == "AGKBFH.LCEI,MDJN");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 5) == "AIBHJNCG,MDF.LEK");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 6) == "A,BJ.CIKDHLEGMFN");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 7) == "AKB.LC,MDJNEIFHG");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 8) == "AMBLNCKD.E,FJGIH");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 9) == "ABNCMDLEKF.G,HJI");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 10) == "ABCDNEMFLGKH.I,J");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 11) == "ABCDEFNGMHLIKJ.,");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 12) == "ABCDEFGHNIMJL,K.");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 13) == "ABCDEFGHIJN,M.LK");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 14) == "ABCDEFGHIJ,.NKML");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 15) == "ABCDEFGHIJ,.KLNM");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 16) == "ABCDEFGHIJ,.KLMN");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 17) == "ABCDEFGHIJ,.KLMN");
        assert(test.convert("ABCDEFGHIJ,.KLMN", 99) == "ABCDEFGHIJ,.KLMN");
    }
};

LEET_CODE_NS_END