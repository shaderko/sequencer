#include <actions.h>

#include <input.h>

static Action *Init()
{
    Action *action = malloc(sizeof(Action));
    if (!action)
    {
        return NULL;
    }

    memset(action, 0, sizeof(Action));

    return action;
}

static Action *MouseActionInit(int x, int y, bool is_press, int button, int time)
{
    Action *action = Init();
    if (!action)
    {
        return NULL;
    }

    action->type = MOUSE_ACTION;
    action->time = time;

    action->data.mouse_action = malloc(sizeof(MouseAction));
    if (!action->data.mouse_action)
    {
        free(action);
        return NULL;
    }

    action->data.mouse_action->x = x;
    action->data.mouse_action->y = y;
    action->data.mouse_action->is_press = is_press;
    action->data.mouse_action->button = button;

    return action;
}

static Action *KeyboardActionInit(bool is_press, int key, int time)
{
    Action *action = Init();
    if (!action)
    {
        return NULL;
    }

    action->type = KEYBOARD_ACTION;
    action->time = time;

    action->data.keyboard_action = malloc(sizeof(KeyboardAction));
    if (!action->data.keyboard_action)
    {
        free(action);
        return NULL;
    }

    action->data.keyboard_action->key = key;
    action->data.keyboard_action->is_press = is_press;

    return action;
}

static void LinkActions(Action *action, Action *next)
{
    if (!action || !next)
    {
        printf("[WARNING] Couldn't link actions\n");
        return;
    }

    if (action->next)
    {
        printf("[ERROR] Action already has a next action\n");

        // todo memory leak?

        // return;
    }

    action->next = next;
}

// execute a single action
static void Execute(Action *action)
{
    switch (action->type)
    {
    case MOUSE_ACTION:
        if (action->data.mouse_action->is_press == 1)
        {
            mouse_action_down(action->data.mouse_action);
        }
        else
        {
            mouse_action_up(action->data.mouse_action);
        }
        break;
    case KEYBOARD_ACTION:
        if (action->data.keyboard_action->is_press == 1)
        {
            keyboard_action_down(action->data.keyboard_action);
        }
        else
        {
            keyboard_action_up(action->data.keyboard_action);
        }
        break;
    default:
        printf("[ERROR] Unknown action type\n");
        break;
    }

    time_t next_time;
    if (action->next)
    {
        next_time = action->next->time;
        cross_platform_sleep(next_time - action->time);
    }
}

// execute a sequence of actions
static void ExecuteSequence(Action *action)
{
    Action *current = action;
    while (current)
    {
        Execute(current);
        current = current->next;
    }
}

// serialize
static Serialized Serialize(Action *action)
{
    // serializes and returns a struct with buffer pointer and size
    Serialized serialized;

    serialized.size = sizeof(time_t) + sizeof(ActionType);
    switch (action->type)
    {
    case MOUSE_ACTION:
        serialized.size += sizeof(MouseAction);
        break;
    case KEYBOARD_ACTION:
        serialized.size += sizeof(KeyboardAction);
        break;
    }

    serialized.buffer = malloc(serialized.size);
    if (!serialized.buffer)
    {
        return serialized;
    }

    memcpy(serialized.buffer, &action->time, sizeof(time_t));
    memcpy(serialized.buffer + sizeof(time_t), &action->type, sizeof(ActionType));

    if (action->type == MOUSE_ACTION)
    {
        memcpy(serialized.buffer + sizeof(time_t) + sizeof(ActionType), action->data.mouse_action, sizeof(MouseAction));
    }
    else if (action->type == KEYBOARD_ACTION)
    {
        memcpy(serialized.buffer + sizeof(time_t) + sizeof(ActionType), action->data.keyboard_action, sizeof(KeyboardAction));
    }

    return serialized;
}

// deserialize
static Action *Deserialize(Serialized serialized)
{
    Action *action = AAction.Init();

    unsigned char *buffer_ptr = serialized.buffer;

    memcpy(&action->time, buffer_ptr, sizeof(time_t));
    buffer_ptr += sizeof(time_t);

    memcpy(&action->type, buffer_ptr, sizeof(ActionType));
    buffer_ptr += sizeof(ActionType);

    if (action->type == MOUSE_ACTION)
    {
        action->data.mouse_action = malloc(sizeof(MouseAction));
        if (!action->data.mouse_action)
        {
            free(action);
            return NULL;
        }

        memcpy(action->data.mouse_action, buffer_ptr, sizeof(MouseAction));
    }
    else if (action->type == KEYBOARD_ACTION)
    {
        action->data.keyboard_action = malloc(sizeof(KeyboardAction));
        if (!action->data.keyboard_action)
        {
            free(action);
            return NULL;
        }

        memcpy(action->data.keyboard_action, buffer_ptr, sizeof(KeyboardAction));
    }

    free(serialized.buffer);

    return action;
}

static void Remove(Action *action)
{
    if (!action)
    {
        puts("[WARNING] Action doesn't exist");
        return;
    }

    switch (action->type)
    {
    case MOUSE_ACTION:
        free(action->data.mouse_action);
        break;
    case KEYBOARD_ACTION:
        free(action->data.keyboard_action);
        break;
    }

    free(action);
}

static void RemoveSequence(Action *action)
{
    if (!action)
    {
        puts("[WARNING] Action doesn't exist");
        return;
    }

    Action *current = action;
    while (current)
    {
        Action *next = current->next;
        AAction.Remove(current);
        current = next;
    }
}

extern struct AAction AAction;
struct AAction AAction =
    {
        .Init = Init,
        .MouseActionInit = MouseActionInit,
        .KeyboardActionInit = KeyboardActionInit,
        .LinkActions = LinkActions,
        .Execute = Execute,
        .ExecuteSequence = ExecuteSequence,
        .Serialize = Serialize,
        .Deserialize = Deserialize,
        .Remove = Remove,
        .RemoveSequence = RemoveSequence,
};