// wait(semaphore_stage1)
// critical section of stage 1
// wait(semaphore_buffer_between_1_and_2)
// print("Added to buffer between 1 and 2")
// signal(semaphore_stage1)
// wait(semaphore_stage2)
// signal(semaphore_buffer_between_1_and_2)
// critical of stage 2
// wait(semaphore_buffer_between_2_and_3)
// print("Added to buffer between 1 and 2")
// signal(semaphore_stage2)
// wait(semaphore_stage3)
// signal(semaphore_buffer_between_2_and_3)
// critical of stage 3
// signal(semaphore_stage3)


#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

sem_t stage1_sem;
sem_t stage2_sem;
sem_t stage3_sem;
sem_t buffer1_2_sem;
sem_t buffer2_3_sem;

void* product(void* arg)
{
    int thread_id = *(int*)arg;

    sem_wait(&stage1_sem);
    printf("Process %d: Assembling...\n", thread_id);
    sleep(1);
    printf("Process %d: Assembled.\n", thread_id);
    sem_wait(&buffer1_2_sem);
    printf("Process %d: Waiting in buffer 1.\n", thread_id);
    sem_post(&stage1_sem);


    sem_wait(&stage2_sem);
    sem_post(&buffer1_2_sem);
    printf("Process %d: Quality checking...\n", thread_id);
    sleep(1);
    printf("Process %d: Quality checked.\n", thread_id);
    sem_wait(&buffer2_3_sem);
    printf("Process %d: Waiting in buffer 2.\n", thread_id);
    sem_post(&stage2_sem);

    sem_wait(&stage3_sem);
    sem_post(&buffer2_3_sem);
    printf("Process %d: Packaging...\n", thread_id);
    sleep(1);
    printf("Process %d: Packaged.\n", thread_id);
    sem_post(&stage3_sem);

    return NULL;
}

int main() {
    const int NUM_THREADS = 20;
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    sem_init(&stage1_sem, 0, 4);
    sem_init(&stage2_sem, 0, 3);
    sem_init(&stage3_sem, 0, 2);
    sem_init(&buffer1_2_sem, 0, 5);
    sem_init(&buffer2_3_sem, 0, 3);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, product, &thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&stage1_sem);
    sem_destroy(&stage2_sem);
    sem_destroy(&stage3_sem);
    sem_destroy(&buffer1_2_sem);
    sem_destroy(&buffer2_3_sem);
}