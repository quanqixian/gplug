/**
 * @file gplugMgr.h
 * 
 * Plugin Manager
 */

#ifndef _GPLUGMGR_MANAGER_H__
#define _GPLUGMGR_MANAGER_H__

#include "gplugin.h"

#ifdef _WIN32
    #if defined(GPLUGMGR_EXPORTS)
        #define GPLUGMGR_EXPORT extern "C" __declspec(dllexport)
    #else
        #define GPLUGMGR_EXPORT extern "C" __declspec(dllimport)
    #endif
    #define GPLUGMGR_API __stdcall
#elif defined(__linux__)
    #define GPLUGMGR_EXPORT extern "C"
    #define GPLUGMGR_API
#else
    #define GPLUGMGR_EXPORT
    #define GPLUGMGR_API
#endif

/*
 * 插件管理器类型、常量、错误码定义
 */
enum GPlugErrorCode
{
    GPLUGMGR_OK                     = 0,                 /* 操作成功 */
    GPLUGMGR_ERR                    = -1,                /* 操作失败 */

    GPLUGMGR_E_InvalidParam         = (0x8000 + 1),      /* 输入参数无效。                                                        */
    GPLUGMGR_E_FileNotExist,       /* 插件库文件不存在。                                                                          */
    GPLUGMGR_E_FkeyNotExist,       /* 插件不存在（插件功能标识不存在）。                                                          */
    GPLUGMGR_E_IkeyNotExist,       /* 插件功能接口集不存在（插件功能接口集标识不存在）。                                          */
    GPLUGMGR_E_InvalidPlugin,      /* 无效的插件（未按规则导出 Hplugin_GetPluginInterface 接口，或与插件管理器交互的接口集无效    */
    GPLUGMGR_E_LoadDsoFailed,      /* 加载插件库失败。                                                                            */
    GPLUGMGR_E_UnloadDsoFailed,    /* 卸载插件库失败。                                                                            */
    GPLUGMGR_E_OutOfMemory,        /* 申请内存失败。                                                                              */
    GPLUGMGR_E_ParseXmlFailed,     /* 解析配置文件失败。                                                                          */
    GPLUGMGR_E_MemoryNotEnough,    /* 缓冲区空间不足。                                                                            */
    GPLUGMGR_E_InitPluginFailed,   /* 初始化插件失败。                                                                            */
    GPLUGMGR_E_UninitPluginFailed, /* 反初始化插件失败。                                                                          */
    GPLUGMGR_E_PluginConflict,     /* 插件之间冲突（如插件库文件名相同、插件标识相同等）。                                        */
    GPLUGMGR_E_InstanceConflict,   /* 插件实例冲突（存在插件实例相同）。                                                          */
    GPLUGMGR_E_InvalidConfigFile,  /* 无效的配置文件                                                                              */
    GPLUGMGR_E_NotSupport          /* not support */
};

/**
 * @brief      Initialize the plugin manager.
 * @return     Returns 0 on success, other values on failure, see @ref GPlugErrorCode.
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_Init();

/**
 * @brief      Deinitialize the plugin manager.
 * @return     void
 */
GPLUGMGR_EXPORT void GPLUGMGR_API GPLUGMGR_Deinit();

/**
 * @brief      Create a plugin instance.
 * @param[in]  fkey : Plugin file identifier(file key)
 * @param[out] pInstance : If the creation is successful, it is used to return the plugin instance handle, otherwise it returns NULL.
 * @param[out] pluginError : The error code returned by the plugin when the creation of the plugin instance fails.
 * @return     Returns 0 on success, other values on failure, see @ref GPlugErrorCode.
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_CreateInstance(const char* fkey, GPluginHandle* pInstance, int* pluginError);

/**
 * @brief      Destroys the plugin instance.
 * @param[in]  instance : Plugin instance handle.
 * @param[out] pluginError : The error code returned by the plugin when the creation of the plugin instance fails.
 * @return     Returns 0 on success, other values on failure, see @ref GPlugErrorCode.
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_DestroyInstance(GPluginHandle instance, int* pluginError);

/**
 * @brief      Get the set of plugin function interfaces.
 * @param[in]  instance : Plugin instance handle.
 * @param[in]  ikey : Plugin function interface set identification.
 * @param[out] plugin_interface : Return the interface corresponding to ikey, If the interface is not implemented, return HPLUGIN_NOTSUPPORT
 * @param[out] pluginError : The error code returned by the plugin when the creation of the plugin instance fails.().
 * @return     Returns 0 on success, other values on failure, see @ref GPlugErrorCode.
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_QueryInterface(GPluginHandle instance, const char* ikey, GPluginHandle* plugin_interface, int* pluginError);

/**
 * @brief      Query plugin configuration attribute.
 * @param[in]  fkey : Plugin file identifier(file key)
 * @param[in]  attributeName : Configuration parameter attribute name.
 * @param[out] attributeValue : 配置属性值, 若为NULL，则接口只在buf_len中返回实际长度。
 * @param[in/out] buf_len : 配置属性缓冲区长度/实际value字符串长度。
 * @return     Returns 0 on success, other values on failure, see @ref GPlugErrorCode.
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_QueryConfigAttribute(const char* fkey, const char* attributeName, char* attributeValue, unsigned int* bufLen);

/**
 * @brief      Query the fkeys of all plugins.
 * @param[out] fkeys : Plugin file identifier(file key) array
 * @param[out] fkeysCount : Number of plugins(array size).
 * @return     Returns 0 on success, other values on failure, see @ref GPlugErrorCode.
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_QueryAllFkeys(char*** fkeys, unsigned int* fkeysCount);

/**
 * @brief      Free plugin fkeys memory.
 * @param[in]  fkeys : Plugin file identifier(file key) array
 * @param[in]  fkeysCount : Number of plugins(array size).
 * @return     Returns 0 on success, other values on failure, see @ref GPlugErrorCode.
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_ReleaseAllFkeys(char** fkeys, unsigned int fkeysCount);

#endif

