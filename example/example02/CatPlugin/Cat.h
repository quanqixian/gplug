#ifndef _CAT_TYPE_H_
#define _CAT_TYPE_H_

#include "GetTypeInterface.h"
#include "GetWeightInterface.h"



typedef struct
{
	IGetWeightInterface weightInterface;
	IGetTypeInterface typeInterface;
}Cat;

void Cat_init(Cat* d);
void Cat_deInit(Cat* d);
#endif
