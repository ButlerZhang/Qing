#pragma once
#include "CDuck.h"
#include "CGoose.h"
#include "CFactory.h"

//Ѽ��ģ����
class DuckSimulator {
public:

    static void test() {
        DuckSimulator simulator;
        AbstractDuckFactory *duckFactory = new CountingDuckFactory();
        simulator.simulate(duckFactory);
    }

    void simulate(AbstractDuckFactory *duckFactory) {
        Quackable *mallarDuck = duckFactory->createMallardDuck();
        Quackable *redheadDuck = duckFactory->createRedHeadDuck();
        Quackable *duckCall = duckFactory->createDuckCall();
        Quackable *rubberDuck = duckFactory->createRubberDuck();

        Quackable *gooseDuck = new CGooseAdapter(new CGoose());

        std::cout << "Duck Simulator" << std::endl << std::endl;

        //��Ⱥ
        Flock *flockOfDucks = new Flock();
        flockOfDucks->add(mallarDuck);
        flockOfDucks->add(redheadDuck);
        flockOfDucks->add(duckCall);
        flockOfDucks->add(rubberDuck);

        //һȺ��ͷѼ
        Flock *flockOfMallards = new Flock();
        Quackable *mallarDuck1 = duckFactory->createMallardDuck();
        Quackable *mallarDuck2 = duckFactory->createMallardDuck();
        Quackable *mallarDuck3 = duckFactory->createMallardDuck();
        Quackable *mallarDuck4 = duckFactory->createMallardDuck();
        flockOfMallards->add(mallarDuck1);
        flockOfMallards->add(mallarDuck2);
        flockOfMallards->add(mallarDuck3);
        flockOfMallards->add(mallarDuck4);

        //�ٰ�һȺ��ͷѼ�Ž���Ⱥ
        flockOfDucks->add(flockOfMallards);

        std::cout << "Whole Flock Simulation:" << std::endl;
        simulate(flockOfDucks);

        std::cout << std::endl << "Mallard Flocks Simulation:" << std::endl << std::endl;
        simulate(flockOfMallards);

        std::cout << std::endl << "Observer Simulation:" << std::endl;
        Quackologist *quackologist = new Quackologist();
        flockOfDucks->registerObserver(quackologist);
        simulate(flockOfDucks);

        std::cout << std::endl << "The ducks quacked "
            << QuackCounter::getNumberOfQuacks()
            << " times." << std::endl;
    }

    void simulate(Quackable *duck) {
        duck->quack();
    }
};
