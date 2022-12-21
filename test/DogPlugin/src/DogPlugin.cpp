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
