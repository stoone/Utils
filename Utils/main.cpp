#include <iostream>
#include <regex>
#include "utils/Utils.h"

void test()
{
	std::string str = "789.123.456";
	std::regex reg("(.*)\.123\.456");

	std::smatch result;
	std::regex_match(str, result, reg);
	
	if (std::regex_match(str, result, reg))
	{
		std::cout << "Match:" << result.str() << std::endl;
		for (size_t i = 1; i < result.size(); i++)
		{
			std::cout << result[i] << " ";
		}
	}
	else
	{
		std::cout << "Not Match!";
	}

	std::cout << std::endl;
}

int main()
{
	test();


	system("PAUSE");

	return 0;
}