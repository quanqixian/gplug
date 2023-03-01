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
 * Interface function return value definition.
 */
#define GPLUGIN_OK             (0)                   /* success */
#define GPLUGIN_ERR            (-1)                  /* failure */
#define GPLUGIN_NOTSUPPORT     (-2)                  /* no support */
#define GPLUGIN_INVALID_HANDLE NULL                  /* invalid handle */

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
 * @param[in]  ikey : interface key(Plugin function interface identification)
 * @param[out] pluginInterface : If the plug-in instance implements the corresponding interface set, return the handle of the interface,
                otherwise return GPLUGIN_INVALID_HANDLE.
 * @return     Returns 0 on success, other values on failure.(If the corresponding interface is not implemented, return GPLUGIN_NOTSUPPORT)
 */
typedef int (GPLUGIN_API * GPlugin_QueryInterface)(GPluginHandle instance, const char* ikey, GPluginHandle* pluginInterface);

/**
 * @brief      Get all interface key of the plugin.
 * @return     Returns a list of feature interface set ids supported by the plugin, the last string of the list should always be empty.
 */
typedef const char** (GPLUGIN_API * GPlugin_GetAllInterfaceIkeys)();

/**
 * @brief      Get plugin version.
 * @return     Plugin version string.
 */
typedef const char* (GPLUGIN_API * GPlugin_GetFileVersion)();

/**
 * @brief Interface for plugins to interact with the plugin manager.
 */
typedef struct GPluginExportInterface
{
    GPlugin_Init Init;                                 /* Initialize the plugin */
    GPlugin_Uninit Uninit;                             /* deinitialization plugin */
    GPlugin_CreateInstance CreateInstance;             /* Create plugin instance */
    GPlugin_DestroyInstance DestroyInstance;           /* Destroy the plugin instance */
    GPlugin_QueryInterface QueryInterface;             /* Get the plugin function interface set */
    GPlugin_GetAllInterfaceIkeys GetAllInterfaceIkeys; /* Get a list of function interface set identifiers supported by the plugin */
    GPlugin_GetFileVersion GetFileVersion;             /* Get plugin version */

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
#endif

}GPluginExportInterface;

/**
 * @brief      Get a collection of interfaces for plugins to interact with the plugin manager
 * @return     Returns a collection of interfaces for plugins to interact with the plugin manager.
 */
typedef const GPluginExportInterface* (GPLUGIN_API * GPlugin_GetPluginInterface)();

/**
 * @def   GPLUGIN_MAKE_EXPORT_INTERFACE
 * @brief Call the macro GPLUGIN_MAKE_EXPORT_INTERFACE to declare and implement the interface that the plug-in must export
 */
#if defined(__cplusplus)
    #define GPLUGIN_MAKE_EXPORT_INTERFACE(Init, Uninit, CreateInstance, DestroyInstance, QueryInterface, GetAllInterfaceIkeys, GetFileVersion) \
        GPLUGIN_EXPORT const GPluginExportInterface* GPLUGIN_API GPLUGIN_GetPluginInterface() \
        { \
            static const GPluginExportInterface exportInterface(\
                Init,                 \
                Uninit,               \
                CreateInstance,       \
                DestroyInstance,      \
                QueryInterface,       \
                GetAllInterfaceIkeys, \
                GetFileVersion        \
            );                        \
            return &exportInterface;  \
        }
#else
    #define GPLUGIN_MAKE_EXPORT_INTERFACE(Init, Uninit, CreateInstance, DestroyInstance, QueryInterface, GetAllInterfaceIkeys, GetFileVersion) \
        GPLUGIN_EXPORT const GPluginExportInterface* GPLUGIN_API GPLUGIN_GetPluginInterface() \
        { \
            static const GPluginExportInterface exportInterface = {\
                Init,                 \
                Uninit,               \
                CreateInstance,       \
                DestroyInstance,      \
                QueryInterface,       \
                GetAllInterfaceIkeys, \
                GetFileVersion        \
            };                        \
            return &exportInterface;  \
        }
#endif

#endif
