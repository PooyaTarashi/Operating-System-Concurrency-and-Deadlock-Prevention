#include <stdio.h>
#include <dirent.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 10
#define TRUE 1
#define FALSE 0

int MAP_SIZE, NUM_AGENTS;
pthread_mutex_t maplock;
pthread_mutex_t* cellLocks;
char** map;

typedef struct {
    int initialPosition_x;
    int initialPosition_y;
    char* path;
} AgentArgs;

pthread_t* tids;

void* agent(void* args) {
    AgentArgs* agent_args = (AgentArgs*)args;
    int current_x = agent_args->initialPosition_x;
    int current_y = agent_args->initialPosition_y;
    char* path = agent_args->path;

    for (int i = 0; path[i] != '\0'; i++)
    {
        int next_x = current_x;
        int next_y = current_y;

        if (path[i] == 'N') next_x--;
        if (path[i] == 'S') next_x++;
        if (path[i] == 'E') next_y++;
        if (path[i] == 'W') next_y--;

        pthread_mutex_lock(&cellLocks[next_x * MAP_SIZE + next_y]);
        pthread_mutex_lock(&maplock);

        if (next_x >= 0 && next_x < MAP_SIZE && next_y >= 0 && next_y < MAP_SIZE && map[next_x][next_y] == '.')
        {
            map[current_x][current_y] = '.';
            map[next_x][next_y] = 'O';
            current_x = next_x;
            current_y = next_y;
        }

        pthread_mutex_unlock(&maplock);
        pthread_mutex_unlock(&cellLocks[next_x * MAP_SIZE + next_y]);

        sleep(1);
    }

    return NULL;
}

int areEqual(char** a, char** b) {
    for (int i = 0; i < MAP_SIZE; i++)
        for (int j = 0; j < MAP_SIZE; j++)
            if (a[i][j] != b[i][j])
                return FALSE;
    return TRUE;
}

void* renderer(void* args) {
    char** prev_map = malloc(MAP_SIZE * sizeof(char*));
    for (int i = 0; i < MAP_SIZE; i++)
        prev_map[i] = malloc(MAP_SIZE * sizeof(char));

    while (TRUE) {
        pthread_mutex_lock(&maplock);
        if (areEqual(map, prev_map) == FALSE) {
            // system("clear");
            printf("\n");
            for (int i = 0; i < MAP_SIZE; i++) {
                for (int j = 0; j < MAP_SIZE; j++)
                    printf("%c ", map[i][j]);
                printf("\n");
            }
            for (int i = 0; i < MAP_SIZE; i++)
                for (int j = 0; j < MAP_SIZE; j++)
                    prev_map[i][j] = map[i][j];
        }
        pthread_mutex_unlock(&maplock);
    }

    return NULL;
}

int main(int argc, const char* argv[]) {
    scanf(" %d", &MAP_SIZE);
    scanf(" %d", &NUM_AGENTS);
    pthread_mutex_init(&maplock, NULL);

    cellLocks = calloc(MAP_SIZE * MAP_SIZE, sizeof(pthread_mutex_t));
    for (int i = 0; i < MAP_SIZE * MAP_SIZE; i++)
        pthread_mutex_init(&cellLocks[i], NULL);

    map = malloc(MAP_SIZE * sizeof(char*));
    for (int i = 0; i < MAP_SIZE; i++) {
        map[i] = malloc(MAP_SIZE * sizeof(char));
        for (int j = 0; j < MAP_SIZE; j++)
            scanf(" %c", &map[i][j]);
    }

    AgentArgs* agents = malloc(NUM_AGENTS * sizeof(AgentArgs));
    for (int i = 0; i < NUM_AGENTS; i++) {
        int path_length;
        scanf(" %d", &agents[i].initialPosition_x);
        scanf(" %d", &agents[i].initialPosition_y);
        scanf(" %d", &path_length);
        agents[i].path = malloc(path_length * sizeof(char));
        for (int j = 0; j < path_length; j++)
            scanf(" %c", &agents[i].path[j]);
    }

    tids = malloc((NUM_AGENTS + 1) * sizeof(pthread_t));
    pthread_create(&(tids[0]), NULL, &renderer, NULL);

    for (int i = 0; i < NUM_AGENTS; i++)
        pthread_create(&(tids[i + 1]), NULL, &agent, (void*)&agents[i]);

    for (int i = 1; i <= NUM_AGENTS; i++)
        pthread_join(tids[i], NULL);

    return 0;
}