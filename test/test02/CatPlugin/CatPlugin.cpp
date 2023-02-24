#include "Cat.h"
#include "Debug.h"
#include "gplugin.h"
#include <string.h>

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
	*instance = (GPluginHandle) (new Cat());
	return GPLUGIN_OK;
}

int GPLUGIN_API CatPlugin_DestroyInstance(GPluginHandle instance)
{
	delete (Cat*) instance;
	return GPLUGIN_OK;
}

int GPLUGIN_API CatPlugin_QueryInterface(GPluginHandle instance, const char * ikey, GPluginHandle * interface)
{
	int ret = GPLUGIN_OK;

	if(0 == strcmp(ikey, IKEY_IType))
	{
		*interface = (GPluginHandle)(dynamic_cast<IGetTypeInterface*>((Cat*)instance));
	}
	else if(0 == strcmp(ikey, IKEY_IWeight))
	{
		*interface = (GPluginHandle)(dynamic_cast<IGetWeightInterface*>((Cat*)instance));
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
