#include <stdint.h>
#define N 100 

enum queue_signal {QUEUE_EMPTY = -2, QUEUE_FULL = -1, SUCCESS};

typedef struct request_t {
    int32_t index; 
    char* keyword;
} request_t;

typedef struct request_queue_t {
    int32_t back, front, size;
    request_t arr[N]; 
} request_queue_t;

void request_queue_init(request_queue_t *request_queue) {
    request_queue->back = 0;
    request_queue->front = 0;
    request_queue->size = 0;
}

int32_t request_queue_push(request_queue_t *request_queue, int32_t index, char *keyword) {
    if (request_queue->size == N)
        return (int32_t)QUEUE_FULL;
    
    req_queue_t *new_item = request_queue->arr[request_queue->back]; 
    new_item->index = index;
    new_item->keyword = keyword;
    request_queue->back = (request_queue->back + 1) % N; 
    request_queue->size++; 
    return (int32_t)SUCCESS;
}

request_t request_queue_pop(request_queue *request_queue) {
    if (request_queue->size == 0)
        return (int32_t)QUEUE_EMPTY;
    
    request_t result = request_queue->arr[request_queue->front]; 
    request_queue->front = (request_queue->front + 1) % N;
    request_queue->size--;

    return result;
}

int32_t request_queue_empty(request_queue_t *request_queue) {
    return request_queue->size == 0;
}