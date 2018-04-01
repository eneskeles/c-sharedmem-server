#include "result_queue.h"
#include "request_queue.h"
#include <semaphore.h>

enum queue_status {
    UNUSED, 
    USED
}; 

typedef struct shm_layout_t {
    request_queue_t request_queue; 
    result_queue_t result_queues[N]; 
    int32_t queue_state[N];
	sem_t index_semaphore;
} shm_layout_t; 
