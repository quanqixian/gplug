/**
 * @file gplugMgr.h
 * 
 * This file implements the Plugin Manager API. The interface of Plugin Manager
 * adopts c language style, so you can choose c or c++ language to implement
 * these interfaces, and here use C++ to implement.
 */

#ifndef _GPLUGMGR_MANAGER_H__
#define _GPLUGMGR_MANAGER_H__

#include "gplugin.h"

#ifdef _WIN32
    /**
     * The GPLUGMGR_EXPORT_SYMBOL macro is defined in the cmake file that
     * compiles the gplug library.
     */
    #if defined(GPLUGMGR_EXPORT_SYMBOL)
        #define GPLUGMGR_DLL_DECLARE extern "C" __declspec(dllexport)
    #else
        #define GPLUGMGR_DLL_DECLARE extern "C" __declspec(dllimport)
    #endif
    #define GPLUGMGR_API __stdcall
#elif defined(__linux__)
    #define GPLUGMGR_DLL_DECLARE extern "C"
    #define GPLUGMGR_API
#else
    #define GPLUGMGR_DLL_DECLARE
    #define GPLUGMGR_API
#endif

/*
 * 插件管理器类型、常量、错误码定义
 */
enum GPlugMgrErrorCode
{
    GPLUGMGR_OK                     = 0,                 /* 操作成功 */
    GPLUGMGR_ERR                    = -1,                /* 操作失败 */

    GPLUGMGR_ERROR_InvalidParam         = (0x8000 + 1),      /* 输入参数无效。                                                        */
    GPLUGMGR_ERROR_FileNotExist,       /* 插件库文件不存在。                                                                          */
    GPLUGMGR_ERROR_FkeyNotExist,       /* 插件不存在（插件功能标识不存在）。                                                          */
    GPLUGMGR_ERROR_IkeyNotExist,       /* 插件功能接口集不存在（插件功能接口集标识不存在）。                                          */
    GPLUGMGR_ERROR_InvalidPlugin,      /* 无效的插件（未按规则导出 Hplugin_GetPluginInterface 接口，或与插件管理器交互的接口集无效    */
    GPLUGMGR_ERROR_LoadDsoFailed,      /* 加载插件库失败。                                                                            */
    GPLUGMGR_ERROR_UnloadDsoFailed,    /* 卸载插件库失败。                                                                            */
    GPLUGMGR_ERROR_OutOfMemory,        /* 申请内存失败。                                                                              */
    GPLUGMGR_ERROR_ParseXmlFailed,     /* 解析配置文件失败。                                                                          */
    GPLUGMGR_ERROR_MemoryNotEnough,    /* 缓冲区空间不足。                                                                            */
    GPLUGMGR_ERROR_InitPluginFailed,   /* 初始化插件失败。                                                                            */
    GPLUGMGR_ERROR_UninitPluginFailed, /* 反初始化插件失败。                                                                          */
    GPLUGMGR_ERROR_PluginConflict,     /* 插件之间冲突（如插件库文件名相同、插件标识相同等）。                                        */
    GPLUGMGR_ERROR_InstanceConflict,   /* 插件实例冲突（存在插件实例相同）。                                                          */
    GPLUGMGR_ERROR_InvalidConfigFile,  /* 无效的配置文件                                                                              */
    GPLUGMGR_ERROR_NotSupport          /* not support */
};

/**
 * @brief      Initialize the plugin manager.
 * @return     Returns 0 on success, other values on failure, see @ref GPlugMgrErrorCode.
 */
GPLUGMGR_DLL_DECLARE int GPLUGMGR_API GPlugMgr_Init();

/**
 * @brief      Deinitialize the plugin manager.
 * @return     Returns 0 on success, other values on failure, see @ref GPlugMgrErrorCode.
 */
GPLUGMGR_DLL_DECLARE int GPLUGMGR_API GPlugMgr_Deinit();

/**
 * @brief      Create a plugin instance.
 * @param[in]  fkey : Plugin file identifier(file key)
 * @param[out] pInstance : If the creation is successful, it is used to return the plugin instance handle, otherwise it returns NULL.
 * @param[out] pluginError : The error code returned by the plugin when the creation of the plugin instance fails.
 * @return     Returns 0 on success, other values on failure, see @ref GPlugMgrErrorCode.
 */
GPLUGMGR_DLL_DECLARE int GPLUGMGR_API GPlugMgr_CreateInstance(const char* fkey, GPluginHandle* pInstance, int* pluginError);

/**
 * @brief      Destroys the plugin instance.
 * @param[in]  instance : Plugin instance handle.
 * @param[out] pluginError : The error code returned by the plugin when the creation of the plugin instance fails.
 * @return     Returns 0 on success, other values on failure, see @ref GPlugMgrErrorCode.
 */
GPLUGMGR_DLL_DECLARE int GPLUGMGR_API GPlugMgr_DestroyInstance(GPluginHandle instance, int* pluginError);

/**
 * @brief      Get the set of plugin function interfaces.
 * @param[in]  instance : Plugin instance handle.
 * @param[in]  ikey : Plugin function interface set identification.
 * @param[out] pluginInterface : Return the interface corresponding to ikey, If the interface is not implemented, return GPLUGIN_NOTSUPPORT
 * @param[out] pluginError : The error code returned by the plugin when the creation of the plugin instance fails.().
 * @return     Returns 0 on success, other values on failure, see @ref GPlugMgrErrorCode.
 */
GPLUGMGR_DLL_DECLARE int GPLUGMGR_API GPlugMgr_QueryInterface(GPluginHandle instance, const char* ikey, GPluginHandle* pluginInterface, int* pluginError);

/**
 * @brief      Query plugin configuration attribute.
 * @param[in]  fkey : Plugin file identifier(file key)
 * @param[in]  attributeName : Configuration parameter attribute name.
 * @param[out] attributeValue : Configuration parameter attribute value.
 * @param[in/out] buf_len : 配置属性缓冲区长度/实际value buf长度。
 * @return     Returns 0 on success, other values on failure, see @ref GPlugMgrErrorCode.
 */
GPLUGMGR_DLL_DECLARE int GPLUGMGR_API GPlugMgr_QueryConfigAttribute(const char* fkey, const char* attributeName, char* attributeValue, unsigned int* bufLen);

/**
 * @brief      Query the fkeys of all plugins.
 * @param[out] fkeys : Plugin file identifier(file key) array
 * @param[out] fkeysCount : Number of plugins(array size).
 * @return     Returns 0 on success, other values on failure, see @ref GPlugMgrErrorCode.
 */
GPLUGMGR_DLL_DECLARE int GPLUGMGR_API GPlugMgr_QueryAllFkeys(char*** fkeys, unsigned int* fkeysCount);

/**
 * @brief      Free plugin fkeys memory.
 * @param[in]  fkeys : Plugin file identifier(file key) array
 * @param[in]  fkeysCount : Number of plugins(array size).
 * @return     Returns 0 on success, other values on failure, see @ref GPlugMgrErrorCode.
 */
GPLUGMGR_DLL_DECLARE int GPLUGMGR_API GPlugMgr_ReleaseAllFkeys(char** fkeys, unsigned int fkeysCount);

#endif
