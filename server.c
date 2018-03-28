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
#define MAX_FILE_NAME 32

shm_layout_t *shm;

typedef struct args_t {
    char keyword[MAX_KEYWORD_LEN]; 
    char file_name[MAX_FILE_NAME];
    uint32_t index;
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
    
    // parse arguments
    FILE *input_file = fopen(args->file_name, "r");  
    result_queue_t *result_queue = &shm->result_queues[args->index];    

    // initialize the result queue
    result_queue_init(result_queue);

    char line[MAX_LINE_LEN]; 
    char *holder = line;
    size_t len = MAX_LINE_LEN; 
    uint32_t line_count = 0; 
    ssize_t bytes_read;
    
    // read line by line until EOF
    while ((bytes_read = getline(&holder, &len, input_file) != -1)) {
        line_count++;        
        int i, j;
        
        // search for the keyword
        for (i = 0; line[i]; ++i) {
            for (j = 0; args->keyword[j] && line[i+j] && args->keyword[j] == line[i+j]; ++j);

            // add the line number to the request queue if the keyword is found
            if (!args->keyword[j]) {
                printf("%d\n", line_count);
                fflush(stdout);
                result_queue_push(result_queue, line_count);
                break;
            }
        }
    }

    uint32_t sentinel = SENTINEL_VALUE;
    result_queue_push(result_queue, sentinel);
    fclose(input_file);
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    char *shm_name = argv[1]; 
    char *file_name = argv[2];
    
    // get a pointer to shared memory
    shm = init_shm(shm_name);

    // initialize the request queue
    request_queue_t *request_queue = &shm->request_queue;
    request_queue_init(request_queue);
    
    // main server loop 
    while (1) {
        if (!request_queue_empty(request_queue)) {
            // get a request that will later be passed to a worker thread 
            request_t request;
            request_queue_pop(request_queue, &request); 

            // create arguments
            args_t *args = malloc(sizeof(args_t)); 
            strcpy(args->keyword, request.keyword);
            strcpy(args->file_name, file_name); 
            args->index = request.index;

            // create the worker thread
            pthread_t tid;
            int create_status = pthread_create(&tid, NULL, handle_request, (void *)args);
            
            // if the return value of pthread_create function is not equal to 0 
            // print out an error message
            if (create_status) {
                fprintf(stderr, "error during pthread_create: index = %d, keyword = %s\n", request.index, request.keyword);
            }
        }
    }

    // delete the shared memory
    shm_unlink(shm_name);
    return 0;
}