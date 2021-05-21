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

sem_t s;
int Sum = 0;

void p1(key_t key, int create_pipe[2], int pid)
{
	if(pid>0)
	{
		return;
	}
	int shmid = shmget(key, sizeof(int)*2, 0666 | IPC_CREAT);
    int *x;
    x = (int *)shmat(shmid, NULL, 0);
    int to_give[1];
    to_give[0]= x[0]*100;
    write(create_pipe[1], to_give, sizeof(int));
}

void p2(key_t key, int create_pipe[2], int pid)
{
	if(pid>0)
	{
		return;
	}
	int shmid = shmget(key, sizeof(int)*2, 0666 | IPC_CREAT);
    int *x;
    x = (int *)shmat(shmid, NULL, 0);
    int to_take[1];
    read(create_pipe[0], to_take, sizeof(int));
    x[1] = 3*to_take[0];
}

int main()
{
	sem_init(&s, 0, 1);
	key_t key = ftok("shmfile",65);
    int shmid = shmget(key, sizeof(int)*2, 0666 | IPC_CREAT);
	int *x;
	x = (int*)shmat(shmid, NULL, 0);
	x[0] = 5;
	printf("%d\n",x[0]);

	int create_pipe[2];
    int data;
    data = pipe(create_pipe);
    if (data == -1)
    {
        perror("pipe");
    }

	p2(key, create_pipe, fork()); 
	p1(key, create_pipe, fork());
	 

    wait(NULL);
    wait(NULL);

	printf("%d-%d\n",x[0],x[1]);
	shmdt(x);
    shmctl(shmid,IPC_RMID,NULL);
}