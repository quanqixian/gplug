#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

#define PRINT_RED                    "\033[1;31m"         /* 鲜红 */
#define PRINT_L_PURPLE               "\033[1;35m"         /* 亮粉 */
#define PRINT_YELLOW                 "\033[1;33m"         /* 鲜黄 */
#define PRINT_L_GREEN                "\033[1;32m"         /* 鲜绿 */
#define PRINT_NONE                   "\033[0m"
#define PRINT_COLOR_LOG              (1)                  /* 是否打印有颜色的日志 */

#if PRINT_COLOR_LOG
    #define GPLUGMGR_LOG_FATAL(errnum, fmt, ...) printf(PRINT_RED "[F] %s:%d [%s] errnum=%d " fmt PRINT_NONE "\n", __FILE__, __LINE__, __FUNCTION__, errnum, ##__VA_ARGS__)
    #define GPLUGMGR_LOG_ERROR(errnum, fmt, ...) printf(PRINT_RED "[E] %s:%d [%s] errnum=%d " fmt PRINT_NONE "\n", __FILE__, __LINE__, __FUNCTION__, errnum, ##__VA_ARGS__)
    #define GPLUGMGR_LOG_WARN(errnum, fmt, ...)  printf(PRINT_L_PURPLE "[W] %s:%d [%s] errnum=%d " fmt PRINT_NONE "\n", __FILE__, __LINE__, __FUNCTION__, errnum, ##__VA_ARGS__)
    #define GPLUGMGR_LOG_INFO(fmt, ...)          printf(PRINT_L_GREEN "[I] %s:%d [%s] " fmt PRINT_NONE "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
    #define GPLUGMGR_LOG_DEBUG(fmt, ...)         printf("[D] %s:%d [%s] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
    #define GPLUGMGR_LOG_TRACE(fmt, ...)         printf("[T] %s:%d [%s] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
    #define GPLUG_LOG_FATAL(errnum, fmt, ...) printf(PRINT_RED "[F] %s:%d [%s] errnum=%d " fmt PRINT_NONE "\n", __FILE__, __LINE__, __FUNCTION__, errnum, ##__VA_ARGS__)
    #define GPLUG_LOG_ERROR(errnum, fmt, ...) printf(PRINT_RED "[E] %s:%d [%s] errnum=%d " fmt PRINT_NONE "\n", __FILE__, __LINE__, __FUNCTION__, errnum, ##__VA_ARGS__)
    #define GPLUG_LOG_WARN(errnum, fmt, ...)  printf(PRINT_L_PURPLE "[W] %s:%d [%s] errnum=%d " fmt PRINT_NONE "\n", __FILE__, __LINE__, __FUNCTION__, errnum, ##__VA_ARGS__)
    #define GPLUG_LOG_INFO(fmt, ...)          printf(PRINT_L_GREEN "[I] %s:%d [%s] " fmt PRINT_NONE "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
    #define GPLUG_LOG_DEBUG(fmt, ...)         printf("[D] %s:%d [%s] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
    #define GPLUG_LOG_TRACE(fmt, ...)         printf("[T] %s:%d [%s] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define GPLUGMGR_LOG_FATAL(errnum, fmt, ...) printf("[F] %s:%d [%s] errnum=%d " fmt "\n", __FILE__, __LINE__, __FUNCTION__, errnum, ##__VA_ARGS__)
    #define GPLUGMGR_LOG_ERROR(errnum, fmt, ...) printf("[E] %s:%d [%s] errnum=%d " fmt "\n", __FILE__, __LINE__, __FUNCTION__, errnum, ##__VA_ARGS__)
    #define GPLUGMGR_LOG_WARN(errnum, fmt, ...)  printf("[W] %s:%d [%s] errnum=%d " fmt "\n", __FILE__, __LINE__, __FUNCTION__, errnum, ##__VA_ARGS__)
    #define GPLUGMGR_LOG_INFO(fmt, ...)          printf("[I] %s:%d [%s] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
    #define GPLUGMGR_LOG_DEBUG(fmt, ...)         printf("[D] %s:%d [%s] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
    #define GPLUG_LOG_FATAL(errnum, fmt, ...) printf("[F] %s:%d [%s] errnum=%d " fmt "\n", __FILE__, __LINE__, __FUNCTION__, errnum, ##__VA_ARGS__)
    #define GPLUG_LOG_ERROR(errnum, fmt, ...) printf("[E] %s:%d [%s] errnum=%d " fmt "\n", __FILE__, __LINE__, __FUNCTION__, errnum, ##__VA_ARGS__)
    #define GPLUG_LOG_WARN(errnum, fmt, ...)  printf("[W] %s:%d [%s] errnum=%d " fmt "\n", __FILE__, __LINE__, __FUNCTION__, errnum, ##__VA_ARGS__)
    #define GPLUG_LOG_INFO(fmt, ...)          printf("[I] %s:%d [%s] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
    #define GPLUG_LOG_DEBUG(fmt, ...)         printf("[D] %s:%d [%s] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
    #define GPLUG_LOG_TRACE(fmt, ...)         printf("[T] %s:%d [%s] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#endif

#endif
