#define N 100 

#include "request.h"

typedef struct request_queue_t {
    uint32_t back, front, size;
    request_t arr[N]; 
} request_queue_t;

void request_queue_init(request_queue_t *request_queue) {
    request_queue->back = 0;
    request_queue->front = 0;
    request_queue->size = 0;
}

int32_t request_queue_push(request_queue_t *request_queue, request_t request) {
    if (request_queue->size == N)
        return QUEUE_FULL;
    
    set_request(&request_queue->arr[request_queue->back], request.index, request.keyword);
    request_queue->back = (request_queue->back + 1) % N; 
    request_queue->size++; 
    return SUCCESS;
}

// do not use this method without checking if the queue is empty
void request_queue_pop(request_queue_t *request_queue, request_t *result) {
    set_request(result, request_queue->arr[request_queue->front].index, 
                         request_queue->arr[request_queue->front].keyword); 
    request_queue->front = (request_queue->front + 1) % N;
    request_queue->size--;
}

uint32_t request_queue_empty(request_queue_t *request_queue) {
    return request_queue->size == 0;
}