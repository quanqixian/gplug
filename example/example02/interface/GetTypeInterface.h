#ifndef _GET_TYPE_INTERFACE_H_
#define _GET_TYPE_INTERFACE_H_

#define IKEY_IType "{aaaa}"

typedef struct 
{
    const char * (*type)(void);
}IGetTypeInterface;

#endif
