#ifndef THREADS_H
#define THREADS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

typedef struct Thread Thread;
struct Thread
{
#ifdef _WIN32
    HANDLE thread_handle;
#else
    pthread_t thread_id;
#endif
    bool is_active;
};

struct AThreads
{
    void (*CreateThreads)(void *(*function)(void *), void *arg);

    int (*CleanUpInactiveThreads)();

    void (*CleanUpThreads)();
};

extern struct AThreads AThreads;

#endif