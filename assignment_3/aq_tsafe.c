#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
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
    pthread_mutex_t mutex;    // Mutex for thread-safe access
    pthread_cond_t cond;      // Condition variable for signaling
    pthread_cond_t alarm_cond;      // Condition variable for alarm signaling
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

    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
    pthread_cond_init(&queue->alarm_cond, NULL);

    return (AlarmQueue)queue;
}

int aq_send(AlarmQueue aq, void* msg, MsgKind kind) {
    AlarmQueueStruct* queue = (AlarmQueueStruct*)aq;

    pthread_mutex_lock(&queue->mutex);

    if (kind == AQ_ALARM) {
        while (queue->alarm_present) {
            pthread_cond_wait(&queue->alarm_cond, &queue->mutex); // Wait until there’s a message
        }
        //if (queue->alarm_present) {
        //    pthread_mutex_unlock(&queue->mutex);
        //    return AQ_NO_ROOM; // Cannot send another alarm message
        //}
        queue->alarm_msg = (Msg*)msg;
        queue->alarm_present = 1;
    } else {
        if (queue->count >= queue->capacity) {
            int new_capacity = queue->capacity * 2;
            Msg** new_messages = (Msg**)realloc(queue->messages, new_capacity * sizeof(Msg*));
            if (!new_messages) {
                pthread_mutex_unlock(&queue->mutex);
                return AQ_NO_ROOM; // Reallocation failed
            }
            queue->messages = new_messages;
            queue->capacity = new_capacity;
            
        }
        queue->messages[queue->count++] = (Msg*)msg;
    }

    pthread_cond_signal(&queue->cond); // Signal any waiting receivers
    pthread_mutex_unlock(&queue->mutex);

    return 0; // Message sent successfully
}

int aq_recv(AlarmQueue aq, void** msg) {
    AlarmQueueStruct* queue = (AlarmQueueStruct*)aq;

    pthread_mutex_lock(&queue->mutex);

    while (queue->count == 0 && !queue->alarm_present) {
        pthread_cond_wait(&queue->cond, &queue->mutex); // Wait until there’s a message
    }

    if (queue->alarm_present) {
        *msg = queue->alarm_msg;
        queue->alarm_msg = NULL;
        queue->alarm_present = 0;
        pthread_mutex_unlock(&queue->mutex);
        pthread_cond_signal(&queue->alarm_cond);
        return AQ_ALARM; // Alarm message received
    } else {
        *msg = queue->messages[0];
        for (int i = 1; i < queue->count; i++) {
            queue->messages[i - 1] = queue->messages[i];
        }
        queue->count--;
        pthread_mutex_unlock(&queue->mutex);
        return AQ_NORMAL; // Normal message received
    }
}

int aq_size(AlarmQueue aq) {
    AlarmQueueStruct* queue = (AlarmQueueStruct*)aq;
    pthread_mutex_lock(&queue->mutex);
    int size = queue->count + queue->alarm_present;
    pthread_mutex_unlock(&queue->mutex);
    return size;
}

int aq_alarms(AlarmQueue aq) {
    AlarmQueueStruct* queue = (AlarmQueueStruct*)aq;
    pthread_mutex_lock(&queue->mutex);
    int alarms = queue->alarm_present;
    pthread_mutex_unlock(&queue->mutex);
    return alarms;
}

void aq_destroy(AlarmQueue aq) {
    AlarmQueueStruct* queue = (AlarmQueueStruct*)aq;
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond);
    for (int i = 0; i < queue->count; i++) {
        free(queue->messages[i]);
    }
    free(queue->messages);
    free(queue);
}
