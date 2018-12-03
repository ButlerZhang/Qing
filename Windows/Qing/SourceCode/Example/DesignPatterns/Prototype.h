#pragma once
#include "..\..\QingBase.h"

QING_EXAMPLE_BEGIN



class Prototype
{
public:

    Prototype(void) {}
    virtual ~Prototype(void) {}

    virtual Prototype* Clone() const = 0;
};

class PrototypeA : public Prototype
{
public:

    PrototypeA(void);
    PrototypeA(const PrototypeA&);
    virtual ~PrototypeA(void);

    virtual Prototype* Clone() const;
};

class PrototypeB : public Prototype
{
public:

    PrototypeB(void);
    PrototypeB(const PrototypeB&);
    virtual ~PrototypeB(void);

    virtual Prototype* Clone() const;
};



void QING_DLL ProtoTypeTest();

QING_EXAMPLE_END
