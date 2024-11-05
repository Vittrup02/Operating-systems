#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include "aq.h"

// Define the Msg structure if it's not included in aq.h
typedef struct {
    int value;
    MsgKind kind;
} Msg;

AlarmQueue q;

void *send_alarm_1(void *arg) {
    Msg *msg1 = (Msg *)malloc(sizeof(Msg));
    if (!msg1) {
        perror("Failed to allocate memory for msg1");
        pthread_exit(NULL);
    }
    msg1->value = 1;
    msg1->kind = AQ_ALARM;
    int res = aq_send(q, msg1, AQ_ALARM);
    assert(res == 0);
    printf("Thread 1: Sent alarm message 1\n");
    return NULL;
}

void *send_alarm_2(void *arg) {
    Msg *msg2 = (Msg *)malloc(sizeof(Msg));
    if (!msg2) {
        perror("Failed to allocate memory for msg2");
        pthread_exit(NULL);
    }
    msg2->value = 2;
    msg2->kind = AQ_ALARM;
    int res = aq_send(q, msg2, AQ_ALARM);
    if (res == AQ_NO_ROOM) {
        printf("Thread 2: Waiting to send alarm message 2\n");
    }
    assert(res == 0 || res == AQ_NO_ROOM);
    printf("Thread 2: Sent alarm message 2\n");
    return NULL;
}

void *receive_alarm(void *arg) {
    void *msg;
    int res = aq_recv(q, &msg);
    assert(res == AQ_ALARM);
    printf("Thread 3: Received alarm message 1\n");
    free(msg); // Free the received message
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
    pthread_create(&t1, NULL, send_alarm_1, NULL);

    // Thread 2: Attempt to send second alarm, should block
    pthread_create(&t2, NULL, send_alarm_2, NULL);

    // Thread 3: Receive the first alarm, unblocking Thread 2
    pthread_create(&t3, NULL, receive_alarm, NULL);

    // Wait for all threads to complete
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    return 0;
}
