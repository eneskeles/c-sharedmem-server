#define BUFSIZE 100

enum queue_signal {
    QUEUE_EMPTY = -2, 
    QUEUE_FULL = -1, 
    SUCCESS
};

typedef struct result_queue_t {
    uint32_t back, front, size;
    uint32_t arr[BUFSIZE]; 
} result_queue_t; 

void result_queue_init(result_queue_t *result_queue) {
    result_queue->back = 0;
    result_queue->front = 0;
    result_queue->size = 0;
}

int32_t result_queue_push(result_queue_t *result_queue, uint32_t item) {
    if (result_queue->size == BUFSIZE) 
        return QUEUE_FULL;

    result_queue->arr[result_queue->back] = item; 
    result_queue->back = (result_queue->back + 1) % BUFSIZE; 
    result_queue->size++; 
    return SUCCESS;
}

int32_t result_queue_pop(result_queue_t *result_queue) {
    if (result_queue->size == 0)
        return QUEUE_EMPTY;
    
    int32_t result = result_queue->arr[result_queue->front];
    result_queue->front = (result_queue->front + 1) % BUFSIZE; 
    result_queue->size--; 
    return result;
}

uint32_t result_queue_empty(result_queue_t *result_queue) {
    return result_queue->size == 0;
}