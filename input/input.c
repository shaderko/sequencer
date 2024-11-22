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

void keyboard_action_down(KeyboardAction *action)
{
#ifdef _WIN32
    // Windows implementation
    INPUT input = {0};

    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0; // Hardware scan code for the key
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    // Simulate the key press
    input.ki.wVk = action->key; // Virtual-key code
    input.ki.dwFlags = 0;       // 0 for key press
    SendInput(1, &input, sizeof(INPUT));
#else
    // macOS implementation
    CGEventRef keyDownEvent = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)action->key, true);
    if (keyDownEvent)
    {
        CGEventPost(kCGHIDEventTap, keyDownEvent);
        CFRelease(keyDownEvent);
    }
    else
    {
        fprintf(stderr, "Failed to create key down event.\n");
    }
#endif
}

void keyboard_action_up(KeyboardAction *action)
{
#ifdef _WIN32
    // Windows implementation
    INPUT input = {0};

    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0; // Hardware scan code for the key
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    // Simulate the key release
    input.ki.wVk = action->key;         // Virtual-key code
    input.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &input, sizeof(INPUT));
#else
    // macOS implementation
    CGEventRef keyUpEvent = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)action->key, false);
    if (keyUpEvent)
    {
        CGEventPost(kCGHIDEventTap, keyUpEvent);
        CFRelease(keyUpEvent);
    }
    else
    {
        fprintf(stderr, "Failed to create key up event.\n");
    }
#endif
}

void mouse_action_down(MouseAction *action)
{
#ifdef _WIN32
    // Windows implementation
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;

    // Normalize coordinates to the range 0 - 65535 for absolute positioning
    input.mi.dx = (action->x * 65535) / GetSystemMetrics(SM_CXSCREEN);
    input.mi.dy = (action->y * 65535) / GetSystemMetrics(SM_CYSCREEN);
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &input, sizeof(INPUT));

    // Simulate small delay
    cross_platform_sleep(30000);

    // Map the action->button to the correct MOUSEEVENTF_* flags
    switch (action->button)
    {
    case 0: // Left button
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        break;
    case 1: // Right button
        input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        break;
    case 2: // Middle button
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
        break;
    default:
        return; // Invalid button
    }

    // Simulate the button press
    SendInput(1, &input, sizeof(INPUT));

#else
    // macOS implementation
    CGPoint point = CGPointMake(action->x, action->y);

    // Map the action->button to the correct CGMouseButton and event type
    CGMouseButton cgButton;
    CGEventType eventType;
    switch (action->button)
    {
    case 0: // Left button
        cgButton = kCGMouseButtonLeft;
        eventType = kCGEventLeftMouseDown;
        break;
    case 1: // Right button
        cgButton = kCGMouseButtonRight;
        eventType = kCGEventRightMouseDown;
        break;
    case 2: // Middle button
        cgButton = kCGMouseButtonCenter;
        eventType = kCGEventOtherMouseDown;
        break;
    default:
        return; // Invalid button
    }

    // Create and post the mouse down event
    CGEventRef mouseDownEvent = CGEventCreateMouseEvent(
        NULL,      // Default source
        eventType, // Mouse down event type
        point,     // Location
        cgButton   // Mouse button
    );

    if (mouseDownEvent)
    {
        CGEventPost(kCGHIDEventTap, mouseDownEvent);
        CFRelease(mouseDownEvent);
    }
    else
    {
        fprintf(stderr, "Failed to create mouse down event.\n");
    }
#endif
}

void mouse_action_up(MouseAction *action)
{
#ifdef _WIN32
    // Windows implementation
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;

    // Normalize coordinates to the range 0 - 65535 for absolute positioning
    input.mi.dx = (action->x * 65535) / GetSystemMetrics(SM_CXSCREEN);
    input.mi.dy = (action->y * 65535) / GetSystemMetrics(SM_CYSCREEN);
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &input, sizeof(INPUT));

    // Simulate the button release
    switch (action->button)
    {
    case 0: // Left button
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        break;
    case 1: // Right button
        input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        break;
    case 2: // Middle button
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
        break;
    default:
        return; // Invalid button
    }

    SendInput(1, &input, sizeof(INPUT));

#else
    // macOS implementation
    CGPoint point = CGPointMake(action->x, action->y);

    // Map the action->button to the correct CGMouseButton and event type
    CGMouseButton cgButton;
    CGEventType eventType;
    switch (action->button)
    {
    case 0: // Left button
        cgButton = kCGMouseButtonLeft;
        eventType = kCGEventLeftMouseUp;
        break;
    case 1: // Right button
        cgButton = kCGMouseButtonRight;
        eventType = kCGEventRightMouseUp;
        break;
    case 2: // Middle button
        cgButton = kCGMouseButtonCenter;
        eventType = kCGEventOtherMouseUp;
        break;
    default:
        return; // Invalid button
    }

    // Create and post the mouse up event
    CGEventRef mouseUpEvent = CGEventCreateMouseEvent(
        NULL,      // Default source
        eventType, // Mouse up event type
        point,     // Location
        cgButton   // Mouse button
    );

    if (mouseUpEvent)
    {
        CGEventPost(kCGHIDEventTap, mouseUpEvent);
        CFRelease(mouseUpEvent);
    }
    else
    {
        fprintf(stderr, "Failed to create mouse up event.\n");
    }
#endif
}

// void print_progress_bar(int percentage)
// {
//     int width = 50; // Width of the progress bar

//     // Calculate the number of filled positions
//     int pos = (percentage * width) / 100;

//     // Print the progress bar
//     printf("[");
//     for (int i = 0; i < width; ++i)
//     {
//         if (i < pos)
//         {
//             printf("#"); // Filled part
//         }
//         else
//         {
//             printf(" "); // Empty part
//         }
//     }
//     printf("] %d%%\r", percentage); // \r returns the cursor to the beginning of the line
//     fflush(stdout);                 // Force the output to be written immediately
// }