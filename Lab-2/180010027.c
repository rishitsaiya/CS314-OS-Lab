#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 

void printFork(char str[], int iter, int l){    
    if(iter <= l){
        int idFork = fork();
        if(idFork > 0){ // returned to parent/caller
            printf("%c %d\n", str[iter], idFork);
            sleep(rand()%4 + 1);
            printFork(str, iter+1, l);
        }
    }
    return;
}

int main(){ 
    char strHW[] = "Hello World";
    setbuf(stdout, NULL);
    printFork(strHW, 0, 10);
    return 0; 
}