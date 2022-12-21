#ifndef _GET_WEIGHT_INTERFACE_H_
#define _GET_WEIGHT_INTERFACE_H_

#include <string>

#define IKEY_IWeight "{bbbb}"

class IGetWeightInterface
{
public:
    virtual std::string weight() = 0;
public:
    virtual ~IGetWeightInterface() {}
};

#endif
