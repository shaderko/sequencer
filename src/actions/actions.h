#ifndef ACTIONS_H
#define ACTIONS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

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
    time_t time;
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

    Action *(*MouseActionInit)(int x, int y, bool is_press, int button, int time);

    Action *(*KeyboardActionInit)(bool is_press, int key, int time);

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