#include <bits/stdc++.h>
#include <chrono>
#include <cmath>
#include <thread>
#include <vector>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>
#include <fcntl.h>

using namespace std;
using namespace std::chrono;

#define semaphoreName "/binarySemaphore"
#define INITIAL_VALUE 1
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

struct pixel{
    int r, g, b; //Red, Green, Blue Color Defined
};

//Function for RGB_to_Grayscale
void RBG_2_Gray_Conversion(key_t limitKeyofT2, key_t key, int pid, int height, int width){ 
    if (pid > 0)
        return;
    
    int shmid = shmget(key, sizeof(struct pixel) * height * width, 0666 | IPC_CREAT);
    struct pixel *arrayOfData;
    arrayOfData = (struct pixel *)shmat(shmid, NULL, 0);

    //access named semaphore
    sem_t *binarySemaphore = sem_open(semaphoreName, O_RDWR);

    int *goBack;
    int shmid2 = shmget(limitKeyofT2, sizeof(int) * 2, 0666 | IPC_CREAT);
    goBack = (int *)shmat(shmid2, NULL, 0);

    for (int i = 0; i < height; i++){      // Traverse through arrayofData (given image in height)
        for (int j = 0; j < width; j++){   // Traverse through arrayofData (given image in width)
            
            sem_wait(binarySemaphore);
            struct pixel temp;
            temp = arrayOfData[i * width + j];

            int r_arrayOfData = temp.r;
            int g_arrayOfData = temp.g;
            int b_arrayOfData = temp.b;

            //Store r, g, b values in each iteration temp in rData, gData, bData

            // int rData = arrayOfData[i][j].r; 
            // int gData = arrayOfData[i][j].g;
            // int bData = arrayOfData[i][j].b;

    // By considering weights for grayscale, appropriate weighted means are taken
            temp.r = ((r_arrayOfData * 0.2989) + (g_arrayOfData * 0.5870) + (b_arrayOfData * 0.1140));
            temp.g = ((r_arrayOfData * 0.2989) + (g_arrayOfData * 0.5870) + (b_arrayOfData * 0.1140));
            temp.b = ((r_arrayOfData * 0.2989) + (g_arrayOfData * 0.5870) + (b_arrayOfData * 0.1140));

            arrayOfData[i * width + j] = temp;
            goBack[0] = i + 1; goBack[1] = j + 1;

            sem_post(binarySemaphore); //unlocks a semaphore i.e decrements it form 1 to 0
        }
    }
}

