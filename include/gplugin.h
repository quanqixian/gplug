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
 * ������͡���������
 */
typedef void* GPluginHandle;        // ���ʵ�����;

/**
 * �ӿں�������ֵ����
 */
#define GPLUGIN_OK             (0)                   /* �ɹ�;           */
#define GPLUGIN_ERR            (-1)                  /* ʧ��;           */
#define GPLUGIN_NOTSUPPORT     (-2)                  /* ��֧��;         */
#define GPLUGIN_INVALID_HANDLE NULL                  /* ��������Чֵ; */

/**
 * �������߱������������������Ľӿ�
 */

/**
 *  @brief ��ʼ�����;
 *  @param void
 *  @return �ɹ�����0�����򷵻�����ֵ;
 *  @note �������������ȷ�������ظ����øýӿ�;
 */
typedef int (GPLUGIN_API * GPlugin_Init)();

/**
 *  @brief ����ʼ�����;
 *  @param void
 *  @return �ɹ�����0�����򷵻�����ֵ;
 *  @note �������������ȷ�������ظ����øýӿ�;
 */
typedef int (GPLUGIN_API * GPlugin_Uninit)();

/**
 *  @brief �������ʵ��;
 *  @param instance [out] �����ɹ��򷵻ز��ʵ����������򷵻�HPLUGIN_INVALID_HANDLE;
 *  @return �ɹ�����0�����򷵻�����ֵ;
 *  @note ���ʵ���豣֤�ýӿ��̰߳�ȫ;
 */
typedef int (GPLUGIN_API * GPlugin_CreateInstance)(GPluginHandle* instance);

/**
 *  @brief ���ٲ��ʵ��;
 *  @param instance [in] ���ʵ�����;
 *  @return �ɹ�����0�����򷵻�����ֵ;
 *  @note ���ʵ���豣֤�ýӿ��̰߳�ȫ;
 */
typedef int (GPLUGIN_API * GPlugin_DestroyInstance)(GPluginHandle instance);

/**
 *  @brief ��ȡ������ܽӿڼ�;
 *  @param instance [in] ���ʵ�����;
 *  @param ikey [in] ������ܽӿڼ���ʶ;
 *  @param plugin_interface [out] �����ʵ��ʵ�����������ܽӿڼ���ʶ��Ӧ�Ľӿڼ����򷵻ظýӿڼ���������򷵻�HPLUGIN_INVALID_HANDLE;
 *  @return �ɹ�����0�����򷵻�����ֵ����û��ʵ���������ܽӿڼ���ʶ��Ӧ�Ľӿڼ�������HPLUGIN_NOTSUPPORT��;
 *  @note ���ʵ���豣֤�ýӿ��̰߳�ȫ;
 */
typedef int (GPLUGIN_API * GPlugin_QueryInterface)(GPluginHandle instance, const char* ikey, GPluginHandle* plugin_interface);

/**
 *  @brief ��ȡ���֧�ֵĹ��ܽӿڼ���ʶ�б�;
 *  @param void
 *  @return ���ز��֧�ֵĹ��ܽӿڼ���ʶ�б��б�����һ���ַ���Ӧʼ��Ϊ��;
 *  @note ���ʵ���豣֤�ýӿ��̰߳�ȫ;
 */
typedef const char** (GPLUGIN_API * GPlugin_GetAllInterfaceIkeys)();

/**
 *  @brief ��ȡ������ļ��汾�ִ�;
 *  @param void
 *  @return ���ز�����ļ��汾�ִ�;
 *  @note ���ʵ���豣֤�ýӿ��̰߳�ȫ;
 */
typedef const char* (GPLUGIN_API * GPlugin_GetFileVersion)();


//----------- �������뵼���Ľӿ�; ------------//


/**
 *  @brief ������������������Ľӿڼ���;
 */
struct GPluginExportInterface
{
    GPlugin_Init Init;                                  ///< ��ʼ������ӿڣ���ѡ�ӿ�;
    GPlugin_Uninit Uninit;                              ///< ����ʼ������ӿڣ���ѡ�ӿ�;
    GPlugin_CreateInstance CreateInstance;              ///< �������ʵ���ӿڣ���ѡ�ӿ�;
    GPlugin_DestroyInstance DestroyInstance;            ///< ���ٲ��ʵ���ӿڣ���ѡ�ӿ�;
    GPlugin_QueryInterface QueryInterface;              ///< ��ȡ������ܽӿڼ��ӿڣ���ѡ�ӿ�;
    GPlugin_GetAllInterfaceIkeys GetAllInterfaceIkeys;  ///< ��ȡ���֧�ֵĹ��ܽӿڼ���ʶ�б�ӿڣ���ѡ�ӿ�;
    GPlugin_GetFileVersion GetFileVersion;              ///< ��ȡ������ļ��汾�ִ��ӿڣ���ѡ�ӿ�;

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
 *  @brief ��ȡ������������������Ľӿڼ���;
 *  @param void
 *  @return ���ز�����������������Ľӿڼ���;
 */
typedef const GPluginExportInterface* (GPLUGIN_API * GPlugin_GetPluginInterface)();


// ���øú����������ʵ�ֲ�����뵼���Ľӿ� GPLUGIN_GetPluginInterface;
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
