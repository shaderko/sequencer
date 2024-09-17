/**
 * @file logger.h
 * @author shaderko
 * @brief Logger for logging xd
 * @version 0.1
 * @date 2024-09-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

// log types
typedef enum
{
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

int init_logger(const char *filename);
void close_logger();
void log_message(LogLevel level, const char *message, ...);

#endif