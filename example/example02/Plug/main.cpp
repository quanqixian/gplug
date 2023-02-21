#include "Debug.h"
#include "gplugMgr.h"
#include "GetTypeInterface.h"
#include "GetWeightInterface.h"

int main(int argc, const char *argv[])
{
    int ret = 0;

    ret = GPluginMgr_Init();
    if(0 != ret)
    {
        GPLUGMGR_LOG_ERROR(ret, "GPluginMgr_Init error");
        return ret;
    }

    char** fkeys = NULL;
    unsigned int fkeysCount = 0;
    ret = GPluginMgr_QueryAllFkeys(&fkeys, &fkeysCount);
    if(0 != ret)
    {
        GPLUGMGR_LOG_ERROR(0, "GPluginMgr_QueryAllFkeys error");
        return ret;
    }
    GPLUGMGR_LOG_INFO("fkeysCount=%d", fkeysCount);
    for(int i = 0; i < fkeysCount; i++)
    {
        GPLUGMGR_LOG_INFO("fkeys[%d]=%s", i, fkeys[i]);
        GPluginHandle instance = NULL;
        int errCode = 0;
        /* 创建插件实例 */
        ret = GPluginMgr_CreateInstance(fkeys[i], &instance, &errCode);
        if(0 != ret)
        {
            GPLUGMGR_LOG_ERROR(errCode, "GPluginMgr_CreateInstance error");
            break;
        }
        /*查询插件实例是否具有获取名字功能接口集 */
        {
            GPluginHandle interface = GPLUGIN_INVALID_HANDLE;
            ret = GPluginMgr_QueryInterface(instance, IKEY_IType, &interface, &errCode);
            if(0 != ret)
            {
                GPLUGMGR_LOG_ERROR(errCode, "GPluginMgr_QueryInterface error, ikey=%s", IKEY_IType);
                return ret;
            }

            /* 调用获取名字接口 */
            IGetTypeInterface * p = (IGetTypeInterface*) interface;
            GPLUGMGR_LOG_INFO("fkey=%s, type=%s", fkeys[i], p->type());
        }

        /*查询插件实例是否具有获取重量功能接口集 */
        {
            GPluginHandle interface = GPLUGIN_INVALID_HANDLE;
            ret = GPluginMgr_QueryInterface(instance, IKEY_IWeight, &interface, &errCode);
            if(0 != ret)
            {
                GPLUGMGR_LOG_ERROR(errCode, "GPluginMgr_QueryInterface error, ikey=%s", IKEY_IWeight);
                return ret;
            }

            /* 调用获取名字接口 */
            IGetWeightInterface * p = (IGetWeightInterface*) interface;
            GPLUGMGR_LOG_INFO("fkey=%s, weight=%s", fkeys[i], p->weight());
        }

        /* 查询插件属性 */
        {
            char attrBuf[256] = {0};
            unsigned int attrLen = sizeof(attrBuf);
            ret = GPluginMgr_QueryConfigAttribute(fkeys[i], "file", attrBuf, &attrLen);
            if(0 != ret)
            {
                GPLUGMGR_LOG_ERROR(ret, "GPluginMgr_QueryConfigAttribute error");
                return ret;
            }
            GPLUGMGR_LOG_INFO("file=%s", attrBuf);
        }

        ret = GPluginMgr_DestroyInstance(instance, &errCode);
        if(0 != ret)
        {
            GPLUGMGR_LOG_ERROR(0, "GPluginMgr_DestroyInstance error");
            break;
        }
    }

    ret = GPluginMgr_ReleaseAllFkeys(fkeys, fkeysCount);
    if(0 != ret)
    {
        GPLUGMGR_LOG_ERROR(ret, "GPluginMgr_ReleaseAllFkeys error");
    }

    GPluginMgr_Deinit();

    return 0;
}
