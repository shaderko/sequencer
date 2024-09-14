/**
 * @file input.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-09-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <input.h>
#include <record.h>
#include <recorder.h>
#include <threads.h>
#include <io.h>

unsigned __stdcall InputRecordThread(void *param)
{
    int recordNumber; // Declare recordNumber as a local variable

    // Get user input from terminal
    printf("Enter a record number: ");
    scanf("%d", &recordNumber); // Read terminal input

    printf("Record number changed to: %d\n", recordNumber);

    // Initialize the recorder
    Recorder *r = ARecorder.Init();

    // Select the record using the updated record number
    ARecorder.SelectRecord(r, recordNumber);

    return 0;
}

unsigned __stdcall InputRepeatThread(void *param)
{
    int repeat_times; // Declare recordNumber as a local variable

    // Get user input from terminal
    printf("Enter a repeat times: ");
    scanf("%d", &repeat_times); // Read terminal input

    printf("Repeat time changed to: %d\n", repeat_times);

    // Initialize the recorder
    Recorder *r = ARecorder.Init();

    // Select the record using the updated record number
    ARecord.SelectRepeat(r->current, repeat_times);

    return 0;
}

unsigned __stdcall InputRemoveRecordThread(void *param)
{
    int recordNumber; // Declare recordNumber as a local variable

    // Get user input from terminal
    printf("Enter a record number to delete: ");
    scanf("%d", &recordNumber); // Read terminal input

    // Select the record using the updated record number
    ARecorder.RemoveRecord(recordNumber);

    puts("Record deleted");

    return 0;
}

// Callback function for keyboard events
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
        {
            KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;

            if (kbd->vkCode == VK_HOME)
            {
                Recorder *recorder = ARecorder.Init();
                printf("[INFO] Executing current record\n");

                ExecuteSequenceArgs *args = malloc(sizeof(ExecuteSequenceArgs));
                args->record = recorder->current;
                args->starting_index = 0;

                AThreads.CreateThreads(ARecord.ExecuteSequenceThread, args);
            }
            else if (kbd->vkCode == VK_END)
            {
                Recorder *r = ARecorder.Init();
                ARecorder.StartRecording(r);
            }
            else if (kbd->vkCode == 0x4F)
            {
                AThreads.CreateThreads(InputRecordThread, NULL);
            }
            else if (kbd->vkCode == 0x55)
            {
                AThreads.CreateThreads(InputRepeatThread, NULL);
            }
            else if (kbd->vkCode == 0x49)
            {
                AThreads.CleanUpThreads();
            }
            else if (kbd->vkCode == 0x58) // ESC key
            {
                // Gracefully exit the message loop
                // PostQuitMessage(0); // Posts a WM_QUIT message to exit the message loop
                Recorder *recorder = ARecorder.Init();
                ARecorder.Save(recorder, "save.xdlmaorofl");
            }
            else if (kbd->vkCode == 0x43)
            {
                AThreads.CreateThreads(InputRemoveRecordThread, NULL);
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Callback function for mouse events
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        if (wParam == WM_LBUTTONDOWN)
        {
            MSLLHOOKSTRUCT *mouse = (MSLLHOOKSTRUCT *)lParam;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void keyboard_action_down(KeyboardAction *action)
{
    INPUT input;

    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0; // Hardware scan code for the key
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    // Simulate the key press (e.g., 'A' key)
    input.ki.wVk = action->key; // Virtual-key code for 'A'
    input.ki.dwFlags = 0;       // 0 for key press
    SendInput(1, &input, sizeof(INPUT));
}

void keyboard_action_up(KeyboardAction *action)
{
    INPUT input;

    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0; // Hardware scan code for the key
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    // Simulate the key press (e.g., 'A' key)
    input.ki.wVk = action->key;         // Virtual-key code for 'A'
    input.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &input, sizeof(INPUT));
}

void mouse_action_down(MouseAction *action)
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;

    // Normalize coordinates to the range 0 - 65535 for absolute positioning
    input.mi.dx = (action->x * 65535) / GetSystemMetrics(SM_CXSCREEN);
    input.mi.dy = (action->y * 65535) / GetSystemMetrics(SM_CYSCREEN);
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &input, sizeof(INPUT));

    // simulate small delay for osrs to register the mouse position to show proper right options
    cross_platform_sleep(30000);

    // Map the action->button to the correct MOUSEEVENTF_* flags
    switch (action->button)
    {
    case VK_LBUTTON:
        input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN; // Left mouse button down
        break;

    case VK_RBUTTON:
        input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN; // Right mouse button down
        break;

    case VK_MBUTTON:
        input.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN; // Middle mouse button down
        break;

    default:
        // No valid button
        return;
    }

    // Simulate the button press
    SendInput(1, &input, sizeof(INPUT));
}

void mouse_action_up(MouseAction *action)
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &input, sizeof(INPUT));

    // Normalize coordinates to the range 0 - 65535 for absolute positioning
    input.mi.dx = (action->x * 65535) / GetSystemMetrics(SM_CXSCREEN);
    input.mi.dy = (action->y * 65535) / GetSystemMetrics(SM_CYSCREEN);

    // Simulate the button release after press
    input.mi.dwFlags &= ~(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_MIDDLEDOWN);
    input.mi.dwFlags |= (action->button == VK_LBUTTON) ? MOUSEEVENTF_LEFTUP : (action->button == VK_RBUTTON) ? MOUSEEVENTF_RIGHTUP
                                                                          : (action->button == VK_MBUTTON)   ? MOUSEEVENTF_MIDDLEUP
                                                                                                             : 0;

    SendInput(1, &input, sizeof(INPUT));
}

void print_progress_bar(int percentage)
{
    int width = 50; // Width of the progress bar

    // Calculate the number of filled positions
    int pos = (percentage * width) / 100;

    // Print the progress bar
    printf("[");
    for (int i = 0; i < width; ++i)
    {
        if (i < pos)
        {
            printf("#"); // Filled part
        }
        else
        {
            printf(" "); // Empty part
        }
    }
    printf("] %d%%\r", percentage); // \r returns the cursor to the beginning of the line
    fflush(stdout);                 // Force the output to be written immediately
}