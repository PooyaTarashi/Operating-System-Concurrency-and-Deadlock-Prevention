#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define BUFFER_SIZE 5
#define NUM_SENDERS 3
#define NUM_CONSUMERS 2
#define ACK_TIMEOUT 5 

/////////////////////////////////////////
pthread_cond_t ack_conds[NUM_SENDERS]; 
pthread_mutex_t lock_conds[NUM_SENDERS]; 

typedef struct {
    int id;
    int pid; 
    int ack; 
    char message[256];
} Packet;

typedef struct {
    Packet buffer[BUFFER_SIZE];
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;
} CircularBuffer;

CircularBuffer cb;

void* sender(void* arg) {
    int sender_id = *(int*)arg;
    while (1) {
        Packet pkt;
        pkt.id = rand() % 1000;
        snprintf(pkt.message, sizeof(pkt.message), "Message from sender %d", sender_id);
        pkt.pid = sender_id;  
        pkt.ack = 0;

        pthread_mutex_lock(&cb.lock);

        if (cb.count == BUFFER_SIZE)
        {
            printf("Buffer full. Dropping packet with id %d\n", pkt.id);
        }
        else
        {
            cb.buffer[cb.tail] = pkt;
            cb.tail = (cb.tail + 1) % BUFFER_SIZE;
            cb.count++;
            printf("Sender %d sent packet with id %d\n", sender_id, pkt.id);
        }

        pthread_mutex_unlock(&cb.lock);

        struct timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += ACK_TIMEOUT;

        pthread_mutex_lock(&lock_conds[sender_id]);
        int ret = pthread_cond_timedwait(&ack_conds[sender_id], &lock_conds[sender_id], &timeout);

        if (ret == ETIMEDOUT)
            printf("Timeout happened. No consumer sent ACK for packet %d sent from Sender %d\n", pkt.id, pkt.pid);
        else
            printf("Sender %d received ACK for packet with id %d\n", sender_id, pkt.id);

        pthread_mutex_unlock(&lock_conds[sender_id]);

        sleep(rand() % 3);
    }
}

void* consumer(void* arg) {
    while (1) {      
        pthread_mutex_lock(&cb.lock);

        if (cb.count != 0)
        {
            
            Packet pkt = cb.buffer[cb.head];
            cb.head = (cb.head + 1) % BUFFER_SIZE;
            cb.count--;

            printf("Consumer consumed packet with id %d, message: %s\n", pkt.id, pkt.message);
            pkt.ack = 1;
            printf("Consumer sending ACK for packet id %d to sender %d\n", pkt.id, pkt.pid);

            pthread_cond_signal(&ack_conds[pkt.pid]);
        }

        pthread_mutex_unlock(&cb.lock);

        sleep(rand() % 3);
    }
}

int main() {
    cb.head = 0;
    cb.tail = 4;
    cb.count = 5;
    pthread_mutex_init(&cb.lock, NULL);

    pthread_t senders[NUM_SENDERS];
    pthread_t consumers[NUM_CONSUMERS];

    int sender_ids[NUM_SENDERS];
    for (int i = 0; i < NUM_SENDERS; i++) {
        sender_ids[i] = i;
        pthread_cond_init(&ack_conds[i], NULL);
        pthread_mutex_init(&lock_conds[i], NULL);
        pthread_create(&senders[i], NULL, sender, (void*)&sender_ids[i]);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    for (int i = 0; i < NUM_SENDERS; i++) {
        pthread_join(senders[i], NULL);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    for (int i = 0; i < NUM_SENDERS; i++) {
        pthread_mutex_destroy(&lock_conds[i]);
        pthread_cond_destroy(&ack_conds[i]);
    }
    pthread_mutex_destroy(&cb.lock);
    return 0;
}
