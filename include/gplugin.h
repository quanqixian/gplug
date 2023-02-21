/**
 * @file gplugin.h
 * 
 * plugin
 */

#ifndef _GPLUGIN_H__
#define _GPLUGIN_H__

#include <stdio.h>

#ifdef  _WIN32
    #ifdef __cplusplus
        #define GPLUGIN_EXPORT extern "C" __declspec(dllexport)
    #else
        #define GPLUGIN_EXPORT __declspec(dllexport)
    #endif
    #define GPLUGIN_API __stdcall
#elif defined(__linux__)
    #ifdef __cplusplus
        #define GPLUGIN_EXPORT extern "C"
    #else
        #define GPLUGIN_EXPORT
    #endif
    #define GPLUGIN_API
#else
    #define GPLUGIN_EXPORT
    #define GPLUGIN_API
#endif

/**
 * 接口函数返回值定义
 */
#define GPLUGIN_OK             (0)                   /* 成功;           */
#define GPLUGIN_ERR            (-1)                  /* 失败;           */
#define GPLUGIN_NOTSUPPORT     (-2)                  /* 不支持;         */
#define GPLUGIN_INVALID_HANDLE NULL                  /* 插件句柄无效值; */

/**
 * Plugin instance handle type definition.
 */
typedef void * GPluginHandle;

/**
 * @brief      Initialize the plugin. The plugin manager will not repeatedly call this interface of one plugin.
 * @return     Returns 0 on success, other values on failure.
 */
typedef int (GPLUGIN_API * GPlugin_Init)();

/**
 * @brief      Deinitialization plugin. The plugin manager will not repeatedly call this interface of one plugin.
 * @return     Returns 0 on success, other values on failure.
 */
typedef int (GPLUGIN_API * GPlugin_Uninit)();

/**
 * @brief      Create a plugin instance.
 * @param[out] instance : If the creation is successful, the plugin instance handle will be returned, otherwise NULL will be returned
 * @return     Returns 0 on success, other values on failure.
 */
typedef int (GPLUGIN_API * GPlugin_CreateInstance)(GPluginHandle* instance);

/**
 * @brief      Destroy the plugin instance.
 * @param[in]  instance : Plugin instance handle
 * @return     Returns 0 on success, other values on failure.
 */
typedef int (GPLUGIN_API * GPlugin_DestroyInstance)(GPluginHandle instance);

/**
 * @brief      Get the plugin function interface through ikey
 * @param[in]  instance : Plugin instance handle
 * @param[in]  ikey : Plugin function interface identification(interface key)
 * @param plugin_interface [out] 若插件实例实现了与插件功能接口集标识对应的接口集，则返回该接口集句柄，否则返回HPLUGIN_INVALID_HANDLE;
 * @return 成功返回0，否则返回其他值（若没有实现与插件功能接口集标识对应的接口集，返回HPLUGIN_NOTSUPPORT）;
 * @note 插件实现需保证该接口线程安全;
 */
typedef int (GPLUGIN_API * GPlugin_QueryInterface)(GPluginHandle instance, const char* ikey, GPluginHandle* plugin_interface);

/**
 * @brief 获取插件支持的功能接口集标识列表;
 * @return 返回插件支持的功能接口集标识列表，列表的最后一个字符串应始终为空;
 * @note 插件实现需保证该接口线程安全;
 */
typedef const char** (GPLUGIN_API * GPlugin_GetAllInterfaceIkeys)();

/**
 * @brief 获取插件库文件版本字串;
 * @return 返回插件库文件版本字串;
 * @note 插件实现需保证该接口线程安全;
 */
typedef const char* (GPLUGIN_API * GPlugin_GetFileVersion)();


//----------- 插件库必须导出的接口; ------------//
/**
 * @brief 插件与插件管理器交互的接口集合
 */
typedef struct GPluginExportInterface
{
    GPlugin_Init Init;                                  ///< 初始化插件接口，必选接口;
    GPlugin_Uninit Uninit;                              ///< 反初始化插件接口，必选接口;
    GPlugin_CreateInstance CreateInstance;              ///< 创建插件实例接口，必选接口;
    GPlugin_DestroyInstance DestroyInstance;            ///< 销毁插件实例接口，必选接口;
    GPlugin_QueryInterface QueryInterface;              ///< 获取插件功能接口集接口，必选接口;
    GPlugin_GetAllInterfaceIkeys GetAllInterfaceIkeys;  ///< 获取插件支持的功能接口集标识列表接口，可选接口;
    GPlugin_GetFileVersion GetFileVersion;              ///< 获取插件库文件版本字串接口，可选接口;

#ifdef __cplusplus
    GPluginExportInterface()
        : Init(NULL), Uninit(NULL), CreateInstance(NULL), DestroyInstance(NULL), 
        QueryInterface(NULL), GetAllInterfaceIkeys(NULL), GetFileVersion(NULL)
    {
    }

    GPluginExportInterface( GPlugin_Init init, GPlugin_Uninit uninit,
        GPlugin_CreateInstance create_instance, GPlugin_DestroyInstance destroy_instance,
        GPlugin_QueryInterface query_interface, GPlugin_GetAllInterfaceIkeys get_all_interface_ikeys,
        GPlugin_GetFileVersion get_file_version)
        : Init(init), Uninit(uninit), CreateInstance(create_instance), 
        DestroyInstance(destroy_instance), QueryInterface(query_interface),
        GetAllInterfaceIkeys(get_all_interface_ikeys), GetFileVersion(get_file_version)
    {
    }

    ~GPluginExportInterface()
    {
    }

#endif // #if defined(__cplusplus)

}GPluginExportInterface;

/**
 * @brief 获取插件与插件管理器交互的接口集合;
 * @return 返回插件与插件管理器交互的接口集合;
 */
typedef const GPluginExportInterface* (GPLUGIN_API * GPlugin_GetPluginInterface)();


// 调用该宏可以声明并实现插件必须导出的接口 GPLUGIN_GetPluginInterface;
#if defined(__cplusplus)
    #define GPLUGIN_MAKE_EXPORT_INTERFACE(Init, Uninit, CreateInstance, DestroyInstance, QueryInterface, GetAllInterfaceIkeys, GetFileVersion) \
        GPLUGIN_EXPORT const GPluginExportInterface* GPLUGIN_API GPLUGIN_GetPluginInterface() \
    { \
        static const GPluginExportInterface export_interface( \
        Init, \
        Uninit, \
        CreateInstance, \
        DestroyInstance, \
        QueryInterface, \
        GetAllInterfaceIkeys, \
        GetFileVersion \
        ); \
        return &export_interface; \
    }
#else
    #define GPLUGIN_MAKE_EXPORT_INTERFACE(Init, Uninit, CreateInstance, DestroyInstance, QueryInterface, GetAllInterfaceIkeys, GetFileVersion) \
        GPLUGIN_EXPORT const GPluginExportInterface* GPLUGIN_API GPLUGIN_GetPluginInterface() \
    { \
        static const GPluginExportInterface export_interface = { \
        Init, \
        Uninit, \
        CreateInstance, \
        DestroyInstance, \
        QueryInterface, \
        GetAllInterfaceIkeys, \
        GetFileVersion \
    }; \
        return &export_interface; \
}

#endif // #if defined(__cplusplus)

#endif
