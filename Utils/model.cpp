#include "model.h"
#include <iostream>


model::model()
{
	std::cout << "create" << std::endl;
}


model::~model()
{
	std::cout << "decreate" << std::endl;
}

model* model::getInstance()
{
	static model m;
	return &m;
}

void model::print(int n)
{
	printf("this is %d\r\n", n);
}
