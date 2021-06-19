#pragma once
#include "HouseWay/Bally/BallyUnitTest.h"
#include "HouseWay/Barona/BaronaUnitTest.h"
#include "HouseWay/BritishColumbia/BritishColumbiaUnitTest.h"
#include "HouseWay/CanterburyPark/CanterburyParkUnitTest.h"
#include "HouseWay/Claridge/ClaridgeUnitTest.h"
#include "HouseWay/FlamingoHilton/FlamingoHiltonUnitTest.h"
#include "HouseWay/FourQueens/FourQueensUnitTest.h"
#include "HouseWay/GoldenNugget/GoldenNuggetUnitTest.h"
#include "HouseWay/SantaAnaStar/SantaAnaStarUnitTest.h"
#include "HouseWay/TrumpPlaza/TrumpPlazaUnitTest.h"

PAI_GOW_NS_BEGIN

class HouseWayFactory {
public:

    enum HouseWayType {
        HW_TRUMP_PLAZA,
        HW_CLARIDGE,
        HW_BALLY,
        HW_BARONA,
        HW_BRITISH_COLUMBIA,
        HW_CANTERBURY_PARK,
        HW_FOUR_QUEENS,
        HW_FLAMINGO_HILTON,
        HW_GOLDEN_NUGGET,
        HW_TEXAS_STATION,
        HW_SANTA_ANA_STAR,
        HW_COUNT
    };

    std::shared_ptr<PokerHands> getPokerHands(HouseWayType houseWay) {
        switch (houseWay) {
        case HW_TRUMP_PLAZA:
            return std::make_shared<TrumpPlazaPokerHands>();
        case HW_CLARIDGE:
            return std::make_shared<ClaridgePokerHands>();
        case HW_FOUR_QUEENS:
        case HW_TEXAS_STATION:
            return std::make_shared<FourQueensPokerHands>();
        case HW_BALLY:
            return std::make_shared<BallyPokerHands>();
        case HW_BARONA:
            return std::make_shared<BaronaPokerHands>();
        case HW_BRITISH_COLUMBIA:
            return std::make_shared<BritishColumbiaPokerHands>();
        case HW_CANTERBURY_PARK:
            return std::make_shared<CanterburyParkPokerHands>();
        case HW_FLAMINGO_HILTON:
            return std::make_shared<FlamingoHiltonPokerHands>();
        case HW_GOLDEN_NUGGET:
            return std::make_shared<GoldenNuggetPokerHands>();
        case HW_SANTA_ANA_STAR:
            return std::make_shared<SantaAnaStarPokerHands>();
        default:
            return nullptr;
        }
    }

    void unitTest() {
        BallyUnitTest bally;
        bally.Test();

        BaronaUnitTest barona;
        barona.Test();

        BritishColumbiaUnitTest britishColumbia;
        britishColumbia.Test();

        CanterburyParkUnitTest canterburyPark;
        canterburyPark.Test();

        ClaridgeUnitTest claridge;
        claridge.Test();

        FlamingoHiltonUnitTest flamingoHilton;
        flamingoHilton.Test();

        FourQueensUnitTest fourQueens;
        fourQueens.Test();

        GoldenNuggetUnitTest goldenNugget;
        goldenNugget.Test();

        SantaAnaStarUnitTest santaAnaStar;
        santaAnaStar.Test();

        TrumpPlazaUnitTest trumpPlaza;
        trumpPlaza.Test();
    }
};

PAI_GOW_NS_END