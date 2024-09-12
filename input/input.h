/**
 * @file input.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-09-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef INPUT_H
#define INPUT_H

#include <actions.h>
#include <record.h>
#include <windows.h>

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

void cross_platform_sleep(int microseconds);

void keyboard_action_down(KeyboardAction *action);
void keyboard_action_up(KeyboardAction *action);
void mouse_action_down(MouseAction *action);
void mouse_action_up(MouseAction *action);
void print_progress_bar(int percentage);

#endif