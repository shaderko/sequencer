/**
 * @file helper.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-09-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <helper.h>

void cross_platform_sleep(int microseconds)
{
#ifdef _WIN32
    // Sleep expects milliseconds, so divide by 1000
    Sleep(microseconds / 1000);
#else
    // usleep expects microseconds directly
    usleep(microseconds);
#endif
}