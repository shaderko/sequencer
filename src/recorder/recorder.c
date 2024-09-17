/**
 * @file recorder.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-09-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <recorder.h>
#include <time.h>

#include <logger.h>
#include <threads.h>
#include <input.h>

static Recorder *recorder = NULL;

static Recorder *Init()
{
    if (recorder)
    {
        return recorder;
    }

    Recorder *recorder_init = malloc(sizeof(Recorder));
    if (!recorder_init)
    {
        return NULL;
    }

    memset(recorder_init, 0, sizeof(Recorder));
    recorder = recorder_init;

    return recorder_init;
}

LRESULT CALLBACK KeyboardProcRecorder(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;
        double elapsed_time_us = (double)clock() / CLOCKS_PER_SEC * 1000000;

        // Key down events
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
        {
            if (kbd->vkCode == 0x5A)
            {
                printf("[INFO] Z pressed, stopping recording...\n");
                ARecorder.StopRecording(recorder);
            }

            Action *action = AAction.KeyboardActionInit(true, kbd->vkCode, elapsed_time_us);
            ARecord.AddAction(recorder->current, action);
        }
        // Key up events
        else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
        {
            Action *action = AAction.KeyboardActionInit(false, kbd->vkCode, elapsed_time_us); // Different action type for key up
            ARecord.AddAction(recorder->current, action);
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProcRecorder(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        MSLLHOOKSTRUCT *mouse = (MSLLHOOKSTRUCT *)lParam;
        int button = 0;
        bool is_press = false;

        double elapsed_time_us = (double)clock() / CLOCKS_PER_SEC * 1000000;

        // Check for left button down and up
        if (wParam == WM_LBUTTONDOWN)
        {
            button = VK_LBUTTON; // Left mouse button
            is_press = true;     // Mouse button is being pressed
        }
        else if (wParam == WM_LBUTTONUP)
        {
            button = VK_LBUTTON; // Left mouse button
            is_press = false;    // Mouse button is being released
        }

        // Check for right button down and up
        else if (wParam == WM_RBUTTONDOWN)
        {
            button = VK_RBUTTON; // Right mouse button
            is_press = true;     // Mouse button is being pressed
        }
        else if (wParam == WM_RBUTTONUP)
        {
            button = VK_RBUTTON; // Right mouse button
            is_press = false;    // Mouse button is being released
        }

        // Check for middle button down and up
        else if (wParam == WM_MBUTTONDOWN)
        {
            button = VK_MBUTTON; // Middle mouse button
            is_press = true;     // Mouse button is being pressed
        }
        else if (wParam == WM_MBUTTONUP)
        {
            button = VK_MBUTTON; // Middle mouse button
            is_press = false;    // Mouse button is being released
        }

        // If a button was pressed or released, record the action
        if (button != 0)
        {
            printf("[INFO] Button: %d, Pressed: %d\n", button, is_press);
            Action *action = AAction.MouseActionInit(mouse->pt.x, mouse->pt.y, is_press, button, elapsed_time_us);
            ARecord.AddAction(recorder->current, action);
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

static void *worker_thread_func(void *arg)
{
    Recorder *recorder = (Recorder *)arg;

    Record *record = ARecord.Init();
    ARecorder.AddRecord(record);

    recorder->current = record;

    HHOOK keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProcRecorder, NULL, 0);
    HHOOK mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, MouseProcRecorder, NULL, 0);

    if (!keyboard_hook || !mouse_hook)
    {
        printf("[ERROR] Failed to set hooks\n");
        return NULL;
    }

    MSG msg;
    while (recorder->is_recording)
    {
        // Use MsgWaitForMultipleObjects to wait for input or messages efficiently
        DWORD result = MsgWaitForMultipleObjects(0, NULL, FALSE, INFINITE, QS_ALLINPUT);
        if (result == WAIT_OBJECT_0)
        {
            // There is input or a message, handle it
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    // Unhook when done
    UnhookWindowsHookEx(keyboard_hook);
    UnhookWindowsHookEx(mouse_hook);

    return NULL;
}

static void StartRecording(Recorder *recorder)
{
    if (recorder->is_recording)
    {
        log_message(LOG_WARNING, "Recorder is already recording");
        return;
    }

    recorder->is_recording = true;

    log_message(LOG_INFO, "Recording started");

    // start recording mouse and keyboard actions in separate threads
    AThreads.CreateThreads(worker_thread_func, recorder);
}

static void StopRecording(Recorder *recorder)
{
    if (!recorder->is_recording)
    {
        log_message(LOG_INFO, "Recorder is not recording");
        return;
    }

    recorder->is_recording = false;
}

static void SelectRecord(Recorder *recorder, int index)
{
    if (recorder->is_recording)
    {
        log_message(LOG_WARNING, "Recorder is recording");
        return;
    }

    if (index < 0 || index >= recorder->record_count)
    {
        log_message(LOG_WARNING, "Invalid record index");
        return;
    }

    recorder->current = recorder->records[index];
}

static void AddRecord(Record *record)
{
    recorder->records = realloc(recorder->records, sizeof(Record *) * (recorder->record_count + 1));
    recorder->records[recorder->record_count++] = record;
}

static void RemoveRecord(int index)
{
    if (index < 0 || index >= recorder->record_count)
    {
        log_message(LOG_WARNING, "Invalid record index");
        return;
    }

    recorder->current = NULL;

    ARecord.Remove(recorder->records[index]);

    for (int i = index; i < recorder->record_count - 1; i++)
    {
        recorder->records[i] = recorder->records[i + 1];
    }
    recorder->record_count--;
}

static void Save(Recorder *recorder, const char *path)
{
    if (recorder->is_recording)
    {
        log_message(LOG_WARNING, "Recorder is recording");
        return;
    }

    clear_file(path);

    for (int i = 0; i < recorder->record_count; i++)
    {
        Serialized record = ARecord.Serialize(recorder->records[i]);
        save(record, path);
    }

    log_message(LOG_INFO, "Records saved");
}

static int Load(Recorder *recorder, const char *path)
{
    if (recorder->is_recording)
    {
        log_message(LOG_WARNING, "Recorder is recording");
        return 1;
    }

    Serialized **data;
    size_t count;
    if (load_all(&data, &count, path) > 0)
    {
        log_message(LOG_ERROR, "Failed to load records");
        return 1;
    }

    for (size_t i = 0; i < count; i++)
    {
        Record *record = ARecord.Deserialize(*(data[i]));
        AddRecord(record);
    }

    // Free the allocated memory
    for (size_t i = 0; i < count; i++)
    {
        free(data[i]);
    }
    free(*data);

    return 0;
}

extern struct ARecorder ARecorder;
struct ARecorder ARecorder =
    {
        .Init = Init,
        .StartRecording = StartRecording,
        .StopRecording = StopRecording,
        .SelectRecord = SelectRecord,
        .AddRecord = AddRecord,
        .RemoveRecord = RemoveRecord,
        .Save = Save,
        .Load = Load,
};