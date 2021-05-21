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

int counter = 0;
pthread_mutex_t mutex1;

void *increment(void* Args){
    int* i = (int*) Args;
    printf("%d\n", i[0]);
    return NULL;
}

int main(){
    int numberOfThreads = 5;

    for(int j = 0; j < 2; j++){
        pthread_t *threads;
        threads = (pthread_t*) malloc(numberOfThreads * (sizeof(pthread_t)));
    
        for(int i = 0; i < numberOfThreads; i++){
            pthread_create(threads+i, NULL, increment, (void*)&i);
        }
        for(int k = 0; k < numberOfThreads; k++){
            pthread_join(threads[k], NULL);
        }
    }
    
    printf("%d\n", counter);
}