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

volatile int C = 0;
int num_threads = 5;
pthread_mutex_t mutexx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t* cond = NULL;

void *incr(void* Args)
{
	int i = *(int*) Args;
	while (1) 
	{
		pthread_mutex_lock(&mutexx);
		if(i != C)
		{
			pthread_cond_wait(&cond[i],&mutexx);
		}
		printf("%d ",i+1);

		if(C < num_threads-1)
		{
			C++;
		}
		else
		{
			C=0;
		}
		pthread_cond_signal(&cond[C]);
		pthread_mutex_unlock(&mutexx);
	}
	return NULL;
}

int main()
{	
	pthread_t *threads;
	volatile int i;
	threads = (pthread_t*) malloc(num_threads * (sizeof(pthread_t)));
	cond = (pthread_cond_t*) malloc(num_threads * (sizeof(pthread_cond_t)));

	for (int i = 0; i < num_threads; i++) {
	    if (pthread_cond_init(&cond[i], NULL) != 0) {
	        perror("pthread_cond_init() error");
	        exit(1);
	    }
	}

	for (i = 0; i < num_threads; ++i)
	{
		int p = pthread_create(threads+i, NULL, incr, (void*)&i);
		cout<<"PTHREAD "<<p<<"\n";
	}

	for(i=0; i<num_threads; i++)
	{
		pthread_join(threads[i],NULL);
	}
	
	printf("Done\n");
	return 0;
}