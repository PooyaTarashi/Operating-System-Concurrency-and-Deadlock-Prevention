#include<stdio.h>
#include<dirent.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include <unistd.h>
#include <time.h>

#define CAR_COUNT_PER_SPAWNER 20
#define MAX_SLEEP 3

enum Movement {
    STAY,
    WTLEFT,
    WTRIGHT,
    WSTRAIGHT,
    TLEFT,
    TRIGHT,
    STRAIGHT
};

enum Movement state[4];

enum Direction{
    NORTH,
    SOUTH,
    EAST,
    WEST
};


pthread_mutex_t available[4];


pthread_mutex_t Intersection_lock;

pthread_mutex_t critical_section_lock;



int rightHand(int id)
{
    switch (id)
    {
    case 0:
        return 3;
        
    case 1:
        return 0;
    
    case 2:
        return 1;

    case 3:
        return 2;
    }
}

int leftHand(int id)
{
    switch (id)
    {
    case 0:
        return 1;
        
    case 1:
        return 2;
    
    case 2:
        return 3;

    case 3:
        return 0;
    }
}

int frontSide(int id)
{
    switch (id)
    {
    case 0:
        return 2;
        
    case 1:
        return 3;
    
    case 2:
        return 0;

    case 3:
        return 1;
    }
}



void test(int i, enum Movement proposed_state)
{
    if ((state[i] == WTLEFT && (state[rightHand(i)] != STRAIGHT && state[rightHand(i)] != TLEFT) && (state[frontSide(i)] != TLEFT && state[frontSide(i)] != STRAIGHT && state[frontSide(i)] != TRIGHT) && (state[leftHand(i)] != TLEFT && state[leftHand(i)] != STRAIGHT)) ||
        (state[i] == WSTRAIGHT && (state[rightHand(i)] != STRAIGHT && state[rightHand(i)] != TLEFT && state[rightHand(i)] != TRIGHT) && (state[frontSide(i)] != TLEFT) && (state[leftHand(i)] != STRAIGHT && state[leftHand(i)] != TLEFT)) ||
        (state[i] == WTRIGHT && (state[frontSide(i)] != TLEFT) && (state[leftHand(i)] != STRAIGHT && state[leftHand(i)] != TLEFT)))
    {
        state[i] = proposed_state;
        pthread_mutex_unlock(&available[i]);
    }
    
}

// For handeling rightHand turn logic
void turn_right(int id, int direction){    
    
    // take:
    pthread_mutex_lock(&critical_section_lock);
    state[direction] = WTRIGHT;
    test(direction, TRIGHT);
    pthread_mutex_unlock(&critical_section_lock);
    pthread_mutex_lock(&available[direction]);
    
    
    // Improve this Function
    pthread_mutex_lock(&Intersection_lock);
    switch (direction){
        case NORTH:
            printf("car with id %d with source %d is turning rightHand from North to West\n", id, direction);
            sleep(rand() % MAX_SLEEP);
            break;
        case SOUTH:
            printf("car with id %d with source %d is turning rightHand from South to East\n", id, direction);
            sleep(rand() % MAX_SLEEP);
            break;
        case EAST:
            printf("car with id %d with source %d is turning rightHand from East to North\n", id, direction);
            sleep(rand() % MAX_SLEEP);
            break;
        case WEST:
            printf("car with id %d with source %d is turning rightHand from West to South\n", id, direction);
            sleep(rand() % MAX_SLEEP);
            break;
        default:
            break;
    }
    pthread_mutex_unlock(&Intersection_lock);



    // put:
    pthread_mutex_lock(&critical_section_lock);
    state[direction] = STAY;
    test(leftHand(direction), STRAIGHT);
    test(leftHand(direction), TLEFT);
    test(frontSide(direction), TLEFT);
    pthread_mutex_unlock(&critical_section_lock);
}


