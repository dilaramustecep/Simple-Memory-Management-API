# Simple-Memory-Management-API
In this program we have 
- 10 request threads
- 1 server thread that manages memory allocation request make by request threads
- Shared queue that stores memory allocation requests make by request threads
- thread_message array that stores server threads message to memory allocation request by request threads.
- 150 byte shared memory.

### Program Flow
- Request threads will generate a struct which has the random requested memory size and id. Then struct will be pushed to shared queue. Threads will be blocked on their semaphores.
- When there is an item in the shared queue, server thread will pop the item.
- This popped item contains requesting thread and requested memory size. Server will control the memory to check if there is available space.
- Depending on the previous memory space check,server thread will update thread_message array and unblock the proper threads semaphore.
  -  If there is enough space it should return the start address of the memory array.
  - If there is not enough space it should return -1.
- Unblocked thread will check thread_message array and depending on the value it read, it will update the memory.
  - If space is allocated: Gains access to the shared memory and set all the bytes allocated to it to character value of its id.
  - If space is not allocated: Prints an error message “Thread ID: Not enough memory” and exits.

### Functions
- *thread_function(int id):* This function should run as a thread. It will then call the my_malloc(id, size) function and block until the memory request is handled by the server thread. Once the thread is unblocked by the server thread it will take appro- priate action respect to server thread message to request.
- *my_malloc(int thread_id, int size):* This function should be called by a thread with a size value. It will gain access to the shared queue and write the requesting thread_id and the amount.
- *dump_memory():* This function is used to test memory allocation. It will print the entire contents of the memory array onto the console.
- *server_function():* It will check the queue for the requests and depending on the memory size it should either grant or decline them. The answers to the requests will be written to the thread_message array and the requesting thread should be unblocked
