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
    GPLUGMGR_OK                       = 0,   /* success */
    GPLUGMGR_ERR                      = -1,  /* failed */
    GPLUGMGR_ERROR_InvalidParam       = -2,  /* Invalid parameter. */
    GPLUGMGR_ERROR_FileNotExist       = -3,  /* Plugin file does not exist. */
    GPLUGMGR_ERROR_FkeyNotExist       = -4,  /* File key not exist */
    GPLUGMGR_ERROR_IkeyNotExist       = -5,  /* Interface key not exist */
    GPLUGMGR_ERROR_InvalidPlugin      = -6,  /* Invalid plugin (the GPLUGIN_GetPluginInterface interface is not exported or any function GPLUGIN_GetPluginInterface is invalid)*/
    GPLUGMGR_ERROR_LoadDsoFailed      = -7,  /* Failed to load plugin library */
    GPLUGMGR_ERROR_UnloadDsoFailed    = -8,  /* Failed to unload plugin library */
    GPLUGMGR_ERROR_OutOfMemory        = -9,  /* Fail to allocate memory */
    GPLUGMGR_ERROR_ParseXmlFailed     = -10, /* Failed to parse configuration file. */
    GPLUGMGR_ERROR_MemoryNotEnough    = -11, /* Not enough buffer space */
    GPLUGMGR_ERROR_InitPluginFailed   = -12, /* Failed to initialize plugin */
    GPLUGMGR_ERROR_UninitPluginFailed = -13, /* Deinitialization plugin failed */
    GPLUGMGR_ERROR_PluginConflict     = -14, /* Conflicts between plugins (such as the same plugin library file name same FKey etc.).*/
    GPLUGMGR_ERROR_InvalidConfigFile  = -15, /* Invalid configuration file. */
    GPLUGMGR_ERROR_NotSupport         = -16, /* not support */
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
 * @param[in,out] bufLen : The length of the buffer area, which is also used to store the length of the returned data.
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
