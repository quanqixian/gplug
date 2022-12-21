#include "gplug.h"
#include "tinyxml2.h"
#include "Debug.h"
#include <string>

#if (defined(_WIN32) || defined(_WIN64))
    #include <io.h>
    #include <direct.h>
#else
    #include <unistd.h>
#endif

static bool isPathExist(std::string path)
{
    bool ret = true;

#if (defined(_WIN32) || defined(_WIN64))
    ret = (0 == _access(path.c_str(), 0));
#else
    ret = (0 == access(path.c_str(), F_OK));
#endif

    return ret;
}

static bool getCurrentWorkDir(std::string & path)
{
    bool ret = true;
    char buf[512] = {0};

#if (defined(_WIN32) || defined(_WIN64))
    ret = (NULL != _getcwd(buf, sizeof(buf)));
#else
    ret = (NULL != getcwd(buf, sizeof(buf)));
#endif

    path = buf;
    return ret;
}

/**
 * @brief      拼接路径
 * @param[in]  basePath : 基础路径
 * @param[out] retPath : 全路径
 * @return     true : success false : fail
 */
static bool splicePath(std::string basePath, std::string & retPath)
{
    bool ret = true;
    std::string workDir;
    std::string fullPath;

    /* 获取当前工作路径 */
    ret = getCurrentWorkDir(workDir);
    if(!ret)
    {
        GPLUG_LOG_ERROR(-1, "file to getCurrentWorkDir");
        return ret;
    }

    /* 拼接全路径 */
#if (defined(_WIN32) || defined(_WIN64))
    fullPath = workDir + std::string("\\") + basePath;
#else
    fullPath = workDir + std::string("/") + basePath;
#endif

    /* 检查是否存在文件或路径 */
    ret = isPathExist(fullPath);
    if(!ret)
    {
        GPLUG_LOG_ERROR(-1, "file or dir is not exist, fullPath=%s", fullPath.c_str());
        return ret;
    }

    GPLUG_LOG_INFO("file or dir is exist, fullPath=%s", fullPath.c_str());

    /* 设置返回值 */
    retPath = fullPath;

    return ret;
}


int GPLUG_API GPLUG_Init()
{
	/* 读取配置文件 */
	std::string fullPath;
	std::string basePath = "gplugin/gplugin.xml";
	splicePath(basePath, fullPath);
	
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
