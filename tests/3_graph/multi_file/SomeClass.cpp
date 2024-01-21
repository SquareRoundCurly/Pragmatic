#include "SomeClass.hpp"

namespace SomeNamespace
{
    unsigned int SomeClass::GetMeaningOfLife()
    {
        return meaningOfLife;
    }

    void SomeClass::SetMeaningOfLife(unsigned int meaningOfLife)
    {
        this->meaningOfLife = meaningOfLife;
    }
} // namespace SomeNamespace

