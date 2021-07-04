#include "01Strategy/Duck.h"
#include "02Observer/DisplayElement.h"
#include "03Decorator/CoffeeComponent.h"
#include "04Factory/02FactoryMethod/PizzaStore.h"
#include "09TemplateMethod/CaffeineBeverage.h"
#include "10Iterator/Waitress.h"
#include "11Composite/CWaitress.h"
#include "12State/GumballMachine.h"
#include "99Compound/DuckSimulator.h"
#include <Windows.h>



int main()
{
    //AbstractFactoryTest();
    //TestStrategy();
    //TestObserver();
    //TestDecorator();
    //TestFactoryMethod();
    //TestTemplateMethod();
    //TestIterator();
    //TestComposite();
    //TestState();
    DuckSimulator::test();
    system("pause");
    return 0;
}
