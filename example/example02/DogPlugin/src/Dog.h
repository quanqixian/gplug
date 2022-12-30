#ifndef _DOG_TYPE_H_
#define _DOG_TYPE_H_

#include "GetTypeInterface.h"
#include "GetWeightInterface.h"



typedef struct
{
	IGetWeightInterface weightInterface;
	IGetTypeInterface typeInterface;
}Dog;

void Dog_init(Dog* d);
void Dog_deInit(Dog* d);
#endif
