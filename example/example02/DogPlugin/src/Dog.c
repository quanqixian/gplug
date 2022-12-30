#include "Dog.h"
#include <stddef.h>

static const char * type()
{
	return "Dog";
}

static const char *weight()
{
	return "10KG";
}

void Dog_init(Dog* d)
{
	d->weightInterface.weight = weight;
	d->typeInterface.type = type;
}


void Dog_deInit(Dog* d)
{
	d->weightInterface.weight = NULL;
	d->typeInterface.type = NULL;
}

