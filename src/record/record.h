#ifndef Record_H_
#define Record_H_

#include <stdlib.h>

#include <actions.h>
#include <io.h>

typedef struct Record Record;
struct Record
{
    Action *starting_action;

    double delay;
    double offset;
    int replay_times;
};

typedef struct
{
    Record *record;
    int starting_index;
} ExecuteSequenceArgs;

struct ARecord
{
    Record *(*Init)();

    void (*ExecuteSequenceThread)(void *arg);

    void (*AddAction)(Record *record, Action *action);

    void (*ExecuteSequence)(Record *record, int starting_index);

    void (*SelectRepeat)(Record *record, int replay_times);

    Serialized (*Serialize)(Record *record);

    Record *(*Deserialize)(Serialized serialized);

    void (*Remove)(Record *record);
};

extern struct ARecord ARecord;

#endif