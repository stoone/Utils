#include "Single.h"



Single::Single()
{
}


Single::~Single()
{
}

Single* Single::getInstance()
{
	static Single object;
	return &object;
}
