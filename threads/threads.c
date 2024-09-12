/**
 * @file threads.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-09-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <threads.h>
#include <logger.h>
#include <helper.h>

#define MAX_THREADS 30

Thread *threads = NULL;
int thread_count = 0;

// function to create a new thread
void CreateThreads(void *(*function)(void *), void *arg)
{
    // check if there's an inactive thread we can reuse
    int reuse_index = -1;
    for (int i = 0; i < thread_count; i++)
    {
        if (!threads[i].is_active)
        {
            reuse_index = i;
            break;
        }
    }

    // if no inactive threads are available and thread_count is at the limit
    if (reuse_index == -1 && thread_count >= MAX_THREADS)
    {
        if (AThreads.CleanUpInactiveThreads() <= 0)
        {
            log_message(LOG_WARNING, "Cannot create more threads, limit reached.");

            // wait for a thread to finish
            while (thread_count >= MAX_THREADS && AThreads.CleanUpInactiveThreads() <= 0)
            {
                log_message(LOG_WARNING, "Waiting for a thread to finish...");
                cross_platform_sleep(1000);
            }
        }

        // find inactive thread after clean up
        for (int i = 0; i < thread_count; i++)
        {
            if (!threads[i].is_active)
            {
                reuse_index = i;
                break;
            }
        }
    }

    // Dynamically allocate memory for threads if needed
    if (threads == NULL)
    {
        threads = (Thread *)malloc(MAX_THREADS * sizeof(Thread));
    }

    // Either reuse an inactive thread slot or create a new one
    if (reuse_index != -1)
    {
#ifdef _WIN32
        threads[reuse_index].thread_handle = CreateThread(
            NULL, 0, (LPTHREAD_START_ROUTINE)function, arg, 0, NULL);
        if (threads[reuse_index].thread_handle == NULL)
        {
            log_message(LOG_ERROR, "Failed to create thread.");
            return;
        }
#else
        if (pthread_create(&threads[reuse_index].thread_id, NULL, function, arg) != 0)
        {
            log_message(LOG_ERROR, "Failed to create thread.");
            return;
        }
#endif
        threads[reuse_index].is_active = 1; // Mark thread as active again
    }
    else
    {
#ifdef _WIN32
        threads[thread_count].thread_handle = CreateThread(
            NULL, 0, (LPTHREAD_START_ROUTINE)function, arg, 0, NULL);
        if (threads[thread_count].thread_handle == NULL)
        {
            log_message(LOG_ERROR, "Failed to create thread.");
            return;
        }
#else
        if (pthread_create(&threads[thread_count].thread_id, NULL, function, arg) != 0)
        {
            log_message(LOG_ERROR, "Failed to create thread.");
            return;
        }
#endif
        threads[reuse_index].is_active = 1; // Mark thread as active again
        thread_count++;
    }
}

// Function to clean up inactive threads
int CleanUpInactiveThreads()
{
    int cleaned_up_count = 0;

    for (int i = 0; i < thread_count; i++)
    {
        if (threads[i].is_active)
        {
#ifdef _WIN32
            // Check if the thread is still running using WaitForSingleObject
            DWORD result = WaitForSingleObject(threads[i].thread_handle, 0);
            if (result == WAIT_OBJECT_0)
            {
                // Thread has finished execution
                log_message(LOG_INFO, "Thread %d is done, cleaning up.", i);
                CloseHandle(threads[i].thread_handle); // Close thread handle
                threads[i].is_active = 0;              // Mark the thread as inactive
                cleaned_up_count++;
            }
#else
            if (pthread_kill(threads[i].thread_id, 0) != 0)
            {
                log_message(LOG_INFO, "Thread %d is done, cleaning up.", i);
                threads[i].is_active = 0; // Mark the thread as inactive
                cleaned_up_count++;
            }
#endif
        }
    }

    return cleaned_up_count;
}

// Function to clean up all threads (terminate without waiting)
void CleanUpThreads()
{
    for (int i = 0; i < thread_count; i++)
    {
#ifdef _WIN32
        TerminateThread(threads[i].thread_handle, 0); // Terminate thread immediately
        CloseHandle(threads[i].thread_handle);        // Close the handle
#else
        pthread_cancel(threads[i].thread_id); // Cancel the thread on POSIX systems
#endif
        threads[i].is_active = 0;
    }
}

extern struct AThreads AThreads;
struct AThreads AThreads =
    {
        .CreateThreads = CreateThreads,
        .CleanUpInactiveThreads = CleanUpInactiveThreads,
        .CleanUpThreads = CleanUpThreads,
};