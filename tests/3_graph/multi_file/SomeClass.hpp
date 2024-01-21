#pragma once

namespace SomeNamespace
{
    class SomeClass
    {
        public:
        unsigned int GetMeaningOfLife();
        void SetMeaningOfLife(unsigned int meaningOfLife);

        private:
        unsigned int meaningOfLife = 42;
    };
} // namespace SomeNamespace