#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include "shm_layout.h"

#define SENTINEL_VALUE -1
#define MAX_SEM_NAME 30

int main(int argc, char **argv) {
	char *shm_name = argv[1];
	char *keyword = argv[2];
	char *sem_name = argv[3];

    int fd = shm_open(shm_name, O_RDWR, 0666);

    ftruncate(fd, sizeof(shm_layout_t));

    if (fd == -1) {
        fprintf(stderr, "error during shm_open: %s\n", strerror(errno));
        exit(1);
    } 

    shm_layout_t *shm = (shm_layout_t *) mmap(NULL, sizeof(shm_layout_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
    request_queue_t *request_queue = &shm->request_queue; 

	int request_index = 0;

	sem_wait(&shm->index_semaphore);
	while(1){
		if(request_index == N){
			printf("too many clients started\n");
			sem_post(&shm->index_semaphore);
			return 0;
		}	
		if(shm->queue_state[request_index] == UNUSED){
			shm->queue_state[request_index] = USED;
			break;
		}
		else 
			request_index++;
	}
	sem_post(&shm->index_semaphore);
	// releasing index semaphore
	request_t test_request; 
	set_request(&test_request, request_index, keyword);
	sem_wait(&shm->request_semaphore);
	request_queue_push(request_queue, test_request);
	sem_post(&shm->request_semaphore);

	sem_t *sem;
	char name[MAX_SEM_NAME];
	sprintf(name,"%s%d",sem_name,request_index);
	sem = sem_open(name,O_RDWR);
	
	// read print loop
	while(1) {
		sem_wait(sem);
		int to_print = result_queue_pop(&shm->result_queues[request_index]);
		if(to_print == -1)
			break;
		if(to_print == -2){
			continue;
		}
		printf("%d\n", to_print);
        fflush(stdout);
	}
	
	// releasing request index safely
	sem_wait(&shm->index_semaphore);
	shm->queue_state[request_index] = UNUSED;
	sem_post(&shm->index_semaphore);
    return 0; 
} 