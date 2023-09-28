// Source header
#include "SomeClass.hpp"

// Standard library
#include <iostream>

void SomeClass::SetData(const std::string& data)
{
	this->data = data;
}

void SomeClass::Print()
{
	std::cout << data << std::endl;
}