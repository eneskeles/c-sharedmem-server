#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include "shm_layout.h"

#define SENTINEL_VALUE -1
#define MAX_LINE_LEN 100

typedef struct args_t {
    char *keyword; 
    char *file_name;
    result_queue_t *result_queue;
} args_t;

// initializes the shared memory if not initialized yet 
// returns the address of the shared memory that is type of shm_layout_t*
shm_layout_t* init_shm(char *shm_name) {
    int fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    
    if (fd == -1) {
        fprintf(stderr, "error during shm_open: %s\n", strerror(errno));
        exit(1);  
    }

    if (ftruncate(fd, sizeof(shm_layout_t)) == -1) {
        fprintf(stderr, "error during ftruncate: %s\n", strerror(errno));
        exit(1);
    }

    void *ptr = mmap(NULL, sizeof(shm_layout_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 

    if (ptr == MAP_FAILED) {
        fprintf(stderr, "error during mmap.\n");
        exit(1);
    }

    return (shm_layout_t*)ptr;
}

void *handle_request(void *given_args) {
    args_t *args = (args_t *)given_args;

    // parses arguments
    FILE *input_file = fopen(args->file_name, "r");  
    char *keyword = args->keyword; 
    result_queue_t *result_queue = args->result_queue;

    char line[MAX_LINE_LEN]; 
    char *holder = line;
    size_t len = MAX_LINE_LEN; 
    int32_t count = 0; 
    ssize_t bytes_read;
    
    while ((bytes_read = getline(&holder, &len, input_file) != -1)) {
        count++;
        int i; 
        for (i = 0; i < bytes_read; ++i) {
            int j;
            for (j = 0; keyword[j] && keyword[j] == line[i+j]; ++j);
            if (!keyword[j]) {
                result_queue_push(result_queue, count);
            }
        }
    }
    int sentinel = SENTINEL_VALUE;
    result_queue_push(result_queue, sentinel);
    fclose(input_file);
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    char *shm_name = argv[1]; 
    char *file_name = argv[2];
    
    shm_layout_t *shm = init_shm(shm_name);

    request_queue_t *request_queue = &shm->request_queue;
    printf("%x\n", shm->request_queue);
    request_queue_init(request_queue);

    result_queue_t *result_queues[N];
    
    int i;
    for (i = 0; i < N; ++i) {
        result_queues[i] = &shm->result_queues[i];
        result_queue_init(result_queues[i]);
    }

    // main server loop 
    while (1) {
        if (!request_queue_empty(request_queue)) {
            // get a request that will later be passed to a worker thread 
            request_t request;
            request_queue_pop(request_queue, &request); 
            
            printf("%d", request.index);
            break;
            // create arguments
            args_t *args; 
            args->keyword = request.keyword;
            args->result_queue = result_queues[request.index];
            args->file_name = file_name; 

            pthread_t tid;

            int create_status = pthread_create(&tid, NULL, handle_request, (void *)args);
            
            // if the return value of pthread_create function is not equal to 0 
            // print out an error message
            if (create_status) {
                fprintf(stderr, "error during pthread_create: index = %d, keyword = %s\n", request.index, request.keyword);
            }
        }
    }

    shm_unlink(shm_name);
    return 0;
}