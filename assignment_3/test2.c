#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "aq.h"
#include "aux.h"
static AlarmQueue q;

void *producer(void *arg) {
    // Fill the queue to its initial capacity (4 messages)
    for (int i = 1; i <= 4; i++) {
        put_normal(q, i);
    }

    for (int i = 5; i <= 10; i++) {
        put_normal(q, i);
    }



    return NULL;
}

void *consumer(void *arg) {
    void *msg;
    int ret;
    
    // Retrieve all messages to confirm order and reallocation success
    for (int i = 1; i <= 10; i++) {
        
        get(q);
    }

    return 0;
}

int main() {
    q = aq_create();
    if (q == NULL) {
        printf("Alarm queue could not be created\n");
        return 1;
    }

    pthread_t t1, t2;
    pthread_create(&t1, NULL, producer, NULL);
    pthread_create(&t2, NULL, consumer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
