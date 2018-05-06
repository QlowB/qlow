#include "../Visitor.h"
#include <iostream>

using namespace qlow;


class Animal;
class Pig;
class Cow;
class Horse;


class AnimalVisitor : public Visitor<void, Pig, Cow, Horse>
//    public Visitor<
//        // visitable types
//        Pig, Cow, Horse
//    >
{
public:

    void visit(Pig& p) override
    {
        std::cout << "Hey Piggy" << std::endl;
    }

    void visit(Cow& p) override
    {
        std::cout << "Hay Cow" << std::endl;
    }

    void visit(Horse& p) override
    {
        std::cout << "Hiy Ross" << std::endl;
    }
};


class Animal : public Visitable<AnimalVisitor>
{
};


class Pig : public Animal
{
public:
    inline void accept(AnimalVisitor& av) final
    {
        return av.visit(*this);
    }
};


class Cow : public Animal
{
public:
    inline void accept(AnimalVisitor& av) final
    {
        return av.visit(*this);
    }
};


class Horse : public Animal
{
public:
    inline void accept(AnimalVisitor& av) final
    {
        return av.visit(*this);
    }
};



int main()
{
    Animal* animal = new Pig();
    AnimalVisitor av;

    animal->accept(av);
    return 0;
}





