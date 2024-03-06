// Standard library
#include <iostream>

// Project
#include "SomeClass.hpp"

int main()
{
    SomeNamespace::SomeClass sc;
    std::cout << sc.GetMeaningOfLife() << std::endl;

    return 0;
}