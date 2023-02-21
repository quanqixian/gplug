#include "Debug.h"
#include "gplugMgr.h"
#include "GetTypeInterface.h"
#include "GetWeightInterface.h"

int main(int argc, const char *argv[])
{
    int ret = 0;

    ret = GPlugMgr_Init();
    if(0 != ret)
    {
        GPLUGMGR_LOG_ERROR(ret, "GPlugMgr_Init error");
        return ret;
    }

    char** fkeys = NULL;
    unsigned int fkeysCount = 0;
    ret = GPlugMgr_QueryAllFkeys(&fkeys, &fkeysCount);
    if(0 != ret)
    {
        GPLUGMGR_LOG_ERROR(0, "GPlugMgr_QueryAllFkeys error");
        return ret;
    }
    GPLUGMGR_LOG_INFO("fkeysCount=%d", fkeysCount);
    for(int i = 0; i < fkeysCount; i++)
    {
        GPLUGMGR_LOG_INFO("fkeys[%d]=%s", i, fkeys[i]);
        GPluginHandle instance = NULL;
        int errCode = 0;
        /* 创建插件实例 */
        ret = GPlugMgr_CreateInstance(fkeys[i], &instance, &errCode);
        if(0 != ret)
        {
            GPLUGMGR_LOG_ERROR(errCode, "GPlugMgr_CreateInstance error");
            break;
        }
        /*查询插件实例是否具有获取名字功能接口集 */
        {
            GPluginHandle interface = GPLUGIN_INVALID_HANDLE;
            ret = GPlugMgr_QueryInterface(instance, IKEY_IType, &interface, &errCode);
            if(0 != ret)
            {
                GPLUGMGR_LOG_ERROR(errCode, "GPlugMgr_QueryInterface error, ikey=%s", IKEY_IType);
                return ret;
            }

            /* 调用获取名字接口 */
            IGetTypeInterface * p = (IGetTypeInterface*) interface;
            GPLUGMGR_LOG_INFO("fkey=%s, type=%s", fkeys[i], p->type());
        }

        /*查询插件实例是否具有获取重量功能接口集 */
        {
            GPluginHandle interface = GPLUGIN_INVALID_HANDLE;
            ret = GPlugMgr_QueryInterface(instance, IKEY_IWeight, &interface, &errCode);
            if(0 != ret)
            {
                GPLUGMGR_LOG_ERROR(errCode, "GPlugMgr_QueryInterface error, ikey=%s", IKEY_IWeight);
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
            ret = GPlugMgr_QueryConfigAttribute(fkeys[i], "file", attrBuf, &attrLen);
            if(0 != ret)
            {
                GPLUGMGR_LOG_ERROR(ret, "GPlugMgr_QueryConfigAttribute error");
                return ret;
            }
            GPLUGMGR_LOG_INFO("file=%s", attrBuf);
        }

        ret = GPlugMgr_DestroyInstance(instance, &errCode);
        if(0 != ret)
        {
            GPLUGMGR_LOG_ERROR(0, "GPlugMgr_DestroyInstance error");
            break;
        }
    }

    ret = GPlugMgr_ReleaseAllFkeys(fkeys, fkeysCount);
    if(0 != ret)
    {
        GPLUGMGR_LOG_ERROR(ret, "GPlugMgr_ReleaseAllFkeys error");
    }

    GPlugMgr_Deinit();

    return 0;
}
