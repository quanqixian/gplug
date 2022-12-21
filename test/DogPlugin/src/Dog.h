#ifndef _DOG_TYPE_H_
#define _DOG_TYPE_H_

#include "GetTypeInterface.h"
#include "GetWeightInterface.h"

class Dog : public IGetTypeInterface,
            public IGetWeightInterface
{
public:
	virtual std::string type();
	virtual std::string weight();
};

#endif
