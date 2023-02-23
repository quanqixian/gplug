#include "gplugMgr.h"
#include "tinyxml2.h"
#include "Debug.h"
#include "SysWrapper.h"

#include <string>
#include <map>
#include <set>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

/* This is not a good way to deal with different symbols on different platforms */
#ifdef _WIN32
    #ifdef _WIN64
        #define GPLUGIN_SYMBOL "GPLUGIN_GetPluginInterface"
    #else
        #define GPLUGIN_SYMBOL "_GPLUGIN_GetPluginInterface@0"
    #endif
#else
    #define GPLUGIN_SYMBOL "GPLUGIN_GetPluginInterface"
#endif

using namespace SysWrapper;

struct Plugin
{
    std::string fkey;
    std::string file;
    std::string filePath;
    DLWrapper::DLHandle dlHandler; /* The dynamic library handle obtained after opening the dynamic library */
    GPlugin_GetPluginInterface pluginInterface;
    bool delayload;
    Plugin() : dlHandler(NULL), pluginInterface(NULL), delayload(false)
    {
    }
};

std::map<std::string, Plugin> m_pluginMap;      /* first: file key, second: plugin info */
std::map<GPluginHandle, Plugin*> m_instanceMap; /*  */
SysWrapper::Mutex m_mutex;

static int loadConfigFile()
{
    bool ret = true;

    /* Stitching configuration file path */
    std::string fullPath;

#if (defined(_WIN32) || defined(_WIN64))
    std::string basePath = "gplugin\\gplugin.xml";
#else
    std::string basePath = "gplugin/gplugin.xml";
#endif

    ret = PathWrapper::splicePath(basePath, fullPath);
    if(!ret)
    {
        GPLUGMGR_LOG_ERROR(GPLUGMGR_ERROR_FileNotExist, "Cofig file not exist, fullPath:%s", fullPath.c_str());
        return GPLUGMGR_ERROR_FileNotExist;
    }

    /* load xml from file */
    tinyxml2::XMLDocument doc;
    int xmlRet = doc.LoadFile(fullPath.c_str());
    if(tinyxml2::XML_SUCCESS != xmlRet)
    {
        GPLUGMGR_LOG_ERROR(doc.ErrorID(), "Fail to load xml file:%s", fullPath.c_str());
        return GPLUGMGR_ERROR_InvalidConfigFile;
    }

    /* xml content validation */
    tinyxml2::XMLElement* plugin = NULL; 
    ret = ret && doc.FirstChildElement("gplug");
    if(!ret)
    {
        GPLUGMGR_LOG_ERROR(doc.ErrorID(), "Content error in xml file:%s", fullPath.c_str());
        return GPLUGMGR_ERROR_InvalidConfigFile;
    }

    /* No element is OK */
    ret = ret && (plugin = doc.FirstChildElement("gplug")->FirstChildElement("plugin"));
    if(!ret)
    {
        GPLUGMGR_LOG_WARN(0, "No element in config file:%s", fullPath.c_str());
        return GPLUGMGR_OK;
    }

    for(; ret && plugin; plugin = plugin->NextSiblingElement("plugin"))
    {
        ret = ret && plugin->FindAttribute("fkey");
        ret = ret && plugin->FindAttribute("file");
        ret = ret && plugin->FindAttribute("delayload");
    }
    if(!ret)
    {
        GPLUGMGR_LOG_ERROR(doc.ErrorID(), "Content error in xml file:%s", fullPath.c_str());
        return GPLUGMGR_ERROR_InvalidConfigFile;
    }

    std::set<std::string> fileSet;
    /* lock the mutex */
    LockGuard guard(&m_mutex);

    /* read configuration */
    plugin = doc.FirstChildElement("gplug")->FirstChildElement("plugin");
    do 
    {
        /* read configuration parameters */
        Plugin p;
        p.fkey = plugin->Attribute("fkey");
        plugin->QueryBoolAttribute("delayload", &p.delayload);
    
        std::string file = plugin->Attribute("file");
        p.file = file;

        /**
         * Take the directory where the configuration file is located as the basic directory, 
         * and the absolute path of the stitching file 
         */
    #if (defined(_WIN32) || defined(_WIN64))
        file = "gplugin\\" + file;
        std::replace (file.begin(), file.end(), '/', '\\');
    #else
        file = "gplugin/" + file;
        std::replace (file.begin(), file.end(), '\\', '/');
    #endif

        std::string fullpath;
        ret = PathWrapper::splicePath(file, fullPath);
        if(!ret)
        {
            GPLUGMGR_LOG_ERROR(-1, "Plugin file not exist, path :%s", fullPath.c_str());
            return GPLUGMGR_ERROR_FileNotExist;
        }
        p.filePath = fullPath;

        /* Check for duplicates in fkey */
        if(m_pluginMap.find(p.fkey) != m_pluginMap.end())
        {
            GPLUGMGR_LOG_ERROR(-1, "fkey can not repeated in configure file, fkey :%s", p.fkey.c_str());
            return GPLUGMGR_ERROR_InvalidConfigFile;
        }

        /* Check for duplicate plugin filenames */
        if(fileSet.find(p.file) != fileSet.end())
        {
            GPLUGMGR_LOG_ERROR(-1, "Plugin file can not repeated in configure file, file :%s", p.file.c_str());
            return GPLUGMGR_ERROR_InvalidConfigFile;
        }
        fileSet.insert(p.file);

        m_pluginMap[p.fkey] = p;
        GPLUGMGR_LOG_INFO("Plugin fkey=%s, file=%s,delayload=%d", p.fkey.c_str(), p.filePath.c_str(), p.delayload);
        
        plugin = plugin->NextSiblingElement("plugin");
    }while(plugin);

    return GPLUGMGR_OK;
}

