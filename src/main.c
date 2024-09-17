/**
 * @file main.c
 * @author shaderko
 * @brief Main entry point for the program.
 * @version 0.1
 * @date 2024-09-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <stdio.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <window.h>

#include <logger.h>
#include <input.h>
#include <recorder.h>

void print_welcome_message()
{
    printf(" ___       __   _______   ___       ________  ________  _____ ______   _______      \n");
    printf("|\\  \\     |\\  \\|\\  ___ \\ |\\  \\     |\\   ____\\|\\   __  \\|\\   _ \\  _   \\|\\  ___ \\     \n");
    printf("\\ \\  \\    \\ \\  \\ \\   __/|\\ \\  \\    \\ \\  \\___|\\ \\  \\|\\  \\ \\  \\\\\\__\\ \\  \\ \\   __/|    \n");
    printf(" \\ \\  \\  __\\ \\  \\ \\  \\_|/_\\ \\  \\    \\ \\  \\    \\ \\  \\\\\\  \\ \\  \\\\|__| \\  \\ \\  \\_|/__  \n");
    printf("  \\ \\  \\|\\__\\_\\  \\ \\  \\_|\\ \\ \\  \\____\\ \\  \\____\\ \\  \\\\\\  \\ \\  \\    \\ \\  \\ \\  \\_|\\ \\ \n");
    printf("   \\ \\____________\\ \\_______\\ \\_______\\ \\_______\\ \\_______\\ \\__\\    \\ \\__\\ \\_______\\\n");
    printf("    \\|____________|\\|_______|\\|_______|\\|_______|\\|_______|\\|__|     \\|__|\\|_______|\n");
    printf("                                To Sequencer                                       \n");
    printf("                               Mainly for OSRS                                     \n");
    printf("                                                                                   \n");
}

// function to clean up any resources that need to be cleaned up and save records at exit
void cleanup()
{
    Recorder *r = ARecorder.Init();
    // ARecorder.Save(r, "save.xdlmaorofl");
}

int main(int argc, char *argv[])
{
    // initialize writing logs to a file
    init_logger("log.log");

    log_message(LOG_INFO, "Loading saved records...");

    // create the main recorder, this is what records actions and replays them
    Recorder *r = ARecorder.Init();
    if (ARecorder.Load(r, "save.xdlmaorofl") > 0)
    {
        log_message(LOG_ERROR, "Failed to load records");
    }

    // register the save_records function to be called at exit
    atexit(cleanup);

    print_welcome_message();

    // // set keyboard hooks for the main thread
    // HHOOK keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    // HHOOK mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);

    // if (!keyboard_hook || !mouse_hook)
    // {
    //     log_message(LOG_ERROR, "Failed to set hooks");
    //     return 1;
    // }

    // // process messages in the message queue
    // MSG msg;
    // while (GetMessage(&msg, NULL, 0, 0))
    // {
    //     TranslateMessage(&msg);
    //     DispatchMessage(&msg);
    // }

    // // unhook when done
    // UnhookWindowsHookEx(keyboard_hook);
    // UnhookWindowsHookEx(mouse_hook);

    Window *window = AWindow.Init(800, 600, "Sequencer");

    // Event loop
    int quit = 0;
    SDL_Event e;
    while (!quit)
    {
        // Handle events
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = 1;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
            }
        }

        // Clear screen with transparent background
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 0); // Fully transparent background
        SDL_RenderClear(window->renderer);

        // Update screen
        SDL_RenderPresent(window->renderer);
    }

    return 0;
}