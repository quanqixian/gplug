#include "gtest/gtest.h"
#include "Debug.h"
#include "gplugMgr.h"
/**
 * @brief test class HttpReq
 */
TEST(GPlugMgr, GPlugMgr_Init)
{
    int ret = 0;
    ret = GPlugMgr_Init();
    EXPECT_EQ(ret, 0);

    ret = GPlugMgr_Init();
    EXPECT_NE(ret, 0);

    GPlugMgr_Deinit();

    ret = GPlugMgr_Init();
    EXPECT_EQ(ret, 0);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
