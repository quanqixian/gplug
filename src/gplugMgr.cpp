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
volatile bool m_isInited = false;               /* initialized flag */

static int checkAndprintPluginInfo(const Plugin & p)
{
    bool ret = true;

    ret = ret && (NULL != p.pluginInterface()->Init);
    ret = ret && (NULL != p.pluginInterface()->Uninit);
    ret = ret && (NULL != p.pluginInterface()->CreateInstance);
    ret = ret && (NULL != p.pluginInterface()->DestroyInstance);
    ret = ret && (NULL != p.pluginInterface()->QueryInterface);
    ret = ret && (NULL != p.pluginInterface()->GetAllInterfaceIkeys);
    ret = ret && (NULL != p.pluginInterface()->GetFileVersion);
    if(!ret)
    {
        GPLUGMGR_LOG_ERROR(-1, "Plugin function invalid");
        return GPLUGMGR_ERROR_InvalidPlugin;
    }

    GPLUGMGR_LOG_INFO("***** Plugin info *****");
    GPLUGMGR_LOG_INFO("Plugin fkey=%s", p.fkey.c_str());
    GPLUGMGR_LOG_INFO("Plugin file=%s", p.file.c_str());
    GPLUGMGR_LOG_INFO("Plugin filePath=%s", p.filePath.c_str());
    GPLUGMGR_LOG_INFO("Plugin version=%s", p.pluginInterface()->GetFileVersion());
    return GPLUGMGR_OK;
}

static int loadConfigFile(const std::string & fullPath)
{
    bool ret = true;

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

    /* lock the mutex */
    LockGuard guard(&m_mutex);

    std::set<std::string> fileSet;

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

        std::string dirName = fullPath.substr(0, fullPath.size() - strlen("gplugin.xml") - 1);
    #if (defined(_WIN32) || defined(_WIN64))
        std::replace(file.begin(), file.end(), '/', '\\');
        std::string pluginPath = dirName +  "\\" + file;
    #else
        std::replace(file.begin(), file.end(), '\\', '/');
        std::string pluginPath = dirName +  "/" + file;
    #endif

        p.filePath = pluginPath;

        /* Check for duplicates in fkey */
        if(m_pluginMap.find(p.fkey) != m_pluginMap.end())
        {
            GPLUGMGR_LOG_ERROR(-1, "fkey can not repeated in configure file, fkey :%s", p.fkey.c_str());
            return GPLUGMGR_ERROR_InvalidConfigFile;
        }

        /* Check for duplicate plugin filenames */
        if(fileSet.find(pluginPath) != fileSet.end())
        {
            GPLUGMGR_LOG_ERROR(-1, "Plugin file can not repeated in configure file, file :%s", p.file.c_str());
            return GPLUGMGR_ERROR_InvalidConfigFile;
        }
        fileSet.insert(pluginPath);

        m_pluginMap[p.fkey] = p;
        GPLUGMGR_LOG_INFO("Plugin fkey=%s, file=%s,delayload=%d", p.fkey.c_str(), p.filePath.c_str(), p.delayload);
        
        plugin = plugin->NextSiblingElement("plugin");
    }while(plugin);

    return GPLUGMGR_OK;
}

static int loadConfigFiles()
{
    std::string currentPath;
    bool boolRet = PathWrapper::splicePath(".", currentPath);
    if(!boolRet)
    {
        GPLUGMGR_LOG_ERROR(-1, "Fail to get current path.");
        return GPLUGMGR_ERR;
    }

    std::vector<std::string> retVec;
    boolRet = PathWrapper::getFilesInDir(currentPath, "gplugin.xml", retVec);
    if(!boolRet)
    {
        GPLUGMGR_LOG_ERROR(-1, "Fail to getFilesInDir.");
        return GPLUGMGR_ERR;
    }

    for(unsigned int i = 0; i < retVec.size(); i++)
    {
        int ret = loadConfigFile(retVec[i]);
        if(0 != ret)
        {
            GPLUGMGR_LOG_ERROR(ret, "Fail to load configure file:%s", retVec[i].c_str());
            return ret;
        }
    }

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

        int ret = checkAndprintPluginInfo(p);
        if(0 != ret)
        {
            GPLUGMGR_LOG_ERROR(ret, "checkAndprintPluginInfo error");
            return ret;
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

    {
        /* lock the mutex */
        LockGuard guard(&m_mutex);
        if(m_isInited)
        {
            GPLUGMGR_LOG_ERROR(-1, "Already inited, can not init again.");
            return GPLUGMGR_ERR;
        }
    }

    /* Load configuration files */
    ret = loadConfigFiles();
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

    {
        /* lock the mutex */
        LockGuard guard(&m_mutex);
        /* set initialized flag */
        m_isInited = true;
    }

    return GPLUGMGR_OK;
}

int GPLUGMGR_API GPlugMgr_Deinit()
{
    LockGuard guard(&m_mutex);

    if(!m_isInited)
    {
        GPLUGMGR_LOG_WARN(-1, "No initialize, can not deinitialize");
        return GPLUGMGR_ERR;    
    }

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

    m_isInited = false;

    return GPLUGMGR_OK;
}

int GPLUGMGR_API GPlugMgr_CreateInstance(const char* fkey, GPluginHandle* pInstance, int* pluginError)
{
    bool ret = true;
    ret = ret && (NULL != fkey);
    ret = ret && (NULL != pInstance);
    ret = ret && (NULL != pluginError);
    if(!ret)
    {
        GPLUGMGR_LOG_ERROR(-1, "Error parameter");
        return GPLUGMGR_ERR;
    }

    *pInstance = NULL;

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

        int ret = checkAndprintPluginInfo(p);
        if(0 != ret)
        {
            GPLUGMGR_LOG_ERROR(ret, "checkAndprintPluginInfo error");
            return ret;
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
    if((NULL == instance) || (NULL == pluginError))
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
    ret = ret && (NULL != bufLen);
    ret = ret && (*bufLen > 1);
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
    *bufLen = value.size();

    return GPLUGMGR_OK;
}

int GPLUGMGR_API GPlugMgr_QueryAllFkeys(char*** fkeys, unsigned int* fkeysCount)
{
    if((NULL == fkeys) || (NULL == fkeysCount))
    {
        GPLUGMGR_LOG_ERROR(-1, "Error parameters.");
        return GPLUGMGR_ERR;
    }

    LockGuard guard(&m_mutex);

    if(!m_isInited)
    {
        GPLUGMGR_LOG_ERROR(-1, "No initialize.");
        return GPLUGMGR_ERR;
    }

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
    if(NULL == fkeys)
    {
        GPLUGMGR_LOG_ERROR(-1, "Error parameters.");
        return GPLUGMGR_ERR;
    }

    for(unsigned int i = 0; i < fkeysCount; i++)
    {
        free(fkeys[i]);
    }
    free(fkeys);
    return GPLUGMGR_OK;
}
