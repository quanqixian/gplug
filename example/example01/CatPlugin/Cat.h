#ifndef _CAT_TYPE_H_
#define _CAT_TYPE_H_

#include "GetTypeInterface.h"
#include "GetWeightInterface.h"

class Cat : public IGetTypeInterface,
            public IGetWeightInterface
{
public:
	virtual std::string type();
	virtual std::string weight();
};

#endif
