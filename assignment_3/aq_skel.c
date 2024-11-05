/**
 * @file   aq.c
 * @Author 02335 team
 * @date   October, 2024
 * @brief  Alarm queue skeleton implementation
 */

#include <stdlib.h>
#include <stdio.h>
#include "aq.h"

typedef struct {
    int value;
    MsgKind kind;
} Msg;

typedef struct {
    Msg** messages;       // Dynamic array of pointers to normal messages
    Msg* alarm_msg;       // Pointer to the single alarm message (if any)
    int capacity;         // Maximum number of normal messages
    int count;            // Current count of normal messages
    int alarm_present;    // Indicator if an alarm is in the queue (0 or 1)
} AlarmQueueStruct;

#define INITIAL_CAPACITY 4

AlarmQueue aq_create() {
    AlarmQueueStruct* queue = (AlarmQueueStruct*)malloc(sizeof(AlarmQueueStruct));
    if (!queue) return NULL;

    queue->messages = (Msg**)malloc(sizeof(Msg*) * INITIAL_CAPACITY);
    if (!queue->messages) {
        free(queue);
        return NULL;
    }
    queue->capacity = INITIAL_CAPACITY;
    queue->count = 0;
    queue->alarm_present = 0;
    queue->alarm_msg = NULL;

    return (AlarmQueue)queue;
}

int aq_send(AlarmQueue aq, void* msg, MsgKind kind) {
    AlarmQueueStruct* queue = (AlarmQueueStruct*)aq;

    if (kind == AQ_ALARM) {
        if (queue->alarm_present) {
            return AQ_NO_ROOM; // Cannot send another alarm message
        }
        queue->alarm_msg = (Msg*)msg;
        queue->alarm_present = 1;
    } else {
        // Handle normal message
        if (queue->count >= queue->capacity) {
            // Double the size of the messages array
            int new_capacity = queue->capacity * 2;
            Msg** new_messages = (Msg**)realloc(queue->messages, new_capacity * sizeof(Msg*));
            if (!new_messages) {
                return AQ_NO_ROOM; // Reallocation failed
            }
            queue->messages = new_messages;
            queue->capacity = new_capacity;
        }
        queue->messages[queue->count++] = (Msg*)msg;
    }
    return 0; // Message sent successfully
}

int aq_recv(AlarmQueue aq, void** msg) {
    AlarmQueueStruct* queue = (AlarmQueueStruct*)aq;

    if (queue->alarm_present) {
        // Return the alarm message
        *msg = queue->alarm_msg;
        queue->alarm_msg = NULL;
        queue->alarm_present = 0;
        return AQ_ALARM; // Indicate that an alarm message was received
    } else if (queue->count > 0) {
        // Return the next normal message
        *msg = queue->messages[0];
        for (int i = 1; i < queue->count; i++) {
            queue->messages[i - 1] = queue->messages[i]; // Shift messages left
        }
        queue->count--;
        return AQ_NORMAL; // Indicate that a normal message was received
    }

    return AQ_NO_MSG; // No messages available
}

int aq_size(AlarmQueue aq) {
    AlarmQueueStruct* queue = (AlarmQueueStruct*)aq;
    return queue->count + queue->alarm_present; // Total messages
}

int aq_alarms(AlarmQueue aq) {
    AlarmQueueStruct* queue = (AlarmQueueStruct*)aq;
    return queue->alarm_present; // 0 or 1
}

void aq_destroy(AlarmQueue aq) {
    AlarmQueueStruct* queue = (AlarmQueueStruct*)aq;
    for (int i = 0; i < queue->count; i++) {
        free(queue->messages[i]); // Free normal messages
    }
    free(queue->messages);
    free(queue);
}