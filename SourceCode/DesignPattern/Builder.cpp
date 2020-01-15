#include "Builder.h"
#include <iostream>



void BuilderA::BuilderPartOne()
{
    std::cout << "BuilderPartOne by BuilderA." << std::endl;
}

void BuilderA::BuilderPartTwo()
{
    std::cout << "BuilderPartTwo by BuilderA." << std::endl;
}



void BuilderB::BuilderPartOne()
{
    std::cout << "BuilderPartOne by BuilderB." << std::endl;
}

void BuilderB::BuilderPartTwo()
{
    std::cout << "BuilderPartTwo by BuilderB." << std::endl;
}



Director::Director(Builder *pBuilder) : m_pBuilder(pBuilder)
{
}

Director::~Director()
{
    delete m_pBuilder;
    m_pBuilder = NULL;
}

void Director::Construct()
{
    //constrcut order
    m_pBuilder->BuilderPartOne();
    m_pBuilder->BuilderPartTwo();
}



void BuilderTest()
{
    Builder *pBuilder1 = new BuilderA();
    Director *pDirector1 = new Director(pBuilder1);
    pDirector1->Construct();

    Builder *pBuilder2 = new BuilderB();
    Director *pDirector2 = new Director(pBuilder2);
    pDirector2->Construct();

    delete pDirector1;
    delete pDirector2;
}
