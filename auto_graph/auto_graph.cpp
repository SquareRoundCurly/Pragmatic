#include <iostream>

#include "instrument.hpp"

int main()
{
	PROFILE_BEGIN_SESSION("profile", "profile.json");
	PROFILE_FUNCTION();

	std::cout << "Hello world !" << std::endl;

	PROFILE_END_SESSION();
	return 0;
}