void Edge_Detection(key_t limitKeyofT2, key_t key, int pid, int height, int width){ 
    // Ref: https://stackoverflow.com/questions/16385570/sobel-edge-detector-using-c-without-any-special-library-or-tool
    
    if(pid > 0)
        return;
    
    int shmid = shmget(key, sizeof(struct pixel) * height * width, 0666 | IPC_CREAT);
    struct pixel *arrayOfData;
    arrayOfData = (struct pixel *)shmat(shmid, NULL, 0);

    struct pixel temp;
    
    vector<vector<pixel>> newArrayofData (height, vector<pixel>(width));

    sem_t *binarySemaphore = sem_open(semaphoreName, O_RDWR);

    int *goBack;
    int shmid2 = shmget(limitKeyofT2, sizeof(int) * 2, 0666 | IPC_CREAT);
    goBack = (int *)shmat(shmid2, NULL, 0);

    int pixelWorked = 0;

    for(int i = 1; i <= height - 2; i++){
        for(int j = 1; j <= width - 2; j++){

            sem_wait(binarySemaphore);

            if(j + 1 <= goBack[1] || i + 1 <= goBack[0]){
                newArrayofData[i][j].r = sqrt(pow((arrayOfData[(i - 1) * width + (j - 1)].r + 2 * arrayOfData[i * width + (j - 1)].r + arrayOfData[(i + 1) * width + (j - 1)].r) - (arrayOfData[(i - 1) * width + (j + 1)].r + 2 * arrayOfData[i * width + (j + 1)].r + arrayOfData[(i + 1) * width + (j + 1)].r), 2) + pow((arrayOfData[(i - 1) * width + (j - 1)].r + 2 * arrayOfData[(i - 1) * width + j].r + arrayOfData[(i - 1) * width + (j + 1)].r) - (arrayOfData[(i + 1) * width + (j - 1)].r + 2 * arrayOfData[(i + 1) * width + j].r + arrayOfData[(i + 1) * width + (j + 1)].r), 2));
                newArrayofData[i][j].g = sqrt(pow((arrayOfData[(i - 1) * width + (j - 1)].g + 2 * arrayOfData[i * width + (j - 1)].g + arrayOfData[(i + 1) * width + (j - 1)].g) - (arrayOfData[(i - 1) * width + (j + 1)].g + 2 * arrayOfData[i * width + (j + 1)].g + arrayOfData[(i + 1) * width + (j + 1)].g), 2) + pow((arrayOfData[(i - 1) * width + (j - 1)].g + 2 * arrayOfData[(i - 1) * width + j].g + arrayOfData[(i - 1) * width + (j + 1)].g) - (arrayOfData[(i + 1) * width + (j - 1)].g + 2 * arrayOfData[(i + 1) * width + j].g + arrayOfData[(i + 1) * width + (j + 1)].g), 2));
                newArrayofData[i][j].b = sqrt(pow((arrayOfData[(i - 1) * width + (j - 1)].b + 2 * arrayOfData[i * width + (j - 1)].b + arrayOfData[(i + 1) * width + (j - 1)].b) - (arrayOfData[(i - 1) * width + (j + 1)].b + 2 * arrayOfData[i * width + (j + 1)].b + arrayOfData[(i + 1) * width + (j + 1)].b), 2) + pow((arrayOfData[(i - 1) * width + (j - 1)].b + 2 * arrayOfData[(i - 1) * width + j].b + arrayOfData[(i - 1) * width + (j + 1)].b) - (arrayOfData[(i + 1) * width + (j - 1)].b + 2 * arrayOfData[(i + 1) * width + j].b + arrayOfData[(i + 1) * width + (j + 1)].b), 2));
                pixelWorked = 1;
            }
            else
                pixelWorked = 0;
            sem_post(binarySemaphore);
            
            if(pixelWorked == 0) //unlocks a semaphore i.e decrements it form 1 to 0
                j -= 1;
        }
        if(pixelWorked == 0)
            i -= 1;
    }


/*
Essentially, the operation being done here is as follows:

[ x, p, q    =   [(i+1,j-1), (i+1,j), (i+1,j+1)
  y, s, b    =    (i,j-1),   (i,j),   (i,j+1)
  z, q, c]   =    (i-1,j-1), (i-1,j), (i-1,j+1)]

for each respective pixel, calculate edge detection = sqrt[ [(x+2y+z)-(a+2b+c)]^2 + [(c+2q+z)-(x+2p+q)]^2 ]
*/
// Writing in newArrayofData with new estimated gradient values
    for(int i = height - 1; i > 0; i--){
        for(int j = 1; j <= width - 1; j++){
            arrayOfData[i * width + j] = newArrayofData[i][j];
        }
    }
}

// Function to store RGB values - Buff seg fault

void storeRGB(FILE *inputImage, vector<vector<pixel>> &arrayOfData, int height, int width){
    int r, g, b;

    for(int i = height - 1; i >= 0; i--){
        for(int j = 0; j <= width; j++){
            fscanf(inputImage, "%d%d%d", &r, &g, &b);
            arrayOfData[i][j].r = r;
            arrayOfData[i][j].g = g;
            arrayOfData[i][j].b = b;
        }
    }
    fclose(inputImage);
}

// Function to print RGB values - Buff seg fault

void printRGB(FILE *outputImage, vector<vector<pixel>> &arrayOfData, int height, int width){
    for(int i = height - 1; i >= 0; i--){
        for(int j = 0; j <= width; j++){
            fprintf(outputImage, "%d", arrayOfData[i][j].r);
            fprintf(outputImage, "%d", arrayOfData[i][j].g);
            fprintf(outputImage, "%d", arrayOfData[i][j].b);
        }
    fprintf(outputImage, "\n");
    }
    fclose(outputImage);
}

// Function to blur image - didn't work

