#ifndef _GET_TYPE_INTERFACE_H_
#define _GET_TYPE_INTERFACE_H_

#include <string>

#define IKEY_IType "{aaaa}"

class IGetTypeInterface
{
public:
    virtual std::string type() = 0;
public:
    virtual ~IGetTypeInterface() {}
};

#endif
