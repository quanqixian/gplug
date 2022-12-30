#include "Cat.h"
#include "Debug.h"
#include "gplugin.h"
#include <string.h>
#include <stdlib.h>

int GPLUGIN_API CatPlugin_init()
{
	GPLUG_LOG_INFO("CatPlugin_init");
	return GPLUGIN_OK;
}

int GPLUGIN_API CatPlugin_Uninit()
{
	GPLUG_LOG_INFO("CatPlugin_Uninit");
	return GPLUGIN_OK;
}

int GPLUGIN_API CatPlugin_CreateInstance(GPluginHandle * instance)
{
	Cat * p = (Cat*) malloc(sizeof(Cat));
	Cat_init(p);
	*instance = (GPluginHandle)p;

	return GPLUGIN_OK;
}

int GPLUGIN_API CatPlugin_DestroyInstance(GPluginHandle instance)
{
	Cat * p = (Cat*) instance;
	Cat_deInit(p);
	free(p);
	return GPLUGIN_OK;
}

int GPLUGIN_API CatPlugin_QueryInterface(GPluginHandle instance, const char * ikey, GPluginHandle * interface)
{
	int ret = GPLUGIN_OK;
	Cat * p = (Cat*) instance;

	if(0 == strcmp(ikey, IKEY_IType))
	{
		*interface = (GPluginHandle)(&p->typeInterface);
	}
	else if(0 == strcmp(ikey, IKEY_IWeight))
	{
		*interface = (GPluginHandle)(&p->weightInterface);
	}
	else
	{
		*interface = GPLUGIN_INVALID_HANDLE;
		ret = GPLUGIN_NOTSUPPORT;
	}

	return ret;
}

const char** GPLUGIN_API CatPlugin_GetAllInterfaceIkeys()
{
	static const char * ikeys[] = {
		IKEY_IType,
		IKEY_IWeight,
		NULL
	};

	return  ikeys;
}

const char* GPLUGIN_API CatPlugin_GetFileVersion()
{
	return "V1.0.0.0";
}

GPLUGIN_MAKE_EXPORT_INTERFACE(CatPlugin_init, CatPlugin_Uninit, CatPlugin_CreateInstance,
		CatPlugin_DestroyInstance, CatPlugin_QueryInterface,
		CatPlugin_GetAllInterfaceIkeys, CatPlugin_GetFileVersion);
