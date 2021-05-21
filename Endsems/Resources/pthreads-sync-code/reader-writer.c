#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


struct read_write_lock
{
	// If required, use this strucure to create
	// reader-writer lock related variables
	pthread_mutex_t lock;
	pthread_cond_t read_cond;
	pthread_cond_t write_cond;
	//pthread_cond_t pref_cond;
	int nreaders;
	int nwriters;
	int nwaiting;


}rwlock;

struct thread_args
{
	int t_delay;
	int t_id;
};

long int data = 0;			//	Shared data variable

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
	pthread_mutex_init(&rw->lock, NULL);
	pthread_cond_init(&rw->read_cond, NULL);
	pthread_cond_init(&rw->write_cond, NULL);
	rw->nreaders = 0;
	rw->nwriters = 0;
	rw->nwaiting = 0;

}


// The pthread based reader lock
void ReaderLock(struct read_write_lock * rw)
{
	//Wait on the writer cond
	pthread_mutex_lock(&rw->lock);
	while(rw->nwriters > 0)
		pthread_cond_wait(&rw->read_cond,&rw->lock);
	while(rw->nwaiting > 0)
		pthread_cond_wait(&rw->read_cond,&rw->lock);
	rw->nreaders++;
	pthread_mutex_unlock(&rw->lock);
}	

// The pthread based reader unlock
void ReaderUnlock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&rw->lock);
	rw->nreaders--;
	//if(rw->nwaiting > 0)
	//	pthread_cond_signal(rw->write_cond);
	if(rw->nreaders == 0)
		pthread_cond_signal(&rw->write_cond);
	pthread_mutex_unlock(&rw->lock);
}

// The pthread based writer lock
void WriterLock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&rw->lock);
	rw->nwaiting++;
	while(rw->nreaders > 0 || rw->nwriters > 0){
		
		pthread_cond_wait(&rw->write_cond,&rw->lock);
	}
	rw->nwaiting--;
	rw->nwriters ++;
	pthread_mutex_unlock(&rw->lock);

}

// The pthread based writer unlock
void WriterUnlock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&rw->lock);
	rw->nwriters--;
	if(rw->nwaiting > 0)
		pthread_cond_signal(&rw->write_cond);
	else
		pthread_cond_broadcast(&rw->read_cond);
	pthread_mutex_unlock(&rw->lock);
}

//	Call this function to delay the execution by 'delay' ms
void delay(int delay)
{
	struct timespec wait;

	if(delay <= 0)
		return;

	wait.tv_sec = delay / 1000;
	wait.tv_nsec = (delay % 1000) * 1000 * 1000;
	nanosleep(&wait, NULL);
}

// The pthread reader start function
void *ReaderFunction(void *args)
{
	//	Delay the execution by arrival time specified in the input
	struct thread_args * targs = (struct thread_args *) args;
	delay(targs->t_delay);
	
	//	....
	
	//  Get appropriate lock
	//	Display  thread ID and value of the shared data variable
	//
    //  Add a dummy delay of 1 ms before lock release  
	//	....

	ReaderLock(&rwlock);
	printf("Reader %d, data: %ld\n",targs->t_id,data);
	delay(1);
	ReaderUnlock(&rwlock);
}

// The pthread writer start function
void *WriterFunction(void *args)
{
	//	Delay the execution by arrival time specified in the input
	struct thread_args * targs = (struct thread_args *) args;
	delay(targs->t_delay);
	
	//	....
	//
	//  Get appropriate lock
	//	Increment the shared data variable
	//	Display  thread ID and value of the shared data variable
	//
    //  Add a dummy delay of 1 ms before lock release  
	//	....
	WriterLock(&rwlock);
	data++;
	printf("Writer %d, data: %ld\n",targs->t_id,data);
	delay(1);
	WriterUnlock(&rwlock);
	
}

int main(int argc, char *argv[])
{
	pthread_t *threads;
	//struct argument_t *arg;
	
	int reader_number = 0;
	int writer_number = 0;
	long int thread_number = 0;
	long int total_readers = 0;
	long int total_writers = 0;
	long int total_threads = 0;	
	
	int count = 0;			// Number of 3 tuples in the inputs.	Assume maximum 10 tuples 
	int rws[10];			
	int nthread[10];
	int delay[10];

	//	Verifying number of arguments
	if(argc<4 || (argc-1)%3!=0)
	{
		printf("reader-writer <r/w> <no-of-threads> <thread-arrival-delay in ms> ...\n");		
		printf("Any number of readers/writers can be added with repetitions of above mentioned 3 tuple \n");
		exit(1);
	}

	//	Reading inputs
	for(int i=0; i<(argc-1)/3; i++)
	{
		char rw[2];
		strcpy(rw, argv[(i*3)+1]);

		if(strcmp(rw, "r") == 0 || strcmp(rw, "w") == 0)
		{
			if(strcmp(rw, "r") == 0)
				rws[i] = 1;					// rws[i] = 1, for reader
			else
				rws[i] = 2;					// rws[i] = 2, for writer
			
			nthread[i] = atol(argv[(i*3)+2]);		
			delay[i] = atol(argv[(i*3)+3]);

			count ++;						//	Number of tuples
			total_threads += nthread[i];	//	Total number of threads
			if(rws[i] == 1){
				total_readers = total_readers + nthread[i];
			}
			else total_writers = total_writers + nthread[i];
		}
		else
		{
			printf("reader-writer <r/w> <no-of-threads> <thread-arrival-delay in ms> ...\n");
			printf("Any number of readers/writers can be added with repetitions of above mentioned 3 tuple \n");
			exit(1);
		}
	}

	//	Create reader/writer threads based on the input and read and write.
	pthread_t reader_tid[total_readers];
	pthread_t writer_tid[total_writers];
    struct thread_args reader_threads[total_readers];
    struct thread_args writer_threads[total_writers];
    InitalizeReadWriteLock(&rwlock);

	for(int i=0;i<count;i++){
		if(rws[i] ==1 ){
			//reader
			for(int j=0;j<nthread[i];j++){
				reader_threads[reader_number + j].t_delay = delay[i];
				reader_threads[reader_number + j].t_id = reader_number  + j + 1;
				pthread_create(&reader_tid[reader_number+j],NULL,ReaderFunction,(void *)&reader_threads[reader_number+j]);
			}
			reader_number += nthread[i];
		}
		else{
			//writer
			for(int j=0;j<nthread[i];j++){
				writer_threads[writer_number + j].t_delay = delay[i];
				writer_threads[writer_number + j].t_id = writer_number  + j + 1;
				pthread_create(&writer_tid[writer_number+j],NULL,WriterFunction,(void *)&writer_threads[writer_number+j]);
			}
			writer_number += nthread[i];
		}
	}

	//  Clean up threads on exit
	for(int i=0;i<total_readers;i++){
		pthread_join(reader_tid[i],NULL);
	}
	for(int i=0;i<total_writers;i++){
		pthread_join(writer_tid[i],NULL);
	}

	exit(0);
}
