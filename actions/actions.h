#ifndef ACTIONS_H
#define ACTIONS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <helper.h>
#include <io.h>

typedef enum
{
    MOUSE_ACTION,
    KEYBOARD_ACTION,
} ActionType;

typedef struct MouseAction MouseAction;
struct MouseAction
{
    int x;
    int y;

    bool is_press;

    int button;
};

typedef struct KeyboardAction KeyboardAction;
struct KeyboardAction
{
    bool is_press;

    int key;
};

typedef struct Action Action;
struct Action
{
    double time;
    ActionType type;

    union
    {
        MouseAction *mouse_action;
        KeyboardAction *keyboard_action;
    } data;

    Action *next;
};

struct AAction
{
    Action *(*Init)();

    Action *(*MouseActionInit)(int x, int y, bool is_press, int button, double time);

    Action *(*KeyboardActionInit)(bool is_press, int key, double time);

    void (*LinkActions)(Action *action, Action *next);

    void (*Execute)(Action *action);

    void (*ExecuteSequence)(Action *action);

    Serialized (*Serialize)(Action *action);

    Action *(*Deserialize)(Serialized serialized);

    void (*Remove)(Action *action);

    void (*RemoveSequence)(Action *action);
};

extern struct AAction AAction;

#endif