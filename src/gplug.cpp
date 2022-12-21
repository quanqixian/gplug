#include "gplug.h"
#include "tinyxml2.h"
#include "Debug.h"
#include <string>
#include <map>
#include <stdlib.h>
#include <string.h>


#if (defined(_WIN32) || defined(_WIN64))
    #include <io.h>
    #include <direct.h>
#else
    #include <unistd.h>
    #include <dlfcn.h>
#endif

struct Plugin
{
    std::string fkey;
    std::string filePath;
	void *handler; /* dlopen后获得的动态库句柄 */
	GPlugin_GetPluginInterface pluginInterface;
    bool delayload;
	Plugin() : handler(NULL), pluginInterface(NULL), delayload(false)
	{
	}
};

std::map<std::string, Plugin> m_map;

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

    /* 设置返回值 */
    retPath = fullPath;

    /* 检查是否存在文件或路径 */
    ret = isPathExist(fullPath);
    if(!ret)
    {
        GPLUG_LOG_WARN(0, "file or dir is not exist, fullPath=%s", fullPath.c_str());
        return ret;
    }

    GPLUG_LOG_INFO("file or dir is exist, fullPath=%s", fullPath.c_str());

    return ret;
}


int GPLUG_API GPLUG_Init()
{
    bool ret = true;

    /* 拼接配置文件路径 */
    std::string fullPath;
    std::string basePath = "gplugin/gplugin.xml";
    splicePath(basePath, fullPath);

    /* 从文件加载xml */
    tinyxml2::XMLDocument doc;
    int xmlRet = doc.LoadFile(fullPath.c_str());
    if(tinyxml2::XML_SUCCESS != xmlRet)
    {
        GPLUG_LOG_ERROR(doc.ErrorID(), "Fail to load xml file:%s", fullPath.c_str());
        return false;
    }

    /* xml内容校验 */
    tinyxml2::XMLElement* plugin = NULL; 
    ret = ret && doc.FirstChildElement("gplug");
    ret = ret && (plugin = doc.FirstChildElement("gplug")->FirstChildElement("plugin"));

    for(; ret && plugin; plugin = plugin->NextSiblingElement("plugin"))
    {
        ret = ret && plugin->FindAttribute("fkey");
        ret = ret && plugin->FindAttribute("file");
        ret = ret && plugin->FindAttribute("delayload");
    }
    if(!ret)
    {
        GPLUG_LOG_ERROR(doc.ErrorID(), "Content error in xml file:%s", fullPath.c_str());
        return ret;
    }

    /* 读取配置 */
    plugin = doc.FirstChildElement("gplug")->FirstChildElement("plugin");
    do 
    {
        Plugin p;
        p.fkey = plugin->Attribute("fkey");
        plugin->QueryBoolAttribute("delayload", &p.delayload);
    
        std::string file = plugin->Attribute("file");
        /* 以配置文件所在目录为基本目录 */
        file = "gplugin/" + file;
        std::string fullpath;
        ret = splicePath(file, fullPath);
		if(!ret)
		{
			GPLUG_LOG_ERROR(-1, "Plugin file not exist, path :%s", fullPath.c_str());
			return ret;
		}
        p.filePath = fullPath;

		if(m_map.find(p.fkey) != m_map.end())
		{
			GPLUG_LOG_ERROR(-1, "fkey can not repeated in configure file, fkey :%s", p.fkey.c_str());
			return ret;
		}
		m_map[p.fkey] = p;

        GPLUG_LOG_INFO("Plugin fkey=%s, file=%s,delayload=%d", p.fkey.c_str(), p.filePath.c_str(), p.delayload);
		
        plugin = plugin->NextSiblingElement("plugin");
    }while(plugin);


    /* 根据配置文件加载动态库 */
	for(std::map<std::string, Plugin>::iterator iter = m_map.begin(); iter != m_map.end(); ++iter)
	{
		Plugin & p = iter->second;
		p.handler = dlopen(p.filePath.c_str(), RTLD_LAZY);
		if(NULL == p.handler)
		{
			GPLUG_LOG_ERROR(-1, "Loads the dynamic library fail, filePath:%s, error:%s", p.filePath.c_str(), dlerror());
			return false;
		}
		p.pluginInterface = (GPlugin_GetPluginInterface)dlsym(p.handler, "GPLUGIN_GetPluginInterface");
		if(NULL == p.pluginInterface)
		{
			GPLUG_LOG_ERROR(-1, "Fail to get symbol from %s, error:%s", p.filePath.c_str(), dlerror());
			return false;
		}

		p.pluginInterface()->Init();

	}
    return GPLUG_OK;
}

void GPLUG_API GPLUG_Uninit()
{
    /* 卸载打开的库 */
	for(std::map<std::string, Plugin>::iterator iter = m_map.begin(); iter != m_map.end(); ++iter)
	{
		Plugin & p = iter->second;
		if(p.handler)
		{
			if(p.pluginInterface)
			{
				p.pluginInterface()->Uninit();
			}

			dlclose(p.handler);
			p.handler = NULL;
		}
	}
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
	int i = 0;
	*fkeysCout = m_map.size();
	*fkeys = (char**) malloc(sizeof(char*) * (*fkeysCout));

	for(std::map<std::string, Plugin>::iterator iter = m_map.begin(); iter != m_map.end(); ++iter)
	{
		int len = sizeof(char) * iter->first.length() + 1;
		(*fkeys)[i] = (char*) malloc(len);
		memset((*fkeys)[i], 0, len);
		memcpy((*fkeys)[i], iter->first.c_str(), iter->first.length());
		i++;
	}

    return GPLUG_OK;
}

int GPLUG_API GPLUG_ReleaseAllFkeys(char** fkeys, unsigned int fkeysCout)
{
	for(unsigned int i = 0; i < fkeysCout; i++)
	{
		free(fkeys[i]);
	}
	free(fkeys);
    return GPLUG_OK;
}
