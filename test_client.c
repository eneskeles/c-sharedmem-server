#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include "shm_layout.h"

int main(int argc, char **argv) {
    char *shm_name = argv[1];
    int fd = shm_open(shm_name, O_RDWR, 0666);


    ftruncate(fd, sizeof(shm_layout_t));

    if (fd == -1) {
        fprintf(stderr, "error during shm_open: %s\n", strerror(errno));
        exit(1);
    } 

    shm_layout_t *shm = (shm_layout_t *) mmap(NULL, sizeof(shm_layout_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 

    request_queue_t *request_queue = &shm->request_queue; 
    request_t test_request; 
    set_request(&test_request, 5, "ali");
    request_queue_push(request_queue, test_request);
    return 0; 
} 