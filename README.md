# Shared Memory Server
A simple offline server which searches keywords in a file. This C program written to demonstrate usage of pthreads, shared memory and semaphores under the umbrella of a school project for [CS342 Operating Systems](https://catalog.bilkent.edu.tr/course/c11342.html) course at Bilkent University. Detailed problem description can be found in `description.pdf`.

## Requirements
  - A UNIX based OS which supports pthreads and shared memory.
  - GCC (GNU C Compiler)
  - make
  
## Usage
Build the project:
```bash 
$ make
```
Run the server:
```bash
$ ./server <shared_mem_name> <file_name> <semaphore_name>
```

Run the client:
```bash 
$ ./test_client <shared_mem_name> <keyword> <semaphore_name>
```
