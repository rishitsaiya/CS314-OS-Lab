#include <bits/stdc++.h>
#include <cmath>
#include <vector>
#include <chrono>
#include <thread>
#include <semaphore.h>
#include <unistd.h> //for fork()
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <fcntl.h>

#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

using namespace std;

int item_to_produce, item_to_eat, curr_buf_size;
int total_items, max_buf_size, num_workers, num_masters;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t eat,produce;
int *buffer;

void print_produced(int num, int master) {

  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);
  
}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while(1)
    {

      if(item_to_produce >= total_items) {
	break;
      }
      pthread_mutex_lock(&mutex1);
      while(curr_buf_size==max_buf_size)
      {
      	//Wait someone to eat
      	pthread_cond_wait(&eat,&mutex1);
      }
 	
      buffer[curr_buf_size++] = item_to_produce;
      print_produced(item_to_produce, thread_id);
      item_to_produce++;
      //Signal Produced
      
     pthread_mutex_unlock(&mutex1);
     pthread_cond_signal(&produce);
    }
  return 0;
}

//write function to be run by worker threads
void *generate_eat_loop(void *data)
{
  int thread_id = *((int *)data);

  while(1)
    {

      if(item_to_eat >= total_items) {
	break;
      }
      pthread_mutex_lock(&mutex2);
      while(curr_buf_size==0)
      {
      	//Wait for someone to fill
      	pthread_cond_wait(&produce,&mutex2);
      }
      curr_buf_size--;
      print_consumed(item_to_eat, thread_id);
      item_to_eat++;
      //Signal Done eating
      
      pthread_mutex_unlock(&mutex2);
      pthread_cond_signal(&eat);
    }
  return 0;
}
//ensure that the workers call the function print_consumed when they consume an item

int main(int argc, char *argv[])
{
  int *master_thread_id;
  int *worker_thread_id;
  pthread_t *master_thread;
  pthread_t *worker_thread;
  item_to_produce = 0;
  item_to_eat =0;
  curr_buf_size = 0;
  pthread_cond_init(&eat,NULL);
  pthread_cond_init(&produce,NULL);
  int i;
  
   if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
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
   worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
   worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
  for (i = 0; i < num_workers; i++)
    worker_thread_id[i] = i;

  for (i = 0; i < num_workers; i++)
    pthread_create(&worker_thread[i], NULL, generate_eat_loop, (void *)&worker_thread_id[i]);
  
  
  //wait for all threads to complete
  for (i = 0; i < num_masters; i++)
    {
      pthread_join(master_thread[i], NULL);
      printf("master %d joined\n", i);
    }
  
  for (i = 0; i < num_workers; i++)
    {
      pthread_join(worker_thread[i], NULL);
      printf("worker %d joined\n", i);
    }
  /*----Deallocating Buffers---------------------*/
  free(buffer);
  free(master_thread_id);
  free(worker_thread_id);
  free(master_thread);
  free(worker_thread);
  return 0;
}
