#include <bits/stdc++.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <semaphore.h>
#include <unistd.h> //for fork()
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <fcntl.h>


using namespace std;

// Condition Variable to denote eat and produce variables
pthread_cond_t eat;
pthread_cond_t produce;

int item_to_produce, curr_buf_size;
int item_to_consume;
int total_items, max_buf_size, num_workers, num_masters;
pthread_mutex_t M1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t M2 = PTHREAD_MUTEX_INITIALIZER;
int *buffer;

void print_produced(int num, int master) {

  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);
  
}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data) {
  int thread_id = *((int *)data);

  while(1) {

      if(item_to_produce >= total_items) {
	      break;
      }

      // Apply Lock - Critical Section
      pthread_mutex_lock(&M1);

      // Spinlock to avoid other threads from entering critical sections
      while(curr_buf_size == max_buf_size){
      	//Wait for thread to complete
      	pthread_cond_wait(&eat, &M1);
      }
 	
      buffer[curr_buf_size++] = item_to_produce;
      print_produced(item_to_produce, thread_id);
      item_to_produce++;

     // Apply Unlock - Critical Section 
     pthread_mutex_unlock(&M1);

    //Produce Signal to wake other signals
     pthread_cond_signal(&produce);
    }
  return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item
void *generate_eat_loop(void *data) {
  int thread_id = *((int *)data);

  while(1){

    if(item_to_consume >= total_items) {
	    break;
    }
    
    // Apply Lock - Critical Section
    pthread_mutex_lock(&M2);
      
      // Spinlock to avoid other threads from entering critical sections
      while(curr_buf_size == 0) {
      	//Wait for a thread to place in
      	pthread_cond_wait(&produce,&M2);
      }
      
      curr_buf_size--;
      print_consumed(item_to_consume, thread_id);
      
      item_to_consume++;
      
      // Apply Unlock - Critical Section 
      pthread_mutex_unlock(&M2);
      
      //Signal Done eating
      pthread_cond_signal(&eat);
    }
  return 0;
}

int main(int argc, char *argv[]) {
  int i; // For loops
  
  // Pthreads conditions initialized in main()
  pthread_cond_init(&eat,NULL);
  pthread_cond_init(&produce,NULL);

  int *master_thread_id;  // Created array of pointers for masters
  int *worker_thread_id;  // Created array of pointers for workers
  
  pthread_t *master_thread;
  pthread_t *worker_thread; // Created threads and declared for workers
  
  item_to_produce = 0;
  item_to_consume = 0;  // Variable for Item created to eat
  curr_buf_size = 0; // Variable for Buffer Size
  
  if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
    num_workers = atoi(argv[3]);
    num_masters = atoi(argv[4]);
  }

   buffer = (int *)malloc (sizeof(int) * max_buf_size);

   //create master producer threads
   master_thread_id = (int *)malloc(sizeof(int) * num_masters);
   master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;

  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
  
  //create worker consumer threads

  //Using worker_thread_id similar to master_thread_id made above
   worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
   worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
  for (i = 0; i < num_workers; i++)
    worker_thread_id[i] = i;

// Created threads for workers
  for (i = 0; i < num_workers; i++)
    pthread_create(&worker_thread[i], NULL, generate_eat_loop, (void *)&worker_thread_id[i]);
  
  
  //wait for all threads to complete - master
  for (i = 0; i < num_masters; i++){
      pthread_join(master_thread[i], NULL);
      printf("master %d joined\n", i);
  }

  //wait for all threads to complete - worker
  for (i = 0; i < num_workers; i++) {
      pthread_join(worker_thread[i], NULL);
      printf("worker %d joined\n", i);
    }

  /*----Deallocating Buffers---------------------*/
  free(buffer);
  free(master_thread);
  free(worker_thread);
  free(master_thread_id);
  free(worker_thread_id);
  return 0;
}
