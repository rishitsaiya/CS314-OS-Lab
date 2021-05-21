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

void increment(){
    pthread_mutex_lock(&mutex1); // Lock
    for (int i = 0; i < 10000; i++){
        counter++;
    }
    pthread_mutex_unlock(&mutex1); // unLock
}

int main(){
    pthread_mutex_init(&mutex1, NULL);
    thread t1(increment);
    thread t2(increment);
    thread t3(increment);
    thread t4(increment);
    thread t5(increment);
    thread t6(increment);
    thread t7(increment);
    thread t8(increment);
    thread t9(increment);
    thread t10(increment);

    t1.join(); t2.join(); t3.join(); t4.join(); t5.join(); t6.join(); t7.join(); t8.join(); t9.join(); t10.join();

    printf("%d\n", counter);
}