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

    ret = GPlugMgr_QueryAllFkeys(NULL, &fkeysCount);
    EXPECT_NE(ret, 0);

    ret = GPlugMgr_QueryAllFkeys(&fkeys, NULL);
    EXPECT_NE(ret, 0);

    ret = GPlugMgr_Deinit();
    EXPECT_EQ(ret, 0);
}

/**
 * @brief test GPlugMgr_ReleaseAllFkeys
 */
TEST(GPlugMgr, GPlugMgr_ReleaseAllFkeys)
{
    int ret = 0;
    char** fkeys = NULL;
    unsigned int fkeysCount = 0;

    ret = GPlugMgr_Init();
    EXPECT_EQ(ret, 0);

    ret = GPlugMgr_QueryAllFkeys(&fkeys, &fkeysCount);
    EXPECT_EQ(ret, 0);

    ret = GPlugMgr_ReleaseAllFkeys(fkeys, fkeysCount);
    EXPECT_EQ(ret, 0);

    ret = GPlugMgr_ReleaseAllFkeys(NULL, fkeysCount);
    EXPECT_NE(ret, 0);

    ret = GPlugMgr_Deinit();
    EXPECT_EQ(ret, 0);
}

/**
 * @brief test GPlugMgr_CreateInstance
 */
TEST(GPlugMgr, GPlugMgr_CreateInstance)
{
    int ret = 0;
    char** fkeys = NULL;
    unsigned int fkeysCount = 0;

    ret = GPlugMgr_Init();
    EXPECT_EQ(ret, 0);

    ret = GPlugMgr_QueryAllFkeys(&fkeys, &fkeysCount);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(fkeysCount, 2);

    for(int i = 0; i < fkeysCount; i++)
    {
        GPluginHandle instance = NULL;
        int errCode = 0;

        ret = GPlugMgr_CreateInstance(fkeys[i], &instance, &errCode);
        ASSERT_EQ(ret, 0);

        ret = GPlugMgr_DestroyInstance(instance, &errCode);
        EXPECT_EQ(ret, 0);
    }

    {
        int errCode = 0;
        GPluginHandle instance = (GPluginHandle)1234;/* give a invalid value */

        ret = GPlugMgr_CreateInstance("NoExist", &instance, &errCode);
        EXPECT_NE(ret, 0);
        EXPECT_EQ(instance, (GPluginHandle)NULL);

        ret = GPlugMgr_CreateInstance("NoExist", NULL, &errCode);
        EXPECT_NE(ret, 0);
    }

    ret = GPlugMgr_ReleaseAllFkeys(fkeys, fkeysCount);
    EXPECT_EQ(ret, 0);

    ret = GPlugMgr_Deinit();
    EXPECT_EQ(ret, 0);
}

/**
 * @brief test GPlugMgr_DestroyInstance
 */
TEST(GPlugMgr, GPlugMgr_DestroyInstance)
{
    int ret = 0;
    char** fkeys = NULL;
    unsigned int fkeysCount = 0;

    ret = GPlugMgr_Init();
    EXPECT_EQ(ret, 0);

    ret = GPlugMgr_QueryAllFkeys(&fkeys, &fkeysCount);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(fkeysCount, 2);

    for(int i = 0; i < fkeysCount; i++)
    {
        GPluginHandle instance = NULL;
        int errCode = 0;

        ret = GPlugMgr_CreateInstance(fkeys[i], &instance, &errCode);
        ASSERT_EQ(ret, 0);

        ret = GPlugMgr_DestroyInstance(instance, &errCode);
        EXPECT_EQ(ret, 0);
    }

    {
        int errCode = 0;
        GPluginHandle instance = (GPluginHandle)1234;/* give a invalid value */
        ret = GPlugMgr_DestroyInstance(instance, &errCode);
        EXPECT_NE(ret, 0);
    
        ret = GPlugMgr_DestroyInstance(NULL, &errCode);
        EXPECT_NE(ret, 0);

        ret = GPlugMgr_DestroyInstance(instance, NULL);
        EXPECT_NE(ret, 0);
    }

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
