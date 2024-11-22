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
// #include <threads.h>
// #include <input.h>

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

#ifdef _WIN32
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
#else

double get_elapsed_time_us()
{
    uint64_t end_time = mach_absolute_time();
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);

    // Convert to nanoseconds, then to microseconds
    uint64_t elapsed_ns = end_time * timebase.numer / timebase.denom;
    return elapsed_ns / 1000.0; // Convert to microseconds
}

// macOS-specific event tap callback
CGEventRef EventTapCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon)
{
    Recorder *recorder = (Recorder *)refcon;

    double elapsed_time_us = get_elapsed_time_us();
    int button = 0;
    bool is_press = false;

    if (type == kCGEventKeyDown)
    {
        int keyCode = (int)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

        // Check for the 'Z' key (keyCode for 'Z' is 6 on macOS)
        if (keyCode == 6)
        { // 'Z' key on macOS
            printf("[INFO] Z pressed, stopping recording...\n");
            recorder->is_recording = false;
            return NULL; // Stop processing events
        }
    }

    if (type == kCGEventKeyDown || type == kCGEventKeyUp)
    {
        int keyCode = (int)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
        printf("[INFO] KeyCode: %d, Pressed: %d\n", keyCode, is_press);
        is_press = (type == kCGEventKeyDown);
        Action *action = AAction.KeyboardActionInit(is_press, keyCode, elapsed_time_us);
        ARecord.AddAction(recorder->current, action);
    }
    else if (type == kCGEventLeftMouseDown || type == kCGEventRightMouseDown ||
             type == kCGEventLeftMouseUp || type == kCGEventRightMouseUp)
    {
        CGPoint point = CGEventGetLocation(event);
        button = (type == kCGEventRightMouseDown || type == kCGEventRightMouseUp) ? 1 : 0;
        is_press = (type == kCGEventLeftMouseDown || type == kCGEventRightMouseDown);
        printf("[INFO] Button: %d, Pressed: %d\n", button, is_press);
        Action *action = AAction.MouseActionInit((int)point.x, (int)point.y, is_press, button, elapsed_time_us);
        ARecord.AddAction(recorder->current, action);
    }

    if (!recorder->is_recording)
    {
        return NULL; // Stop processing events if recording is stopped
    }

    return event; // Pass the event along the chain
}
#endif

static void StartRecording(Recorder *recorder)
{
    if (recorder->is_recording)
    {
        log_message(LOG_WARNING, "Recorder is already recording");
        return;
    }

    recorder->is_recording = true;

    log_message(LOG_INFO, "Recording started");

    Record *record = ARecord.Init();
    ARecorder.AddRecord(record);

    recorder->current = record;

#ifdef _WIN32
    // Windows-specific hooks and message loop
    HHOOK keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProcRecorder, NULL, 0);
    HHOOK mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, MouseProcRecorder, NULL, 0);

    if (!keyboard_hook || !mouse_hook)
    {
        log_message(LOG_WARNING, "Failed to set hooks");
        recorder->is_recording = false;
        return;
    }

    MSG msg;
    while (recorder->is_recording)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    UnhookWindowsHookEx(keyboard_hook);
    UnhookWindowsHookEx(mouse_hook);

#else
    // macOS-specific event monitoring using Quartz
    CGEventMask eventMask = CGEventMaskBit(kCGEventKeyDown) |
                            CGEventMaskBit(kCGEventKeyUp) |
                            CGEventMaskBit(kCGEventLeftMouseDown) |
                            CGEventMaskBit(kCGEventLeftMouseUp) |
                            CGEventMaskBit(kCGEventRightMouseDown) |
                            CGEventMaskBit(kCGEventRightMouseUp);

    CFMachPortRef eventTap = CGEventTapCreate(
        kCGHIDEventTap,           // Tap location
        kCGHeadInsertEventTap,    // Placement
        kCGEventTapOptionDefault, // Options
        eventMask,                // Events to capture
        EventTapCallback,         // Callback function
        recorder                  // Pass recorder to callback
    );

    if (!eventTap)
    {
        log_message(LOG_WARNING, "Failed to create event tap");
        recorder->is_recording = false;
        return;
    }

    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(NULL, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CGEventTapEnable(eventTap, true);

    while (recorder->is_recording)
    {
        CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.1, false);
    }

    // Cleanup
    CFRunLoopRemoveSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CFRelease(runLoopSource);
    CFRelease(eventTap);
#endif

    recorder->is_recording = false;
    log_message(LOG_INFO, "Recording stopped");
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