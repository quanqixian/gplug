#ifndef _GPLUG_H__
#define _GPLUG_H__

#include "gplugin.h"

#if (defined(_WIN32) || defined(_WIN64))
    #if defined(GPLUG_EXPORTS)
        #define GPLUG_EXTERN extern "C" __declspec(dllexport)
    #else
        #define GPLUG_EXTERN extern "C" __declspec(dllimport)
    #endif
    #define GPLUG_API __stdcall
#elif defined(__linux__)
    #define GPLUG_EXTERN extern "C"
    #define GPLUG_API
#else
    #define GPLUG_EXTERN
    #define GPLUG_API
#endif

/*
 * 插件管理器类型、常量、错误码定义
 */
#define GPLUG_OK                    (0)                   /* 操作成功 */
#define GPLUG_ERR                   (-1)                  /* 操作失败 */
#define GPLUG_E_Base                ((int)0x80000000)     /* 错误码基数0x80000000, 错误码最高位为1。                                                     */
#define GPLUG_E_InvalidParam        (GPLUG_E_Base + 1)    /* 输入参数无效。                                                                              */
#define GPLUG_E_FileNotExist        (GPLUG_E_Base + 2)    /* 插件库文件不存在。                                                                          */
#define GPLUG_E_FkeyNotExist        (GPLUG_E_Base + 3)    /* 插件不存在（插件功能标识不存在）。                                                          */
#define GPLUG_E_IkeyNotExist        (GPLUG_E_Base + 4)    /* 插件功能接口集不存在（插件功能接口集标识不存在）。                                          */
#define GPLUG_E_InvalidPlugin       (GPLUG_E_Base + 5)    /* 无效的插件（未按规则导出 Hplugin_GetPluginInterface 接口，或与插件管理器交互的接口集无效）。*/
#define GPLUG_E_LoadDsoFailed       (GPLUG_E_Base + 6)    /* 加载插件库失败。                                                                            */
#define GPLUG_E_UnloadDsoFailed     (GPLUG_E_Base + 7)    /* 卸载插件库失败。                                                                            */
#define GPLUG_E_OutOfMemory         (GPLUG_E_Base + 8)    /* 申请内存失败。                                                                              */
#define GPLUG_E_ParseXmlFailed      (GPLUG_E_Base + 9)    /* 解析配置文件失败。                                                                          */
#define GPLUG_E_MemoryNotEnough     (GPLUG_E_Base + 10)   /* 缓冲区空间不足。                                                                            */
#define GPLUG_E_InitPluginFailed    (GPLUG_E_Base + 11)   /* 初始化插件失败。                                                                            */
#define GPLUG_E_UninitPluginFailed  (GPLUG_E_Base + 12)   /* 反初始化插件失败。                                                                          */
#define GPLUG_E_PluginConflict      (GPLUG_E_Base + 13)   /* 插件之间冲突（如插件库文件名相同、插件标识相同等）。                                        */
#define GPLUG_E_InstanceConflict    (GPLUG_E_Base + 14)   /* 插件实例冲突（存在插件实例相同）。                                                          */
#define GPLUG_E_NotSupport          (GPLUG_E_Base + 1000) /* 不支持。                                                                                    */

/**
 *  @brief 插件功能标识集合
 */
struct GPLUG_FKEYS
{
    const char** fkeys; /* 插件功能标识数组 */
    int count;          /* 插件功能标识数组长度 */

    GPLUG_FKEYS()
        : fkeys(NULL), count(0)
    {
    }
};


/**
 * 插件管理器基本接口
 */

/**
 *  @brief 初始化插件管理器。
 *  @return 成功返回0，失败返回其他值。
 */
GPLUG_EXTERN int GPLUG_API GPLUG_Init();

/**
 *  @brief 反初始化插件管理器。
 *  @return void
 */
GPLUG_EXTERN void GPLUG_API GPLUG_Uninit();

/**
 *  @brief 创建插件实例。
 *  @param fkey [in] 插件标识。
 *  @param instance [out] 创建成功则返回插件实例句柄，否则返回HPLUGIN_INVALID_HANDLE。
 *  @param plugin_error [out] 创建失败时，插件返回的错误码。
 *  @return 成功返回0，否则返回其他值。
 */
GPLUG_EXTERN int GPLUG_API GPLUG_CreateInstance(const char* fkey, GPluginHandle* instance, int* plugin_error);

/**
 *  @brief 销毁插件实例。
 *  @param instance [in] 插件实例句柄。
 *  @param plugin_error [out] 销毁失败时，插件返回的错误码。
 *  @return 成功返回0，否则返回其他值。
 */
GPLUG_EXTERN int GPLUG_API GPLUG_DestroyInstance(GPluginHandle instance, int* plugin_error);

/**
 *  @brief 获取插件功能接口集。
 *  @param instance [in] 插件实例句柄。
 *  @param ikey [in] 插件功能接口集标识。
 *  @param plugin_interface [out] 若插件实例实现了与插件功能接口集标识对应的接口集，则返回该接口集句柄，否则返回HPLUGIN_INVALID_HANDLE。
 *  @param plugin_error [out] 获取失败时，插件返回的错误码（若没有实现与插件功能接口集标识对应的接口集，返回HPLUGIN_NOTSUPPORT）。
 *  @return 成功返回0，否则返回其他值。
 */
GPLUG_EXTERN int GPLUG_API GPLUG_QueryInterface(GPluginHandle instance, const char* ikey, GPluginHandle* plugin_interface, int* plugin_error);

/**
 *  @brief 查询插件配置属性。
 *  @param fkey [in] 插件功能接口集标识。
 *  @param attribute_name [in] 配置属性名称。
 *  @param attribute_value [out] 配置属性值, 若为NULL，则接口只在buf_len中返回实际长度。
 *  @param buf_len [in/out] 配置属性缓冲区长度/实际value字符串长度。
 *  @return 成功返回0，否则返回其他值。
 */
GPLUG_EXTERN int GPLUG_API GPLUG_QueryConfigAttribute(const char* fkey, const char* attributeName, char* attributeValue, unsigned int* bufLen);

/**
 *  @brief 查询所有插件，仅包含hplugin.xml中配置的fkeys列表，不包含插件内部生成的映射fkeys。
 *  @param fkeys [out] 插件集。
 *  @param fkeysCout [out] 插件数量。
 *  @return 成功返回0，否则返回其他值。
 */
GPLUG_EXTERN int GPLUG_API GPLUG_QueryAllFkeys(char*** fkeys, unsigned int* fkeysCout);

/**
 *  @brief 释放查询所有插件内存。
 *  @param fkeys [in] 插件集。
 *  @param fkeysCout [in] 插件数量。
 *  @return 成功返回0，否则返回其他值。
 */
GPLUG_EXTERN int GPLUG_API GPLUG_ReleaseAllFkeys(char** fkeys, unsigned int fkeysCout);


#endif
