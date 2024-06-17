#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define PQUEUE_IMPLEMENTATION
#include "../common/pqueue.h"

#define LINE_MAX 256
#define LETTERS 26

int stepcmp(const void* a, const void* b) {
    return *(char*)b - *(char*)a;
}

typedef struct {
    int8_t req_cnt;
    bool req[LETTERS];
} Step;

typedef struct {
    uint8_t timer;
    char task;
} Worker;

void push_tasks(Step* steps, Pq queue, char task) {
    for (int i = 0; i < LETTERS; ++i) {
        if (steps[i].req[task - 'A']) {
            steps[i].req[task - 'A'] = false;
            if (--steps[i].req_cnt == 0)
                pq_push(queue, (char) { i + 'A' });
        }
    }
}

int main() {
    FILE* input = fopen("data/day_07.txt", "r");

    Step steps[LETTERS] = { 0 };
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        strtok(buffer, " ");
        char req = strtok(NULL, " ")[0];
        for (int i = 0; i < 5; ++i) strtok(NULL, " ");
        char step = strtok(NULL, " ")[0];
        steps[step - 'A'].req[req - 'A'] = true;
        ++steps[step - 'A'].req_cnt;
    }
    fclose(input);

    Step steps_workers[LETTERS] = { 0 };
    memcpy(steps_workers, steps, sizeof(steps));

    Pq queue = pq_create(sizeof(char), stepcmp);
    Pq queue_workers = pq_create(sizeof(char), stepcmp);
    for (int i = 0; i < LETTERS; ++i) {
        if (steps[i].req_cnt == 0) {
            pq_push(queue, (char) { i + 'A' });
            pq_push(queue_workers, (char) { i + 'A' });
        }
    }

    char order[LETTERS + 1] = { 0 };
    char* order_p = order;
    while (pq_length(queue)) {
        char task;
        pq_pop(queue, &task);
        *(order_p++) = task;
        push_tasks(steps, queue, task);
    }
    pq_free(queue);

    Worker workers[5] = { 0 };
    int minutes = -1;
    uint8_t remaining_tasks = strlen(order);
    while (remaining_tasks) {
        for (int i = 0; i < 5; ++i) {
            if (workers[i].timer > 0)
                --workers[i].timer;
            else {
                char task = workers[i].task;
                if (task != 0) {
                    push_tasks(steps_workers, queue_workers, task);
                    workers[i].task = 0;
                    --remaining_tasks;
                }
                if (pq_length(queue_workers)) {
                    char new_task;
                    pq_pop(queue_workers, &new_task);
                    workers[i].task = new_task;
                    workers[i].timer = 60 + new_task - 'A';
                }
            }
        }
        ++minutes;
    }
    pq_free(queue_workers);

    printf("Part 1: %s\n", order);
    printf("Part 2: %d\n", minutes);
}