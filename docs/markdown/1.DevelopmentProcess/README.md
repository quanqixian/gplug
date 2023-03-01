The development and usage process of the plug-in is as follows:

>Define the plugin functional interface
>        ↓
>Implement the plugin function interface
>        ↓
>Export plugin unified interface
>        ↓
>Create configuration file
>        ↓
>Use the PlugMgr interface to call the Plugin function interface



# 1.Develop plugins

## 1.1Define the set of plug-in functional interfaces

In this example, two functional interfaces are defined arbitrarily.

Define "get-type-function-interface-set":

```c++
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
```

Define "Get Weight Functional Interface Set":

```c++
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
```



## 1.2 Implement the plug-in function interface

In Dog class:

```c++
#include "Dog.h"

std::string Dog::type()
{
	return "Dog";
}
std::string Dog::weight()
{
	return "10KG";
}
```

In Cat class:

```c++
#include "Cat.h"

std::string Cat::type()
{
	return "Cat";
}

std::string Cat::weight()
{
	return "5KG";
}
```

## 1.3 Export the plug-in interface according to the plugin specification

```c++
#include "Dog.h"
#include "Debug.h"
#include "gplugin.h"
#include <string.h>

int GPLUGIN_API DogPlugin_init()
{
	GPLUG_LOG_INFO("DogPlugin_init");
	return GPLUGIN_OK;
}

int GPLUGIN_API DogPlugin_Uninit()
{
	GPLUG_LOG_INFO("DogPlugin_Uninit");
	return GPLUGIN_OK;
}

int GPLUGIN_API DogPlugin_CreateInstance(GPluginHandle * instance)
{
	*instance = (GPluginHandle) (new Dog());
	return GPLUGIN_OK;
}

int GPLUGIN_API DogPlugin_DestroyInstance(GPluginHandle instance)
{
	delete (Dog*) instance;
	return GPLUGIN_OK;
}

int GPLUGIN_API DogPlugin_QueryInterface(GPluginHandle instance, const char * ikey, GPluginHandle * interface)
{
	int ret = GPLUGIN_OK;

	if(0 == strcmp(ikey, IKEY_IType))
	{
		*interface = (GPluginHandle)(dynamic_cast<IGetTypeInterface*>((Dog*)instance));
	}
	else if(0 == strcmp(ikey, IKEY_IWeight))
	{
		*interface = (GPluginHandle)(dynamic_cast<IGetWeightInterface*>((Dog*)instance));
	}
	else
	{
		*interface = GPLUGIN_INVALID_HANDLE;
		ret = GPLUGIN_NOTSUPPORT;
	}

	return ret;
}

const char** GPLUGIN_API DogPlugin_GetAllInterfaceIkeys()
{
	static const char * ikeys[] = {
		IKEY_IType,
		IKEY_IWeight,
		NULL
	};

	return  ikeys;
}

const char* GPLUGIN_API DogPlugin_GetFileVersion()
{
	return "V1.0.0.0";
}

GPLUGIN_MAKE_EXPORT_INTERFACE(DogPlugin_init, DogPlugin_Uninit, DogPlugin_CreateInstance,
		DogPlugin_DestroyInstance, DogPlugin_QueryInterface,
		DogPlugin_GetAllInterfaceIkeys, DogPlugin_GetFileVersion);

```



# 2.Create configuration file

```xml
<?xml version="1.0" encoding="utf-8"?>
<gplug>
	<plugin fkey="DogPlugin" file="../../DogPlugin/bin/libDogPlugin.so" delayload="true" />
	<plugin fkey="CatPlugin" file="../../CatPlugin/bin/libCatPlugin.so" delayload="false" />
</gplug>
```



# 3.Use the PlugMgr interface to call the Plugin function interface

```c++
#include "Debug.h"
#include "gplugMgr.h"
#include "GetTypeInterface.h"
#include "GetWeightInterface.h"

int main(int argc, const char *argv[])
{
    int ret = 0;

    GPlugMgr_Init();

    char** fkeys = NULL;
    unsigned int fkeysCount = 0;
    ret = GPlugMgr_QueryAllFkeys(&fkeys, &fkeysCount);
    if(0 != ret)
    {
        GPLUGMGR_LOG_ERROR(0, "GPlugMgr_QueryAllFkeys error");
        return ret;
    }
    GPLUGMGR_LOG_INFO("fkeysCount=%d", fkeysCount);
    for(int i = 0; i < fkeysCount; i++)
    {
        GPLUGMGR_LOG_INFO("fkeys[%d]=%s", i, fkeys[i]);
        GPluginHandle instance = NULL;
        int errCode = 0;
        /* Create plugin instance */
        ret = GPlugMgr_CreateInstance(fkeys[i], &instance, &errCode);
        if(0 != ret)
        {
            GPLUGMGR_LOG_ERROR(errCode, "GPlugMgr_CreateInstance error");
            break;
        }
        /* Query whether the plugin instance has "get name function interface set" */
        {
            GPluginHandle interface = GPLUGIN_INVALID_HANDLE;
            ret = GPlugMgr_QueryInterface(instance, IKEY_IType, &interface, &errCode);
            if(0 != ret)
            {
                GPLUGMGR_LOG_ERROR(errCode, "GPlugMgr_QueryInterface error, ikey=%s", IKEY_IType);
                return ret;
            }

            /* Call the "Get type Interface" */
            IGetTypeInterface * p = (IGetTypeInterface*) interface;
            GPLUGMGR_LOG_INFO("fkey=%s, type=%s", fkeys[i], p->type().c_str());
        }

        /* Query whether the plugin instance has "get weight function interface set"*/
        {
            GPluginHandle interface = GPLUGIN_INVALID_HANDLE;
            ret = GPlugMgr_QueryInterface(instance, IKEY_IWeight, &interface, &errCode);
            if(0 != ret)
            {
                GPLUGMGR_LOG_ERROR(errCode, "GPlugMgr_QueryInterface error, ikey=%s", IKEY_IWeight);
                return ret;
            }

            /* Call "Get Weight Interface" */
            IGetWeightInterface * p = (IGetWeightInterface*) interface;
            GPLUGMGR_LOG_INFO("fkey=%s, weight=%s", fkeys[i], p->weight().c_str());
        }

		/* Query plugin attributes */
		{
			char attrBuf[256] = {0};
			unsigned int attrLen = sizeof(attrBuf);
			ret = GPlugMgr_QueryConfigAttribute(fkeys[i], "file", attrBuf, &attrLen);
			if(0 != ret)
			{
				GPLUGMGR_LOG_ERROR(ret, "GPlugMgr_QueryConfigAttribute error");
				return ret;
			}
			GPLUGMGR_LOG_INFO("file=%s", attrBuf);
		}

        ret = GPlugMgr_DestroyInstance(instance, &errCode);
        if(0 != ret)
        {
            GPLUGMGR_LOG_ERROR(0, "GPlugMgr_DestroyInstance error");
            break;
        }
    }

    GPlugMgr_ReleaseAllFkeys(fkeys, fkeysCount);

    GPlugMgr_Deinit();
    return 0;
}

```


