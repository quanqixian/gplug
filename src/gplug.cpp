#include "gplug.h"
#include "tinyxml2.h"

int GPLUG_API GPLUG_Init()
{
	/* 读取配置文件 */
	
	/* 根据配置文件加载动态库 */

	return GPLUG_OK;
}

void GPLUG_API GPLUG_Uninit()
{
	/* 卸载打开的库 */
	return;
}

int GPLUG_API GPLUG_CreateInstance(const char* fkey, GPluginHandle* instance, int* plugin_error)
{
	return GPLUG_OK;
}

int GPLUG_API GPLUG_DestroyInstance(GPluginHandle instance, int* plugin_error)
{
	return GPLUG_OK;
}

int GPLUG_API GPLUG_QueryInterface(GPluginHandle instance, const char* ikey, GPluginHandle* plugin_interface, int* plugin_error)
{
	return GPLUG_OK;
}

int GPLUG_API GPLUG_QueryConfigAttribute(const char* fkey, const char* attributeName, char* attributeValue, unsigned int* bufLen)
{
	return GPLUG_OK;
}

int GPLUG_API GPLUG_QueryAllFkeys(char*** fkeys, unsigned int* fkeysCout)
{
	return GPLUG_OK;
}

int GPLUG_API GPLUG_ReleaseAllFkeys(char** fkeys, unsigned int fkeysCout)
{
	return GPLUG_OK;
}
