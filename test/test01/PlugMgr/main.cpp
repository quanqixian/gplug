#include "Debug.h"
#include "gplugMgr.h"

int main(int argc, const char *argv[])
{
    int ret = 0;

    ret = GPlugMgr_Init();
    if(0 != ret)
    {
        GPLUGMGR_LOG_ERROR(ret, "GPlugMgr_Init error");
        return ret;
    }

    GPlugMgr_Deinit();

    return 0;
}
