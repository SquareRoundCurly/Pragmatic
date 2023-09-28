#pragma once

// Standard library
#include <string>

class SomeClass
{
	public: // Methods
	void SetData(const std::string& data);
	void Print();

	private:
	std::string data;
};