#include "Debug.h"
#include "gplug.h"
#include "GetTypeInterface.h"
#include "GetWeightInterface.h"

int main(int argc, const char *argv[])
{
    int ret = 0;

    GPLUG_Init();

    char** fkeys = NULL;
    unsigned int fkeysCout = 0;
    ret = GPLUG_QueryAllFkeys(&fkeys, &fkeysCout);
    if(0 != ret)
    {
        GPLUG_LOG_ERROR(0, "GPLUG_QueryAllFkeys error");
        return ret;
    }
    GPLUG_LOG_INFO("fkeysCout=%d", fkeysCout);
    for(int i = 0; i < fkeysCout; i++)
    {
        GPLUG_LOG_INFO("fkeys[%d]=%s", i, fkeys[i]);
        GPluginHandle instance = NULL;
        int errCode = 0;
        /* 创建插件实例 */
        ret = GPLUG_CreateInstance(fkeys[i], &instance, &errCode);
        if(0 != ret)
        {
            GPLUG_LOG_ERROR(errCode, "GPLUG_CreateInstance error");
            break;
        }
        /*查询插件实例是否具有获取名字功能接口集 */
        {
            GPluginHandle interface = GPLUGIN_INVALID_HANDLE;
            ret = GPLUG_QueryInterface(instance, IKEY_IType, &interface, &errCode);
            if(0 != ret)
            {
                GPLUG_LOG_ERROR(errCode, "GPLUG_QueryInterface error, ikey=%s", IKEY_IType);
                return ret;
            }

            /* 调用获取名字接口 */
            IGetTypeInterface * p = (IGetTypeInterface*) interface;
            GPLUG_LOG_INFO("fkey=%s, type=%s", fkeys[i], p->type().c_str());
        }

        /*查询插件实例是否具有获取重量功能接口集 */
        {
            GPluginHandle interface = GPLUGIN_INVALID_HANDLE;
            ret = GPLUG_QueryInterface(instance, IKEY_IWeight, &interface, &errCode);
            if(0 != ret)
            {
                GPLUG_LOG_ERROR(errCode, "GPLUG_QueryInterface error, ikey=%s", IKEY_IWeight);
                return ret;
            }

            /* 调用获取名字接口 */
            IGetWeightInterface * p = (IGetWeightInterface*) interface;
            GPLUG_LOG_INFO("fkey=%s, weight=%s", fkeys[i], p->weight().c_str());
        }

        ret = GPLUG_DestroyInstance(instance, &errCode);
        if(0 != ret)
        {
            GPLUG_LOG_ERROR(0, "GPLUG_DestroyInstance error");
            break;
        }
    }

    GPLUG_ReleaseAllFkeys(fkeys, fkeysCout);

    GPLUG_Uninit();
    return 0;
}