static int loadPlugins()
{
    /* lock the mutex */
    LockGuard guard(&m_mutex);

    /* Load dynamic library according to configuration file */
    for(std::map<std::string, Plugin>::iterator iter = m_pluginMap.begin(); iter != m_pluginMap.end(); ++iter)
    {
        Plugin & p = iter->second;
    
        if(p.delayload)
        {
            GPLUGMGR_LOG_INFO("fkey:%s delayload", p.fkey.c_str());
            continue;
        }

        p.dlHandler = DLWrapper::open(p.filePath.c_str());
        if(NULL == p.dlHandler)
        {
            GPLUGMGR_LOG_ERROR(-1, "Load the dynamic library fail, filePath:%s, error:%s", p.filePath.c_str(), DLWrapper::getError().c_str());
            return GPLUGMGR_ERROR_LoadDsoFailed;
        }
        p.pluginInterface = (GPlugin_GetPluginInterface)DLWrapper::getSym(p.dlHandler, GPLUGIN_SYMBOL);
        if(NULL == p.pluginInterface)
        {
            GPLUGMGR_LOG_ERROR(-1, "Fail to get symbol from %s, error:%s", p.filePath.c_str(), DLWrapper::getError().c_str());
            DLWrapper::close(p.dlHandler);
            p.dlHandler = NULL;
            return GPLUGMGR_ERROR_InvalidPlugin;
        }

        /* Plugin initialization */
        int pRet = p.pluginInterface()->Init();
        if(0 != pRet)
        {
            GPLUGMGR_LOG_ERROR(pRet, "Unit plugin failed, plugin:%s", p.filePath.c_str());
            DLWrapper::close(p.dlHandler);
            p.dlHandler = NULL;
            return GPLUGMGR_ERROR_InitPluginFailed;
        }
    }

    return GPLUGMGR_OK;
}

int GPLUGMGR_API GPlugMgr_Init()
{
    int ret = GPLUGMGR_OK;

    /* Load configuration parameters */
    ret = loadConfigFile();
    if(GPLUGMGR_OK != ret)
    {
        GPLUGMGR_LOG_ERROR(ret, "Fail to load confiugure file");
        return ret;
    }

    /* load plugin */
    ret = loadPlugins();
    if(GPLUGMGR_OK != ret)
    {
        GPLUGMGR_LOG_ERROR(ret, "Fail to load plugins");
        return ret;
    }

    return GPLUGMGR_OK;
}

void GPLUGMGR_API GPlugMgr_Deinit()
{
    LockGuard guard(&m_mutex);

    /* destroy all instances */
    for(std::map<GPluginHandle, Plugin*>::iterator iter = m_instanceMap.begin(); iter != m_instanceMap.end(); ++ iter)
    {
        Plugin * p = iter->second;
        p->pluginInterface()->DestroyInstance(iter->first);
    }
    m_instanceMap.clear();

    /* unload open library */
    for(std::map<std::string, Plugin>::iterator iter = m_pluginMap.begin(); iter != m_pluginMap.end(); ++iter)
    {
        Plugin & p = iter->second;
        if(p.dlHandler)
        {
            if(p.pluginInterface)
            {
                /* Plugin deinitialization */
                p.pluginInterface()->Uninit();
            }

            DLWrapper::close(p.dlHandler);
            p.dlHandler = NULL;
        }
    }
    m_pluginMap.clear();
}

