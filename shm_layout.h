
#include "result_queue.h"
#include "request_queue.h"

#define N 10

enum queue_status {UNUSED, USED}; 

typedef struct shm_layout {
    request_queue_t request_queue; 
    result_queue_t result_queues[N]; 
    int32_t queue_state[N];
} shm_layout_t; 