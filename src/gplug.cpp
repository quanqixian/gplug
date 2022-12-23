#include "gplug.h"
#include "tinyxml2.h"
#include "Debug.h"
#include "SysWrapper.h"

#include <string>
#include <map>
#include <stdlib.h>
#include <string.h>

using namespace SysWrapper;

struct Plugin
{
    std::string fkey;
    std::string file;    
    std::string filePath;
    void *handler; /* 打开动态库后获得的动态库句柄 */
    GPlugin_GetPluginInterface pluginInterface;
    bool delayload;
    Plugin() : handler(NULL), pluginInterface(NULL), delayload(false)
    {
    }
};

std::map<std::string, Plugin> m_map;
std::map<GPluginHandle, Plugin*> m_instanceMap;

int GPLUG_API GPLUG_Init()
{
    bool ret = true;

    /* 拼接配置文件路径 */
    std::string fullPath;
    std::string basePath = "gplugin/gplugin.xml";
    ret = PathWrapper::splicePath(basePath, fullPath);
    if(!ret)
    {
        GPLUG_LOG_ERROR(GPLUG_E_FileNotExist, "Cofig file not exist, fullPath:%s", fullPath.c_str());
        return GPLUG_E_FileNotExist;
    }

    /* 从文件加载xml */
    tinyxml2::XMLDocument doc;
    int xmlRet = doc.LoadFile(fullPath.c_str());
    if(tinyxml2::XML_SUCCESS != xmlRet)
    {
        GPLUG_LOG_ERROR(doc.ErrorID(), "Fail to load xml file:%s", fullPath.c_str());
        return GPLUG_E_InvalidConfigFile;
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
        return GPLUG_E_InvalidConfigFile;
    }

    /* 读取配置 */
    plugin = doc.FirstChildElement("gplug")->FirstChildElement("plugin");
    do 
    {
        Plugin p;
        p.fkey = plugin->Attribute("fkey");
        plugin->QueryBoolAttribute("delayload", &p.delayload);
    
        std::string file = plugin->Attribute("file");
        p.file = file;
        /* 以配置文件所在目录为基本目录 */
        file = "gplugin/" + file;
        std::string fullpath;
        ret = PathWrapper::splicePath(file, fullPath);
        if(!ret)
        {
            GPLUG_LOG_ERROR(-1, "Plugin file not exist, path :%s", fullPath.c_str());
            return GPLUG_E_FileNotExist;
        }
        p.filePath = fullPath;

        if(m_map.find(p.fkey) != m_map.end())
        {
            GPLUG_LOG_ERROR(-1, "fkey can not repeated in configure file, fkey :%s", p.fkey.c_str());
            return GPLUG_E_InvalidConfigFile;
        }
        m_map[p.fkey] = p;

        GPLUG_LOG_INFO("Plugin fkey=%s, file=%s,delayload=%d", p.fkey.c_str(), p.filePath.c_str(), p.delayload);
        
        plugin = plugin->NextSiblingElement("plugin");
    }while(plugin);


    /* 根据配置文件加载动态库 */
    for(std::map<std::string, Plugin>::iterator iter = m_map.begin(); iter != m_map.end(); ++iter)
    {
        Plugin & p = iter->second;
        p.handler = DLWrapper::open(p.filePath.c_str());
        if(NULL == p.handler)
        {
            GPLUG_LOG_ERROR(-1, "Loads the dynamic library fail, filePath:%s, error:%s", p.filePath.c_str(), dlerror());
            return GPLUG_E_LoadDsoFailed;
        }
        p.pluginInterface = (GPlugin_GetPluginInterface)DLWrapper::getSym(p.handler, "GPLUGIN_GetPluginInterface");
        if(NULL == p.pluginInterface)
        {
            GPLUG_LOG_ERROR(-1, "Fail to get symbol from %s, error:%s", p.filePath.c_str(), dlerror());
            return GPLUG_E_InvalidPlugin;
        }

        /* 插件初始化 */
        int pRet = p.pluginInterface()->Init();
        if(0 != pRet)
        {
            GPLUG_LOG_ERROR(pRet, "Unit plugin failed, plugin:%s", p.filePath.c_str());
            return GPLUG_E_InitPluginFailed;
        }
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
                /* 插件反初始化 */
                p.pluginInterface()->Uninit();
            }

            DLWrapper::close(p.handler);
            p.handler = NULL;
        }
    }

    m_map.clear();
}

int GPLUG_API GPLUG_CreateInstance(const char* fkey, GPluginHandle* instance, int* plugin_error)
{
    std::map<std::string, Plugin>::iterator iter = m_map.find(fkey);
    if(iter == m_map.end())
    {
        return false;
    }
    
    Plugin & p = iter->second;
    p.pluginInterface()->CreateInstance(instance);

    m_instanceMap[*instance] = &p;
    *plugin_error = 0;

    return GPLUG_OK;
}

int GPLUG_API GPLUG_DestroyInstance(GPluginHandle instance, int* plugin_error)
{
    std::map<GPluginHandle, Plugin*>::iterator iter = m_instanceMap.find(instance);
    if(iter == m_instanceMap.end())
    {
        return false;
    }
    
    Plugin * p = iter->second;
    p->pluginInterface()->DestroyInstance(instance);
    m_instanceMap.erase(iter);

    *plugin_error = 0;
    return GPLUG_OK;
}

int GPLUG_API GPLUG_QueryInterface(GPluginHandle instance, const char* ikey, GPluginHandle* plugin_interface, int* plugin_error)
{
    std::map<GPluginHandle, Plugin*>::iterator iter = m_instanceMap.find(instance);
    if(iter == m_instanceMap.end())
    {
        return false;
    }
    
    Plugin * p = iter->second;
    p->pluginInterface()->QueryInterface(instance, ikey, plugin_interface);
    *plugin_error = 0;

    return GPLUG_OK;
}

int GPLUG_API GPLUG_QueryConfigAttribute(const char* fkey, const char* attributeName, char* attributeValue, unsigned int* bufLen)
{
    std::map<std::string, Plugin>::iterator iter = m_map.find(fkey);
    if(iter == m_map.end())
    {
        return false;
    }

    Plugin & p = iter->second;

    std::string value;
    if(std::string("file") == attributeName)
    {
        value = p.file;
    }
    else if(std::string("delayload") == attributeName)
    {
        value = p.delayload ? "true" : "false";
    }
    else
    {
        return false;
    }

    if(*bufLen < (value.size() + 1))
    {
        return false;
    }

    strncpy(attributeValue, value.c_str(), value.size());
    attributeValue[value.size()] = 0;

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
