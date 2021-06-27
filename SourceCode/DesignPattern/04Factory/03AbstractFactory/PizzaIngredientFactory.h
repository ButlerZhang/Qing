#pragma once
#include <vector>

class Dough;
class Sauce;
class Cheese;
class Veggies;
class Pepperoni;
class Clams;

//≈˚»¯≈‰¡œπ§≥ß
class PizzaIngredientFactory {
public:

    virtual Dough* createDough() = 0;
    virtual Sauce* createSauce() = 0;
    virtual Clams* createClams() = 0;
    virtual Cheese* createCheese() = 0;
    virtual Pepperoni* createPepperoni() = 0;
    virtual std::vector<Veggies*> createVeggies() = 0;
};

class NYPizzaIngredientFactory : public PizzaIngredientFactory {
public:

    virtual Dough* createDough() {
        return nullptr;// new ThinCrustDough();
    }

    virtual Sauce* createSauce() {
        return nullptr;// new MarinaraSauce();
    }

    virtual Cheese* createCheese() {
        return nullptr;// new ReggianoCheese();
    }

    virtual std::vector<Veggies*> createVeggies() {
        return { nullptr };//{ new Garlic(), new Onion(), new Mushroom(), new RedPepper() };
    }

    virtual Pepperoni* createPepperoni() {
        return nullptr;//new SlicedPepperoni();
    }

    virtual Clams* createClams() {
        return nullptr;//new FreshClams();
    }
};

class ChicagoPizzaIngredientFactory : public PizzaIngredientFactory {
public:

    virtual Dough* createDough() {
        return nullptr;//new ThickCrustDough();
    }

    virtual Sauce* createSauce() {
        return nullptr;//new PlumTomatoSauce();
    }

    virtual Cheese* createCheese() {
        return nullptr;//new ReggianoCheese();
    }

    virtual std::vector<Veggies*> createVeggies() {
        return { nullptr };//{ new Garlic(), new Onion(), new Mushroom(), new RedPepper() };
    }

    virtual Pepperoni* createPepperoni() {
        return nullptr;//new SllcedPepperoni();
    }

    virtual Clams* createClams() {
        return nullptr;//new FrozenClams();
    }
};