// For handeling Streight logic
void go_straight(int id, int direction){ 
    
    // take:
    pthread_mutex_lock(&critical_section_lock);
    state[direction] = WSTRAIGHT;
    test(direction, STRAIGHT);
    pthread_mutex_unlock(&critical_section_lock);
    pthread_mutex_lock(&available[direction]);
    
    // Improve this function
    pthread_mutex_lock(&Intersection_lock);   
    switch (direction){
        case NORTH:
            printf("car with id %d with source %d is going straight from North to South\n", id, direction);
            sleep(rand() % MAX_SLEEP);
            break;
        case SOUTH:
            printf("car with id %d with source %d is going straight from South to Norh\n", id, direction);
            sleep(rand() % MAX_SLEEP);
            break;
        case EAST:
            printf("car with id %d with source %d is going straight East to West\n", id, direction);
            sleep(rand() % MAX_SLEEP);
            break;
        case WEST:
            printf("car with id %d with source %d is going straight West to East\n", id, direction);
            sleep(rand() % MAX_SLEEP);
            break;
        default:
            break;
    }
    pthread_mutex_unlock(&Intersection_lock);



    // put:
    pthread_mutex_lock(&critical_section_lock);
    state[direction] = STAY;
    test(leftHand(direction), STRAIGHT);
    test(leftHand(direction), TLEFT);
    test(rightHand(direction), STRAIGHT);
    test(rightHand(direction), TRIGHT);
    test(rightHand(direction), TLEFT);
    test(frontSide(direction), TLEFT);
    pthread_mutex_unlock(&critical_section_lock);
}


// For handeling leftHand turn logic
void turn_left(int id, int direction){ 
    // think done

    // take:
    pthread_mutex_lock(&critical_section_lock);
    state[direction] = WTLEFT;
    test(direction, TLEFT);
    pthread_mutex_unlock(&critical_section_lock);
    pthread_mutex_lock(&available[direction]);


    // eat:
    // Improve this function
    pthread_mutex_lock(&Intersection_lock);   
    switch (direction){
        case NORTH:
            printf("car with id %d with source %d is turning leftHand from North to East\n", id, direction);
            sleep(rand() % MAX_SLEEP);
            break;
        case SOUTH:
            printf("car with id %d with source %d is turning leftHand from South to West\n", id, direction);
            sleep(rand() % MAX_SLEEP);
            break;
        case EAST:
            printf("car with id %d with source %d is turning leftHand East to South\n", id, direction);
            sleep(rand() % MAX_SLEEP);
            break;
        case WEST:
            printf("car with id %d with source %d is turning leftHand West to North\n", id, direction);
            sleep(rand() % MAX_SLEEP);
            break;
        default:
            break;
    }
    pthread_mutex_unlock(&Intersection_lock);

    // put:
    pthread_mutex_lock(&critical_section_lock);
    state[direction] = STAY;
    test(leftHand(direction), STRAIGHT);
    test(leftHand(direction), TLEFT);
    test(rightHand(direction), STRAIGHT);
    test(rightHand(direction), TRIGHT);
    test(rightHand(direction), TLEFT);
    test(frontSide(direction), STRAIGHT);
    test(frontSide(direction), TLEFT);
    test(frontSide(direction), TRIGHT);
    pthread_mutex_unlock(&critical_section_lock);
}

typedef struct{
    int source; 
    int count;
} SpawnerArgs;

// DO NOT CHANGE - Spawns cars from a given direction in the intersection
void* car_spawner(void* args){  
    SpawnerArgs* spawner_args = (SpawnerArgs*)args;
    int source = spawner_args->source;
    int count = spawner_args->count;
    for(int i = 0; i < count; i++){
        int move = rand() % 3;

        switch (move){
            case 0:
                turn_right(i, source);
                break;
            case 1:
                go_straight(i, source);
                break;
            case 2:
                turn_left(i, source);
                break;
            default:
                break;
        }
    }
}

// DO NOT CHANGE - Simulation Logic
int main (){
    pthread_t spawner_tid[4];
    SpawnerArgs args[4];
    clock_t time;
    time = clock();
    for(int i = 0; i < 4; i++){
        args[i].source = i;
        args[i].count = CAR_COUNT_PER_SPAWNER;
        pthread_create(&spawner_tid[i], NULL, car_spawner, (void*)&args[i]);
    }
    for(int i = 0; i < 4; i++)
        pthread_join(spawner_tid[i], NULL);
    time = clock() - time;
    double runtime = ((double)time);
    printf("the simulation took %f seconds\n", runtime);
    return 0;
}