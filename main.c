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
#include <SDL2/SDL.h>
#include <stdbool.h>

#include <logger.h>
// #include <input.h>
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
void cleanup_and_save_records()
{
    Recorder *r = ARecorder.Init();
    ARecorder.Save(r, "save.xdlmaorofl");
}

void display_help()
{
    printf("Available commands:\n");
    printf("  help       - Show this help message\n");
    printf("  record N   - Record with number N\n");
    printf("  repeat N   - Repeat action N times\n");
    printf("  load path  - Load saved records, optional path argument\n");
    printf("  save path  - Save records, optional path argument\n");
    printf("  exit       - Quit the program\n");
}

int main()
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
    atexit(cleanup_and_save_records);

    print_welcome_message();

    bool running = true;
    while (running)
    {
        // create a buffer to store commands
        char input[1024];
        printf("> ");

        fgets(input, sizeof(input), stdin);

        input[strcspn(input, "\n")] = '\0';

        // Tokenize the input into command and arguments
        char *command = strtok(input, " "); // First token is the command
        char *arg = strtok(NULL, " ");      // Second token is the argument (if any)

        if (command == NULL)
        {
            continue; // Skip empty input
        }

        // Match commands
        if (strcmp(command, "help") == 0)
        {
            display_help();
        }
        else if (strcmp(command, "record") == 0)
        {
            if (arg != NULL)
            {
                int recordNumber = atoi(arg); // Convert argument to integer
                ARecorder.SelectRecord(r, recordNumber);
            }
            else
            {
                printf("Error: 'record' command requires a number argument.\n");
            }
        }
        else if (strcmp(command, "repeat") == 0)
        {
            if (arg != NULL)
            {
                int times = atoi(arg); // Convert argument to integer
                ARecord.SelectRepeat(r->current, times);
            }
            else
            {
                printf("Error: 'repeat' command requires a number argument.\n");
            }
        }
        else if (strcmp(command, "x") == 0)
        {
            ARecorder.StartRecording(r);
        }
        else if (strcmp(command, "replay") == 0)
        {
            ARecord.ExecuteSequence(r->current, 0);
        }
        else if (strcmp(command, "save") == 0)
        {
            const char *path = (arg != NULL) ? arg : "save.xdlmaorofl"; // Default path if not provided
            ARecorder.Save(r, path);
        }
        else if (strcmp(command, "load") == 0)
        {
            const char *path = (arg != NULL) ? arg : "save.xdlmaorofl"; // Default path if not provided
            ARecorder.Load(r, path);
        }
        else if (strcmp(command, "exit") == 0)
        {
            printf("Exiting...\n");
            running = 0; // Exit the loop
        }
        else
        {
            printf("Invalid command: '%s'. Type 'help' for a list of commands.\n", command);
        }
    }

    // save records if we get here
    ARecorder.Save(r, "save.xdlmaorofl");

    return 0;
}