#ifndef _GPLUGIN_H__
#define _GPLUGIN_H__

#include <stdio.h>

#if (defined(_WIN32) || defined(_WIN64))
    #define GPLUGIN_EXTERN extern "C" __declspec(dllexport)
    #define GPLUGIN_API __stdcall
#elif defined(__linux__)
    #define GPLUGIN_EXTERN extern "C"
    #define GPLUGIN_API
#else
    #define GPLUGIN_EXTERN
    #define GPLUGIN_API
#endif



/**
 * 插件类型、常量定义
 */
typedef void* GPluginHandle;        // 插件实例句柄;

/**
 * 接口函数返回值定义
 */
#define GPLUGIN_OK             (0)                   /* 成功;           */
#define GPLUGIN_ERR            (-1)                  /* 失败;           */
#define GPLUGIN_NOTSUPPORT     (-2)                  /* 不支持;         */
#define GPLUGIN_INVALID_HANDLE NULL                  /* 插件句柄无效值; */

/**
 * 插件必须具备的与插件管理器交互的接口
 */

/**
 *  @brief 初始化插件;
 *  @param void
 *  @return 成功返回0，否则返回其他值;
 *  @note 插件管理器可以确保不会重复调用该接口;
 */
typedef int (GPLUGIN_API * GPlugin_Init)();

/**
 *  @brief 反初始化插件;
 *  @param void
 *  @return 成功返回0，否则返回其他值;
 *  @note 插件管理器可以确保不会重复调用该接口;
 */
typedef int (GPLUGIN_API * GPlugin_Uninit)();

/**
 *  @brief 创建插件实例;
 *  @param instance [out] 创建成功则返回插件实例句柄，否则返回HPLUGIN_INVALID_HANDLE;
 *  @return 成功返回0，否则返回其他值;
 *  @note 插件实现需保证该接口线程安全;
 */
typedef int (GPLUGIN_API * GPlugin_CreateInstance)(GPluginHandle* instance);

/**
 *  @brief 销毁插件实例;
 *  @param instance [in] 插件实例句柄;
 *  @return 成功返回0，否则返回其他值;
 *  @note 插件实现需保证该接口线程安全;
 */
typedef int (GPLUGIN_API * GPlugin_DestroyInstance)(GPluginHandle instance);

/**
 *  @brief 获取插件功能接口集;
 *  @param instance [in] 插件实例句柄;
 *  @param ikey [in] 插件功能接口集标识;
 *  @param plugin_interface [out] 若插件实例实现了与插件功能接口集标识对应的接口集，则返回该接口集句柄，否则返回HPLUGIN_INVALID_HANDLE;
 *  @return 成功返回0，否则返回其他值（若没有实现与插件功能接口集标识对应的接口集，返回HPLUGIN_NOTSUPPORT）;
 *  @note 插件实现需保证该接口线程安全;
 */
typedef int (GPLUGIN_API * GPlugin_QueryInterface)(GPluginHandle instance, const char* ikey, GPluginHandle* plugin_interface);

/**
 *  @brief 获取插件支持的功能接口集标识列表;
 *  @param void
 *  @return 返回插件支持的功能接口集标识列表，列表的最后一个字符串应始终为空;
 *  @note 插件实现需保证该接口线程安全;
 */
typedef const char** (GPLUGIN_API * GPlugin_GetAllInterfaceIkeys)();

/**
 *  @brief 获取插件库文件版本字串;
 *  @param void
 *  @return 返回插件库文件版本字串;
 *  @note 插件实现需保证该接口线程安全;
 */
typedef const char* (GPLUGIN_API * GPlugin_GetFileVersion)();


//----------- 插件库必须导出的接口; ------------//


/**
 *  @brief 插件与插件管理器交互的接口集合;
 */
struct GPluginExportInterface
{
    GPlugin_Init Init;                                  ///< 初始化插件接口，必选接口;
    GPlugin_Uninit Uninit;                              ///< 反初始化插件接口，必选接口;
    GPlugin_CreateInstance CreateInstance;              ///< 创建插件实例接口，必选接口;
    GPlugin_DestroyInstance DestroyInstance;            ///< 销毁插件实例接口，必选接口;
    GPlugin_QueryInterface QueryInterface;              ///< 获取插件功能接口集接口，必选接口;
    GPlugin_GetAllInterfaceIkeys GetAllInterfaceIkeys;  ///< 获取插件支持的功能接口集标识列表接口，可选接口;
    GPlugin_GetFileVersion GetFileVersion;              ///< 获取插件库文件版本字串接口，可选接口;

#if defined(__cplusplus)
    GPluginExportInterface()
        : Init(NULL), Uninit(NULL), CreateInstance(NULL), DestroyInstance(NULL)
        , QueryInterface(NULL), GetAllInterfaceIkeys(NULL), GetFileVersion(NULL)
    {
    }

    GPluginExportInterface( GPlugin_Init init, GPlugin_Uninit uninit,
        GPlugin_CreateInstance create_instance, GPlugin_DestroyInstance destroy_instance,
        GPlugin_QueryInterface query_interface, GPlugin_GetAllInterfaceIkeys get_all_interface_ikeys,
        GPlugin_GetFileVersion get_file_version)
        : Init(init), Uninit(uninit), CreateInstance(create_instance)
        , DestroyInstance(destroy_instance) , QueryInterface(query_interface)
        , GetAllInterfaceIkeys(get_all_interface_ikeys), GetFileVersion(get_file_version)
    {
    }

    ~GPluginExportInterface()
    {
    }

#endif // #if defined(__cplusplus)

};

/** @fn typedef const GPluginExportInterface* (GPLUGIN_API * GPlugin_GetPluginInterface)()
 *  @brief 获取插件与插件管理器交互的接口集合;
 *  @param void
 *  @return 返回插件与插件管理器交互的接口集合;
 */
typedef const GPluginExportInterface* (GPLUGIN_API * GPlugin_GetPluginInterface)();


// 调用该宏可以声明并实现插件必须导出的接口 GPLUGIN_GetPluginInterface;
#if defined(__cplusplus)
    #define GPLUGIN_MAKE_EXPORT_INTERFACE(Init, Uninit, CreateInstance, DestroyInstance, QueryInterface, GetAllInterfaceIkeys, GetFileVersion) \
        GPLUGIN_EXTERN const GPluginExportInterface* GPLUGIN_API GPLUGIN_GetPluginInterface() \
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
        GPLUGIN_EXTERN const GPluginExportInterface* GPLUGIN_API GPLUGIN_GetPluginInterface() \
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
