# Introduction
An assignment given as part of the Operating Systems (fall 2024) course - implementing a queue.

The goal is to gain experience with threads. We create a generic concurrent FIFO queue that supports enqueue and dequeue operations.

## The queue module
We implement the following library in a file named queue.c. The following details the specification of
the library.

- ```void initQueue(void);```\
This function is called before the queue is used. This is your chance to initialize your data structure.
- ```void destroyQueue(void);```\
This function is used for cleanup when the queue is no longer needed. It is possible for
initQueue to be called afterwards.
- ```void enqueue(void*);```\
Adds an item to the queue.
- ```void* dequeue(void);```\
Remove an item from the queue. Will block if empty.
- ```bool tryDequeue(void**);```\
Try to remove an item from the queue. If succeeded, return it via the argument and return true. If the queue is empty, return false and leave the pointer unchanged.
- ```size_t size(void);```\
Return the current amount of items in the queue.
- ```size_t waiting(void);```\
Return the current amount of threads waiting for the queue to fill. This call should not be
blocked due to concurrent operations.
- ```size_t visited(void);```\
Return the amount of items that have passed inside the queue (i.e., inserted and then removed).
This should not block due to concurrent operations.

# Usage
```
gcc -O3 -D_POSIX_C_SOURCE=200809 -Wall -std=c11 -pthread -c queue.c
```