#ifndef RECORDER_H
#define RECORDER_H

#include <stdbool.h>

#include <record.h>

typedef struct Recorder Recorder;
struct Recorder
{
    Record **records;
    size_t record_count;

    Record *current;

    bool is_recording;
};

struct ARecorder
{
    Recorder *(*Init)();

    void (*StartRecording)(Recorder *recorder);

    void (*StopRecording)(Recorder *recorder);

    void (*SelectRecord)(Recorder *recorder, int index);

    void (*AddRecord)(Record *record);

    void (*RemoveRecord)(int index);

    int (*Save)(Recorder *recorder, const char *path);

    int (*Load)(Recorder *recorder, const char *path);
};

extern struct ARecorder ARecorder;

#endif