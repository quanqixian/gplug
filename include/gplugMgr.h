/**
 * @file gplugMgr.h
 * 
 * Plugin Manager
 */

#ifndef _GPLUGMGR_MANAGER_H__
#define _GPLUGMGR_MANAGER_H__

#include "gplugin.h"

#if (defined(_WIN32) || defined(_WIN64))
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
    GPLUGMGR_E_NotSupport          /* 不支持。                                                                                    */
};

/**
 * @brief      初始化插件管理器。
 * @return     成功返回0，失败返回其他值，参见 GPlugErrorCode。
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_Init();

/**
 * @brief      反初始化插件管理器。
 * @return     void
 */
GPLUGMGR_EXPORT void GPLUGMGR_API GPLUGMGR_Uninit();

/**
 * @brief      创建插件实例。
 * @param[in]  fkey : 插件文件标识
 * @param[out] pInstance : 创建成功则用于返回插件实例句柄，否则返回HPLUGIN_INVALID_HANDLE。
 * @param[out] plugin_error : 创建插件实例失败时，插件返回的错误码。
 * @return     成功返回0，失败返回其他值，参见 GPlugErrorCode。
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_CreateInstance(const char* fkey, GPluginHandle* pInstance, int* plugin_error);

/**
 * @brief      销毁插件实例。
 * @param[in]  instance : 插件实例句柄。
 * @param[out] plugin_error : 创建插件实例失败时，插件返回的错误码。
 * @return     成功返回0，失败返回其他值，参见 GPlugErrorCode。
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_DestroyInstance(GPluginHandle instance, int* plugin_error);

/**
 * @brief 获取插件功能接口集。
 * @param[in]  instance : 插件实例句柄。
 * @param[in]  ikey : 插件功能接口集标识。
 * @param[out] plugin_interface : 若插件实例实现了与插件功能接口集标识对应的接口集，则返回该接口集句柄，否则返回HPLUGIN_INVALID_HANDLE。
 * @param[out] plugin_error : 获取失败时，插件返回的错误码（若没有实现与插件功能接口集标识对应的接口集，返回HPLUGIN_NOTSUPPORT）。
 * @return     成功返回0，否则返回其他值。
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_QueryInterface(GPluginHandle instance, const char* ikey, GPluginHandle* plugin_interface, int* plugin_error);

/**
 * @brief      查询插件配置属性。
 * @param[in]  fkey : 插件文件标识
 * @param[in]  attribute_name : 配置属性名称。
 * @param[out] attribute_value : 配置属性值, 若为NULL，则接口只在buf_len中返回实际长度。
 * @param[in/out] buf_len : 配置属性缓冲区长度/实际value字符串长度。
 * @return     成功返回0，否则返回其他值。
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_QueryConfigAttribute(const char* fkey, const char* attributeName, char* attributeValue, unsigned int* bufLen);

/**
 * @brief 查询所有插件，
 * @param[out] fkeys : 插件集。
 * @param[out] fkeysCout : 插件数量。
 * @return     成功返回0，否则返回其他值。
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_QueryAllFkeys(char*** fkeys, unsigned int* fkeysCout);

/**
 * @brief 释放查询所有插件内存。
 * @param[in]  fkeys : 插件集。
 * @param[in]  fkeysCout : 插件数量。
 * @return 成功返回0，否则返回其他值。
 */
GPLUGMGR_EXPORT int GPLUGMGR_API GPLUGMGR_ReleaseAllFkeys(char** fkeys, unsigned int fkeysCout);

#endif
