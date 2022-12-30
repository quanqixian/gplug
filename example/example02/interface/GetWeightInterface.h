#ifndef _GET_WEIGHT_INTERFACE_H_
#define _GET_WEIGHT_INTERFACE_H_

#define IKEY_IWeight "{bbbb}"

typedef struct
{
    const char * (*weight)();
}IGetWeightInterface;

#endif
