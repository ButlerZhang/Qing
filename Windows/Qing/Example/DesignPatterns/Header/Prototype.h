// Copyright (c) ButlerZhang 2016

#ifndef __PROTOTYPE_H__
#define __PROTOTYPE_H__



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



void ProtoTypeTest();

#endif