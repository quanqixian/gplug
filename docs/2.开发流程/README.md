插件的开发和使用流程如下图所示：

![process](../pic/process.png)



# 1.开发插件

## 1.1定义插件功能接口集

随意定义两种功能接口。

定义“[获取类型功能接口集](../../example/example01/interface/GetTypeInterface.h)”：

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

定义“[获取重量功能接口集](../../example/example01/interface/GetWeightInterface.h)”：

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



## 1.2 实现插件功能接口

Dog类：

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

Cat类：

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

## 1.3按照插件规范导出插件接口

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



# 2.编写配置文件

```xml
<?xml version="1.0" encoding="utf-8"?>
<gplug>
	<plugin fkey="DogPlugin" file="../../DogPlugin/bin/libDogPlugin.so" delayload="true" />
	<plugin fkey="CatPlugin" file="../../CatPlugin/bin/libCatPlugin.so" delayload="false" />
</gplug>
```



# 3.调用插件管理接口

```c++
#include "Debug.h"
#include "gplugMgr.h"
#include "GetTypeInterface.h"
#include "GetWeightInterface.h"

int main(int argc, const char *argv[])
{
    int ret = 0;

    GPLUGMGR_Init();

    char** fkeys = NULL;
    unsigned int fkeysCout = 0;
    ret = GPLUGMGR_QueryAllFkeys(&fkeys, &fkeysCout);
    if(0 != ret)
    {
        GPLUGMGR_LOG_ERROR(0, "GPLUGMGR_QueryAllFkeys error");
        return ret;
    }
    GPLUGMGR_LOG_INFO("fkeysCout=%d", fkeysCout);
    for(int i = 0; i < fkeysCout; i++)
    {
        GPLUGMGR_LOG_INFO("fkeys[%d]=%s", i, fkeys[i]);
        GPluginHandle instance = NULL;
        int errCode = 0;
        /* 创建插件实例 */
        ret = GPLUGMGR_CreateInstance(fkeys[i], &instance, &errCode);
        if(0 != ret)
        {
            GPLUGMGR_LOG_ERROR(errCode, "GPLUGMGR_CreateInstance error");
            break;
        }
        /*查询插件实例是否具有获取名字功能接口集 */
        {
            GPluginHandle interface = GPLUGIN_INVALID_HANDLE;
            ret = GPLUGMGR_QueryInterface(instance, IKEY_IType, &interface, &errCode);
            if(0 != ret)
            {
                GPLUGMGR_LOG_ERROR(errCode, "GPLUGMGR_QueryInterface error, ikey=%s", IKEY_IType);
                return ret;
            }

            /* 调用获取名字接口 */
            IGetTypeInterface * p = (IGetTypeInterface*) interface;
            GPLUGMGR_LOG_INFO("fkey=%s, type=%s", fkeys[i], p->type().c_str());
        }

        /*查询插件实例是否具有获取重量功能接口集 */
        {
            GPluginHandle interface = GPLUGIN_INVALID_HANDLE;
            ret = GPLUGMGR_QueryInterface(instance, IKEY_IWeight, &interface, &errCode);
            if(0 != ret)
            {
                GPLUGMGR_LOG_ERROR(errCode, "GPLUGMGR_QueryInterface error, ikey=%s", IKEY_IWeight);
                return ret;
            }

            /* 调用获取名字接口 */
            IGetWeightInterface * p = (IGetWeightInterface*) interface;
            GPLUGMGR_LOG_INFO("fkey=%s, weight=%s", fkeys[i], p->weight().c_str());
        }

		/* 查询插件属性 */
		{
			char attrBuf[256] = {0};
			unsigned int attrLen = sizeof(attrBuf);
			ret = GPLUGMGR_QueryConfigAttribute(fkeys[i], "file", attrBuf, &attrLen);
			if(0 != ret)
			{
				GPLUGMGR_LOG_ERROR(ret, "GPLUGMGR_QueryConfigAttribute error");
				return ret;
			}
			GPLUGMGR_LOG_INFO("file=%s", attrBuf);
		}

        ret = GPLUGMGR_DestroyInstance(instance, &errCode);
        if(0 != ret)
        {
            GPLUGMGR_LOG_ERROR(0, "GPLUGMGR_DestroyInstance error");
            break;
        }
    }

    GPLUGMGR_ReleaseAllFkeys(fkeys, fkeysCout);

    GPLUGMGR_Uninit();
    return 0;
}

```



## 
