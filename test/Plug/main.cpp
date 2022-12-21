#include "Debug.h"
#include "gplug.h"

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
        ret = GPLUG_CreateInstance(fkeys[i], &instance, &errCode);
        if(0 != ret)
        {
            GPLUG_LOG_ERROR(0, "GPLUG_CreateInstance error");
            break;
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
