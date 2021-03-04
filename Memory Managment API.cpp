#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h> 
#include <queue> 
#include <semaphore.h>
#include<vector>
using namespace std;

#define NUM_THREADS 10
#define MEMORY_SIZE 150

struct node
{
	int id;
	int size;

	node(int in_id,int in_size):
	id(in_id),size(in_size){}
};


queue<node> myqueue; // shared que
pthread_mutex_t sharedLock = PTHREAD_MUTEX_INITIALIZER; // mutex
pthread_t server; // server thread handle
sem_t semlist[NUM_THREADS]; // thread semaphores

int thread_message[NUM_THREADS]; // thread memory information
char  memory[MEMORY_SIZE]; // memory size


int INDEX=0;

//This function will add the struct to the queue
void my_malloc(int thread_id, int size)
{
	/*
	FUNCTION CALL CHECK
	printf("Inside the malloc --> Therad id %d and size is  %d\n", thread_id,size);
	*/

	pthread_mutex_lock(&sharedLock);
	node request=node(thread_id,size); // create request based on parameters
	myqueue.push(request);			   // add request to queue 
	pthread_mutex_unlock(&sharedLock);
	
	/*
	QUEUE INSERT CHECK
	printf("Inside the malloc --> Therad id %d and queue size is %d \n",thread_id,myqueue.size());
	*/	
}


// This function should grant or decline a thread depending on memory size.
void * server_function(void *){

	int REQUEST_COUNTER=NUM_THREADS; 
	
	while(REQUEST_COUNTER>=0){
		
		if (myqueue.size()>0) // Check whether there is request
		{
			// Take request
			node temp=myqueue.front();
			int requestedSize=temp.size;
			int requesterId=temp.id;

			// Check to grand a access
			if (MEMORY_SIZE-INDEX>requestedSize){	

								
				thread_message[requesterId]= INDEX;		// Put in to message array

				INDEX=INDEX+requestedSize;				// Modify INDEX 
				myqueue.pop();							// Delete request

				sem_post(&semlist[requesterId]);		// Release the thread
			}
			else{
				thread_message[requesterId]= -1; 		// There is no enough space
				myqueue.pop();							// Delete request
							
				sem_post(&semlist[requesterId]);		// Release the thread
			}

		// Decreament request counter
		REQUEST_COUNTER-=1;
		}
	}

	// kill server thread
	 pthread_exit(&server);
}


// You need to print the whole memory array here.
void dump_memory() 
{
 
	for (int i=0;i<MEMORY_SIZE;i++){
		printf("%c ", memory[i]);
	}
	printf("\n");
}



//This function will create a random size, and call my_malloc
//Block
//Then fill the memory with id's or give an error prompt
void * thread_function(void * id) 
{
	int threadID= *(int *)id;

	int randomSize= rand()%(MEMORY_SIZE/6) + 1; 	// 1. Create a random size  	
	my_malloc(threadID,randomSize);					// 2. Call my_alloc
	sem_wait(&semlist[threadID]); 					// 3. Blocked

	// 4. Server Response Actions

	if (thread_message[threadID]!=-1){				// 4.1 Access granted, Fill the Memory
		int startIndex=thread_message[threadID];

		char c= '0'+threadID;
		for(int i = 0; i<randomSize; i++){
			memory[i+startIndex]=c;
		}
	}
	else{										  // 4.2 Access denied, ERROR Message
		printf("ERROR- ACCESS DENIED  !!! therad id: %d and requested size: %d\n", threadID,randomSize);

	}
}


void init()	 
{
	pthread_mutex_lock(&sharedLock);	//lock
	for(int i = 0; i < NUM_THREADS; i++) //initialize semaphores
	{sem_init(&semlist[i],0,0);}
	for (int i = 0; i < MEMORY_SIZE; i++)	//initialize memory 
  	{char zero = 'x'; memory[i] = zero;}
   	pthread_create(&server,NULL,server_function,NULL); //start server 
	pthread_mutex_unlock(&sharedLock); //unlock
}




int main (int argc, char *argv[])
{
	vector<pthread_t> THREADS(NUM_THREADS);

	// 1. You need to create a thread ID array here
	vector<int> ids(10);
	for(int i=0;i<NUM_THREADS;i++){
		ids[i]=i;
	}


	// 2. Call init
	init();


	// 3. You need to create threads with using thread ID array, using pthread_create()
	for(int i=0;i<NUM_THREADS;i++){
		pthread_create(&THREADS[i],NULL,thread_function,&ids[i]);
	}

 	// 4. You need to join the threads
 	for(int i=0;i<NUM_THREADS;i++){
 		pthread_join(THREADS[i],NULL);
 	}
 
 	// 5. Print out memory
 	dump_memory();

 	// 6. Print out indexes
 	printf("\nMemory Indexes:\n" );
 	for (int i = 0; i < NUM_THREADS; i++)
 	{
 		printf("[%d]" ,thread_message[i]); 
 	}

 	// 7. Terminate
 	printf("\nTerminating...\n");
 	return 0;
 }
 