int GPLUGMGR_API GPlugMgr_CreateInstance(const char* fkey, GPluginHandle* pInstance, int* pluginError)
{
    LockGuard guard(&m_mutex);

    std::map<std::string, Plugin>::iterator iter = m_pluginMap.find(fkey);
    if(iter == m_pluginMap.end())
    {
        return GPLUGMGR_ERR;
    }
    
    Plugin & p = iter->second;

    /* Plugin not loaded due to lazy loading, loaded on first use */
    if(NULL == p.pluginInterface)
    {
        p.dlHandler = DLWrapper::open(p.filePath.c_str());
        if(NULL == p.dlHandler)
        {
            GPLUGMGR_LOG_ERROR(-1, "Load the dynamic library fail, filePath:%s, error:%s", p.filePath.c_str(), DLWrapper::getError().c_str());
            return GPLUGMGR_ERROR_LoadDsoFailed;
        }
        p.pluginInterface = (GPlugin_GetPluginInterface)DLWrapper::getSym(p.dlHandler, GPLUGIN_SYMBOL);
        if(NULL == p.pluginInterface)
        {
            GPLUGMGR_LOG_ERROR(-1, "Fail to get symbol from %s, error:%s", p.filePath.c_str(), DLWrapper::getError().c_str());
            return GPLUGMGR_ERROR_InvalidPlugin;
        }

        /* Plugin lazy initialization */
        *pluginError = p.pluginInterface()->Init();
        if(0 != *pluginError)
        {
            GPLUGMGR_LOG_ERROR(*pluginError, "Unit plugin failed, plugin:%s", p.filePath.c_str());
            return GPLUGMGR_ERROR_InitPluginFailed;
        }
        GPLUGMGR_LOG_WARN(0, "fkey=%s, file=%s delayload ok", p.fkey.c_str(), p.file.c_str());
    }

    *pluginError = p.pluginInterface()->CreateInstance(pInstance);
    if(0 != *pluginError)
    {
        GPLUGMGR_LOG_ERROR(*pluginError, "CreateInstance failed, plugin:%s", p.filePath.c_str());
        return GPLUGMGR_ERR;
    }

    m_instanceMap[*pInstance] = &p;

    return GPLUGMGR_OK;
}

int GPLUGMGR_API GPlugMgr_DestroyInstance(GPluginHandle instance, int* pluginError)
{
    if(NULL == instance)
    {
        return GPLUGMGR_ERR;
    }

    LockGuard guard(&m_mutex);

    std::map<GPluginHandle, Plugin*>::iterator iter = m_instanceMap.find(instance);
    if(iter == m_instanceMap.end())
    {
        return GPLUGMGR_ERR;
    }
    
    Plugin * p = iter->second;

    *pluginError = p->pluginInterface()->DestroyInstance(instance);
    if(0 != *pluginError)
    {
        GPLUGMGR_LOG_ERROR(*pluginError, "DestroyInstance failed, plugin:%s", p->filePath.c_str());
        return GPLUGMGR_ERR;
    }

    m_instanceMap.erase(iter);

    return GPLUGMGR_OK;
}

int GPLUGMGR_API GPlugMgr_QueryInterface(GPluginHandle instance, const char* ikey, GPluginHandle* pluginInterface, int* pluginError)
{
    if(NULL == instance)
    {
        return GPLUGMGR_ERR;
    }

    LockGuard guard(&m_mutex);

    std::map<GPluginHandle, Plugin*>::iterator iter = m_instanceMap.find(instance);
    if(iter == m_instanceMap.end())
    {
        return GPLUGMGR_ERR;
    }
    
    Plugin * p = iter->second;
    *pluginError = p->pluginInterface()->QueryInterface(instance, ikey, pluginInterface);
    if(0 != *pluginError)
    {
        GPLUGMGR_LOG_ERROR(*pluginError, "QueryInterface failed, plugin:%s", p->filePath.c_str());
        return GPLUGMGR_ERR;
    }

    return GPLUGMGR_OK;
}

int GPLUGMGR_API GPlugMgr_QueryConfigAttribute(const char* fkey, const char* attributeName, char* attributeValue, unsigned int* bufLen)
{
    bool ret = true;
    ret = ret && (NULL != fkey);
    ret = ret && (NULL != attributeName);
    ret = ret && (NULL != attributeValue);
    if(!ret)
    {
        GPLUGMGR_LOG_ERROR(-1, "Error parameter");
        return GPLUGMGR_ERR;
    }

    LockGuard guard(&m_mutex);

    std::map<std::string, Plugin>::iterator iter = m_pluginMap.find(fkey);
    if(iter == m_pluginMap.end())
    {
        return GPLUGMGR_ERR;
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
        return GPLUGMGR_ERR;
    }

    if(*bufLen < (value.size() + 1))
    {
        return GPLUGMGR_ERR;
    }

    strncpy(attributeValue, value.c_str(), value.size());
    attributeValue[value.size()] = 0;

    return GPLUGMGR_OK;
}

int GPLUGMGR_API GPlugMgr_QueryAllFkeys(char*** fkeys, unsigned int* fkeysCount)
{
    LockGuard guard(&m_mutex);

    int i = 0;
    *fkeysCount = m_pluginMap.size();
    *fkeys = (char**) malloc(sizeof(char*) * (*fkeysCount));

    for(std::map<std::string, Plugin>::iterator iter = m_pluginMap.begin(); iter != m_pluginMap.end(); ++iter)
    {
        int len = sizeof(char) * iter->first.length() + 1;
        (*fkeys)[i] = (char*) malloc(len);
        memset((*fkeys)[i], 0, len);
        memcpy((*fkeys)[i], iter->first.c_str(), iter->first.length());
        i++;
    }

    return GPLUGMGR_OK;
}

int GPLUGMGR_API GPlugMgr_ReleaseAllFkeys(char** fkeys, unsigned int fkeysCount)
{
    for(unsigned int i = 0; i < fkeysCount; i++)
    {
        free(fkeys[i]);
    }
    free(fkeys);
    return GPLUGMGR_OK;
}
