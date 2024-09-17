/**
 * @file record.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-09-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <record.h>

#include <logger.h>
#include <threads.h>
#include <input.h>
#include <helper.h>

static Record *Init()
{
    Record *record = malloc(sizeof(Record));
    if (!record)
    {
        log_message(LOG_ERROR, "Couldn't allocate memory for record");
        return NULL;
    }

    memset(record, 0, sizeof(Record));

    return record;
}

static void AddAction(Record *record, Action *action)
{
    if (!record || !action)
    {
        log_message(LOG_WARNING, "Couldn't add action");
        return;
    }

    if (!record->starting_action)
    {
        record->starting_action = action;
        return;
    }

    Action *current = record->starting_action;
    while (current->next)
    {
        current = current->next;
    }

    current->next = action;
}

void ExecuteSequenceThread(void *arg)
{
    // Cast the argument to the appropriate struct type
    ExecuteSequenceArgs *args = (ExecuteSequenceArgs *)arg;

    // Call the original ExecuteSequence function
    ARecord.ExecuteSequence(args->record, args->starting_index);

    // Optionally free the memory for args if dynamically allocated
    free(args);

    // End the thread
    return;
}

void __stdcall ProgressThread(void *param)
{
    int total_delay = (int)param;
    if (total_delay < 0)
    {
        return;
    }

    for (int i = 0; i < 100; i++)
    {
        print_progress_bar(i);
        cross_platform_sleep(total_delay / 100);
    }

    printf("\n");

    return;
}

static void ExecuteSequence(Record *record, int starting_index)
{
    if (!record)
    {
        puts("[WARNING] Record doesn't exist");
        return;
    }

    if (!record->starting_action)
    {
        printf("[ERROR] Record is empty");
        return;
    }

    int index = 0;

    Action *current = record->starting_action;
    while (current->next && index < starting_index)
    {
        current = current->next;
        index++;
    }

    if (index != starting_index)
    {
        printf("[WARNING] Index out of bounds, starting from the start");
        current = record->starting_action;
    }

    // get all delays for fun to create a progress bar
    int total_delay = 0;
    Action *current_action = record->starting_action;
    while (current_action)
    {
        if (current_action->next)
        {
            total_delay += current_action->next->time - current_action->time;
        }
        current_action = current_action->next;
    }

    // Convert total_delay from microseconds to seconds
    int total_seconds = total_delay / 1000000;

    int total_replay_times = record->replay_times;

    for (int i = 0; i < total_replay_times; i++)
    {
        int new_total_seconds = total_seconds * (total_replay_times - i);
        int hours = new_total_seconds / 3600;
        int minutes = (new_total_seconds % 3600) / 60;
        int seconds = new_total_seconds % 60;

        printf("[INFO] Approximate time to finish: %d hours, %d minutes, %d seconds\n", hours, minutes, seconds);

        printf("%i\n", i);

        AThreads.CreateThreads(ProgressThread, total_delay);

        AAction.ExecuteSequence(current);

        record->replay_times--;
    }

    puts("Finished!");
}

static void SelectRepeat(Record *record, int replay_times)
{
    if (!record)
    {
        puts("[WARNING] Record doesn't exist");
        return;
    }

    if (replay_times < 0)
    {
        printf("[WARNING] Record number cannot be negative");
        return;
    }

    record->replay_times = replay_times;
}

static Serialized Serialize(Record *record)
{
    if (!record)
    {
        puts("[WARNING] Record doesn't exist");
        return (Serialized){NULL, 0};
    }

    Serialized serialized;
    serialized.size = sizeof(double) + sizeof(double) + sizeof(int);
    serialized.buffer = malloc(serialized.size);
    if (!serialized.buffer)
    {
        puts("[ERROR] Couldn't allocate memory");
        return (Serialized){NULL, 0};
    }

    memcpy(serialized.buffer, &record->delay, sizeof(double));
    memcpy(serialized.buffer + sizeof(double), &record->offset, sizeof(double));
    memcpy(serialized.buffer + sizeof(double) + sizeof(double), &record->replay_times, sizeof(int));

    // serialize all actions for this record
    Action *current = record->starting_action;
    while (current)
    {
        Serialized action_serialized = AAction.Serialize(current);

        serialized.buffer = realloc(serialized.buffer, serialized.size + action_serialized.size + sizeof(size_t));
        if (!serialized.buffer)
        {
            puts("[ERROR] Couldn't reallocate memory");
            return (Serialized){NULL, 0};
        }

        // specify the size at the start
        memcpy(serialized.buffer + serialized.size, &action_serialized.size, sizeof(size_t));
        serialized.size += sizeof(size_t);

        memcpy(serialized.buffer + serialized.size, action_serialized.buffer, action_serialized.size);
        serialized.size += action_serialized.size;

        free(action_serialized.buffer);

        current = current->next;
    }

    puts("serialized");

    return serialized;
}

static Record *Deserialize(Serialized serialized)
{
    if (!serialized.buffer)
    {
        return NULL;
    }

    Record *record = ARecord.Init();

    memcpy(&record->delay, serialized.buffer, sizeof(double));
    memcpy(&record->offset, serialized.buffer + sizeof(double), sizeof(double));
    memcpy(&record->replay_times, serialized.buffer + sizeof(double) + sizeof(double), sizeof(int));

    Action *prev_action = NULL;

    // deserialize all actions for this record
    int offset = sizeof(double) + sizeof(double) + sizeof(int);
    while (offset < serialized.size)
    {
        Serialized action_serialized = {NULL, 0};
        memcpy(&action_serialized.size, serialized.buffer + offset, sizeof(size_t));
        offset += sizeof(size_t);

        action_serialized.buffer = malloc(action_serialized.size);
        if (!action_serialized.buffer)
        {
            puts("[ERROR] Couldn't allocate memory");
            return NULL;
        }

        memcpy(action_serialized.buffer, serialized.buffer + offset, action_serialized.size);

        Action *action = AAction.Deserialize(action_serialized);
        ARecord.AddAction(record, action);
        offset += action_serialized.size;
    }

    return record;
}

static void Remove(Record *record)
{
    if (!record)
    {
        puts("[WARNING] Record doesn't exist");
        return;
    }

    AAction.RemoveSequence(record->starting_action);

    free(record);
}

extern struct ARecord ARecord;
struct ARecord ARecord =
    {
        .Init = Init,
        .ExecuteSequenceThread = ExecuteSequenceThread,
        .AddAction = AddAction,
        .ExecuteSequence = ExecuteSequence,
        .SelectRepeat = SelectRepeat,
        .Serialize = Serialize,
        .Deserialize = Deserialize,
        .Remove = Remove,
};