#include "Cat.h"
#include <stddef.h>

static const char * type()
{
	return "Cat";
}

static const char *weight()
{
	return "5KG";
}

void Cat_init(Cat* d)
{
	d->weightInterface.weight = weight;
	d->typeInterface.type = type;
}


void Cat_deInit(Cat* d)
{
	d->weightInterface.weight = NULL;
	d->typeInterface.type = NULL;
}

