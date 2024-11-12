#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include "aq.h"
#include "aux.h"
// Define the Msg structure if it's not included in aq.h
typedef struct {
    int value;
    MsgKind kind;
} Msg;

static AlarmQueue q;

void *producer_send_alarms(void *arg) {
    put_alarm(q, 13);
    put_alarm(q, 14);
    return NULL;
}

void *producer_send_normal(void *arg) {
    msleep(1000);
    for (int i = 1; i <= 10; i++) {
        
            put_normal(q, i);
        }
        void *msg;
        int ret;
        
        // Retrieve all messages to confirm order and reallocation success
        return NULL;
    }

void *consumer(void *arg) {
    msleep(3000);
    assert(print_sizes_all(q) == 12); //Check if all 12 (normal and alarm) are sent
    assert(print_sizes(q) == 11); //Check if there are 11 normal 
    for (int i = 1; i <= 12; i++) {
        
        get(q);
        msleep(500);
    }
    return NULL;
}

int main(int argc, char **argv) {
    q = aq_create();
    if (q == NULL) {
        printf("Alarm queue could not be created\n");
        exit(1);
    }

    pthread_t t1, t2, t3;

    // Thread 1: Send first alarm
    pthread_create(&t1, NULL, producer_send_alarms, NULL);

    // Thread 2: Attempt to send second alarm, should block
    pthread_create(&t2, NULL, producer_send_normal, NULL);

    // Thread 3: Receive the first alarm, unblocking Thread 2
    pthread_create(&t3, NULL, consumer, NULL);

    // Wait for all threads to complete
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    return 0;
}
