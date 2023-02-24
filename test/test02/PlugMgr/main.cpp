#include "gtest/gtest.h"
#include "Debug.h"
#include "gplugMgr.h"

/**
 * @brief test GPlugMgr_Init
 */
TEST(GPlugMgr, GPlugMgr_Init)
{
    int ret = 0;
    /* can not init muti times. */
    ret = GPlugMgr_Init();
    EXPECT_EQ(ret, 0);

    ret = GPlugMgr_Init();
    EXPECT_NE(ret, 0);

    /* after deinit, you can init again */
    ret = GPlugMgr_Deinit();
    EXPECT_EQ(ret, 0);

    ret = GPlugMgr_Init();
    EXPECT_EQ(ret, 0);

    ret = GPlugMgr_Deinit();
    EXPECT_EQ(ret, 0);
}

/**
 * @brief test GPlugMgr_Deinit
 */
TEST(GPlugMgr, GPlugMgr_Deinit)
{
    int ret = 0;
    ret = GPlugMgr_Deinit();
    EXPECT_NE(ret, 0);

    ret = GPlugMgr_Init();
    EXPECT_EQ(ret, 0);

    ret = GPlugMgr_Deinit();
    EXPECT_EQ(ret, 0);
}

/**
 * @brief test GPlugMgr_QueryAllFkeys
 */
TEST(GPlugMgr, GPlugMgr_QueryAllFkeys)
{
    int ret = 0;
    char** fkeys = NULL;
    unsigned int fkeysCount = 0;

    ret = GPlugMgr_QueryAllFkeys(&fkeys, &fkeysCount);
    EXPECT_NE(ret, 0);

    ret = GPlugMgr_Init();
    EXPECT_EQ(ret, 0);

    ret = GPlugMgr_QueryAllFkeys(&fkeys, &fkeysCount);
    EXPECT_EQ(ret, 0);

    EXPECT_EQ(fkeysCount, 2);

    ret = GPlugMgr_ReleaseAllFkeys(fkeys, fkeysCount);
    EXPECT_EQ(ret, 0);

    ret = GPlugMgr_Deinit();
    EXPECT_EQ(ret, 0);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
