#include <pthread.h>
#include <stdio.h>

pthread_mutex_t locks[10];
int shared_counter[10];

void *myThread(void *id)
{
	for(int i=0; i<1000; i++){
		pthread_mutex_lock(&locks[*(int *)id]);
		shared_counter[*(int *)id]++;
		pthread_mutex_unlock(&locks[*(int *)id]);
	}
}

int main()
{
	pthread_t tid[10];
	int index[10];
	for(int i=0;i<10;i++){
		pthread_mutex_init(&locks[i],NULL);
		shared_counter[i] = 0;
		index[i] = i;
	}


	for(int i=0;i<10; i++){
		pthread_create(&tid[i], NULL, myThread, (void *)&index[i]);    	
    }

    for(int j=0; j<1000; j++)
	{
		for(int i=0;i<10;i++){
			pthread_mutex_lock(&locks[i]);
			shared_counter[i]++;
			pthread_mutex_unlock(&locks[i]);
		}		
	}

    for(int i=0;i<10;i++){
    	pthread_join(tid[i], NULL);
    	pthread_mutex_destroy(&locks[i]);
    }

    //pthread_mutex_destroy()
	for(int i=0;i<10;i++){
		printf("Counter[%d]: ", i);
		printf("%d\n", shared_counter[i]);	
	}
	
	
}