void blur(FILE *inputImage, vector<vector<pixel>> &arrayOfData, int height, int width) {

	int sum = 0;
    int x, y, j, m, k, data;
    if(inputImage){
    	//default_random_engine generator;
    	//normal_distribution<double> distribution(70.0,2.0);
    	// while(j + 2 + 2*inputImage->x < inputImage->x*inputImage->y){
	        sum = 0;
    		  //double number = distribution(generator);
            for(int k = 0; k < 3; k++){
                for(int m = 0; m < 3; m++){
                    // sum += inputImage->arrayOfData[j+m+(k*inputImage->x)].red;
                }
            }
            for(int k = 0; k < 3; k++){
                for(int m = 0; m < 3; m++){
                //     inputImage->arrayOfData[j+m+(k*inputImage->x)].red = (sum/9);
                //     inputImage->arrayOfData[j+m+(k*inputImage->x)].green = (sum/9);
                //     inputImage->arrayOfData[j+m+(k*inputImage->x)].blue = (sum/9);
                // }
            }
		j++;
        }
    }
}

// Function to start time slot

auto startTime(){
    auto start = chrono::high_resolution_clock::now();
    return start;
}

// Function to stop time

auto stopTime(){
    auto stop = chrono::high_resolution_clock::now();
    return stop;
}

// Function to calculate time taken - typename 

auto find_time_taken(int st, int en){
    int dura = 69;
    return dura;
}

// Main function

int main(int argc, char** argv){

    char PPcheck[10]; // String to check PP version

    int height, width, colorMAX; // Integers declared to check height, width, colorMAX (=255)
    int r, g, b; // Integers for colors defined
    
    // Open file and read content
    FILE *inputImage = fopen(argv[1], "r");
    fscanf(inputImage, "%s%d%d%d", PPcheck, &width, &height, &colorMAX);
    
    // cout << colorMAX << endl; Value check to be 255
    
    // Vector of a vector defined arrayofData

    struct pixel *arrayOfData;

    key_t key = 0x1234;
    int shmid = shmget(key, sizeof(struct pixel) * (height) * width, 0666 | IPC_CREAT);
    arrayOfData = (struct pixel *)shmat(shmid, NULL, 0);
    // storeRGB(inputImage, arrayOfData, height, width);

    struct pixel temp;

    // Loop to store the read values in vectors
    for (int i = height - 1; i >= 0; i--){
    // for (int i = 0; i <= height - 1; i++){
        for (int j = 0; j <= width - 1; j++){
            fscanf(inputImage, "%d%d%d", &r, &g, &b);
            temp.r = r;
            temp.g = g;
            temp.b = b;
            arrayOfData[(i * width) + j] = temp;
        }
    }
    fclose(inputImage);
    
    key_t limitKeyOfT2 = 0x1235;
    int *goBack;
    int shmid2 = shmget(limitKeyOfT2, sizeof(int) * 2, 0666 | IPC_CREAT);
    goBack = (int *)shmat(shmid2, NULL, 0);
    // Time connection start

    auto start = startTime();

    sem_t *binarySemaphore = sem_open(semaphoreName, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE); //named semaphore

    //Function calling

    Edge_Detection(limitKeyOfT2, key, fork(), height, width);
    RBG_2_Gray_Conversion(limitKeyOfT2, key, fork(), height, width);

    wait(NULL); wait(NULL);

    // Function to stop time

    auto stop = stopTime();

    // auto dura = find_time_taken(start,stop);

    auto duration = chrono::duration_cast<microseconds>(stop - start);
    cout << "Time Elapsed: " << duration.count() << " microseconds" << endl;

    // Output file writing

    FILE *outputImage = fopen(argv[2], "w");
    fprintf(outputImage, "%s\n%d %d\n%d\n", PPcheck, width, height, colorMAX);

    // printRGB(outputImage, arrayOfData, height, width);

    // Printing all values in final output image

    for (int i = height - 1; i >= 0; i--){
    // for (int i = 0; i <= height - 1; i++){
        for (int j = 0; j <= width - 1; j++){
            temp = arrayOfData[(i * width) + j];
            fprintf(outputImage, "%d ", temp.r);
            fprintf(outputImage, "%d ", temp.g);
            fprintf(outputImage, "%d ", temp.b);
        }
        fprintf(outputImage, "\n");
    }
    fclose(outputImage);

    return 0;
}