#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include "shm_layout.h"

#define SENTINEL_VALUE -1

// initializes the shared memory if not initialized yet 
// returns the address of the shared memory that is type of shm_layout_t*
shm_layout_t* init_shm(char *shm_name) {
    int fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    
    if (fd == -1) {
        perror("error during shm_open: %s\n", strerror(errno));
        exit(1);  
    }

    if (ftruncate(fd, sizeof(shm_layout)) == -1) {
        perror("error during ftruncate: %s\n", strerror(errno));
        exit(1);
    }

    void *ptr = mmap(NULL, sizeof(shm_layout), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 

    if (ptr == MAP_FAILED) {
        perror("error during mmap.\n");
        exit(1);
    }

    return (shm_layout_t*)ptr;
}

void handle_request(char *keyword, result_queue *result_queue, FILE *input_file) {
    FILE *input_file = fopen(file_name, "r");  
    char *line; 
    int32_t len; 
    int32_t count = 0; 
    int32_t bytes_read;
    
    while (bytes_read = getline(&line, &len, input_file) != -1) {
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
    free(line);
    int sentinel = SENTINEL_VALUE;
    result_queue_push(result_queue, sentinel);
    fclose(input_file);
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    char *shm_name = argv[1]; 
    char *file_name = argv[2];
    
    shm_layout_t shm = init_shm(shm_name);

    request_queue_t *request_queue = &shm->request_queue;
    init_request_queue(request_queue);

    result_queue_t *result_queues[N];
    
    int i;
    for (i = 0; i < N; ++i) {
        result_queues[i] = &shm->result_queues[i];
        init_result_queue(result_queues[i]);
    }

    // main server loop 
    while (1) {
        if (!request_queue_empty(request_queue)) {
            // get a request that will later be passed to a worker thread 
            request_t request = request_queue_pop(request_queue); 
            int32_t index = request.index;
            char *keyword = request.keyword
            pthread_t tid;

            int create_status = pthread_create(&tid, NULL, &handle_request, &keyword, result_queues[index], file_name);
            
            // if the return value of pthread_create function is not equal to 0 
            // print out an error message
            if (create_status) {
                perror("error during pthread_create: index = %d, keyword = %s\n", index, keyword);
            }
        }
    }

    return 0;
}