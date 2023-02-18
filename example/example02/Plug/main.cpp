#include "Debug.h"
#include "gplugMgr.h"
#include "GetTypeInterface.h"
#include "GetWeightInterface.h"

int main(int argc, const char *argv[])
{
    int ret = 0;

    ret = GPLUGMGR_Init();
    if(0 != ret)
    {
        GPLUGMGR_LOG_ERROR(ret, "GPLUGMGR_Init error");
        return ret;
    }

    char** fkeys = NULL;
    unsigned int fkeysCout = 0;
    ret = GPLUGMGR_QueryAllFkeys(&fkeys, &fkeysCout);
    if(0 != ret)
    {
        GPLUGMGR_LOG_ERROR(0, "GPLUGMGR_QueryAllFkeys error");
        return ret;
    }
    GPLUGMGR_LOG_INFO("fkeysCout=%d", fkeysCout);
    for(int i = 0; i < fkeysCout; i++)
    {
        GPLUGMGR_LOG_INFO("fkeys[%d]=%s", i, fkeys[i]);
        GPluginHandle instance = NULL;
        int errCode = 0;
        /* 创建插件实例 */
        ret = GPLUGMGR_CreateInstance(fkeys[i], &instance, &errCode);
        if(0 != ret)
        {
            GPLUGMGR_LOG_ERROR(errCode, "GPLUGMGR_CreateInstance error");
            break;
        }
        /*查询插件实例是否具有获取名字功能接口集 */
        {
            GPluginHandle interface = GPLUGIN_INVALID_HANDLE;
            ret = GPLUGMGR_QueryInterface(instance, IKEY_IType, &interface, &errCode);
            if(0 != ret)
            {
                GPLUGMGR_LOG_ERROR(errCode, "GPLUGMGR_QueryInterface error, ikey=%s", IKEY_IType);
                return ret;
            }

            /* 调用获取名字接口 */
            IGetTypeInterface * p = (IGetTypeInterface*) interface;
            GPLUGMGR_LOG_INFO("fkey=%s, type=%s", fkeys[i], p->type());
        }

        /*查询插件实例是否具有获取重量功能接口集 */
        {
            GPluginHandle interface = GPLUGIN_INVALID_HANDLE;
            ret = GPLUGMGR_QueryInterface(instance, IKEY_IWeight, &interface, &errCode);
            if(0 != ret)
            {
                GPLUGMGR_LOG_ERROR(errCode, "GPLUGMGR_QueryInterface error, ikey=%s", IKEY_IWeight);
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
            ret = GPLUGMGR_QueryConfigAttribute(fkeys[i], "file", attrBuf, &attrLen);
            if(0 != ret)
            {
                GPLUGMGR_LOG_ERROR(ret, "GPLUGMGR_QueryConfigAttribute error");
                return ret;
            }
            GPLUGMGR_LOG_INFO("file=%s", attrBuf);
        }

        ret = GPLUGMGR_DestroyInstance(instance, &errCode);
        if(0 != ret)
        {
            GPLUGMGR_LOG_ERROR(0, "GPLUGMGR_DestroyInstance error");
            break;
        }
    }

    ret = GPLUGMGR_ReleaseAllFkeys(fkeys, fkeysCout);
    if(0 != ret)
    {
        GPLUGMGR_LOG_ERROR(ret, "GPLUGMGR_ReleaseAllFkeys error");
    }

    GPLUGMGR_Uninit();

    return 0;
}
