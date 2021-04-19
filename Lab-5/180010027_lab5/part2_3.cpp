#include <bits/stdc++.h>
#include <cmath>
#include <vector>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <sys/wait.h>
#include <atomic>

using namespace std;
using namespace std::chrono;

struct pixel{
    int r, g, b; //Pixel colors
};

struct head{
    int height;
    int width;
    int colorMAX;
    char PPcheck[100];
};

void RBG_2_Gray_Conversion(char *II, int pid, int pipefds[2], int pipefds2[2]){
    
    if (pid > 0)
        return;

    //open ppm file and read
    int height, width, colorMAX;
    char PPcheck[100];
    int r, g, b;

    FILE *inputImage = fopen(II, "r");

    struct head storeHeader[1];

    fscanf(inputImage, "%s%d%d%d", storeHeader[0].PPcheck, &width, &height, &colorMAX);
    storeHeader[0].height = height;
    storeHeader[0].width = width;
    storeHeader[0].colorMAX = colorMAX;
    write(pipefds2[1], storeHeader, sizeof(struct head));

    vector<vector<pixel>> arrayOfData(height, vector<pixel>(width));

    for (int i = height - 1; i >= 0; i--){
        for (int j = 0; j < width; j++){
            fscanf(inputImage, "%d%d%d", &r, &g, &b);
            arrayOfData[i][j].r = r;
            arrayOfData[i][j].g = g;
            arrayOfData[i][j].b = b;
        }
    }
    fclose(inputImage);

    struct pixel sendPixel[9];
    // cout << "a" << endl;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int arrayOfData_red = arrayOfData[i][j].r;
            int arrayOfData_green = arrayOfData[i][j].g;
            int arrayOfData_blue = arrayOfData[i][j].r;
            arrayOfData[i][j].r = (arrayOfData_red * 0.299) + (arrayOfData_green * 0.587) + (arrayOfData_blue * 0.114);
            arrayOfData[i][j].g = (arrayOfData_red * 0.299) + (arrayOfData_green * 0.587) + (arrayOfData_blue * 0.114);
            arrayOfData[i][j].b = (arrayOfData_red * 0.299) + (arrayOfData_green * 0.587) + (arrayOfData_blue * 0.114);
        }
        // cout << "d" << endl;
    }

    for (int i = 0; i <= height - 3; i += 3){
        for (int j = 0; j <= width - 3; j += 3){
            
            sendPixel[0] = arrayOfData[i][j];
            sendPixel[1] = arrayOfData[i][j + 1];
            sendPixel[2] = arrayOfData[i][j + 2];

            sendPixel[3] = arrayOfData[i + 1][j];
            sendPixel[4] = arrayOfData[i + 1][j + 1];
            sendPixel[5] = arrayOfData[i + 1][j + 2];

            sendPixel[6] = arrayOfData[i + 2][j];
            sendPixel[7] = arrayOfData[i + 2][j + 1];
            sendPixel[8] = arrayOfData[i + 2][j + 2];

            write(pipefds[1], sendPixel, sizeof(sendPixel));
        }
    }
    // cout << "b" << endl;
    exit(0);
}

void Edge_Detection(char *DI, int pid, int pipefds[2], int pipefds2[2]){
    if (pid > 0)
        return;
    //for the pipe
    struct pixel readPixel[9];

    struct head storeHeader1[1];
    read(pipefds2[0], storeHeader1, sizeof(storeHeader1));

    vector<vector<pixel>> arrayOfData(storeHeader1[0].height, vector<pixel>(storeHeader1[0].width));
    vector<vector<pixel>> newarrayOfData(storeHeader1[0].height, vector<pixel>(storeHeader1[0].width));

    // difference edge detection
    int T2_i = 1; int T2_j = 1;

    int maxt2i = 0;
    int maxt2j = 0;

    for (int i = 0; i <= storeHeader1[0].height - 3; i = i + 3){
        T2_j = 1;
        for (int j = 0; j <= storeHeader1[0].width - 3; j = j + 3){
            
            read(pipefds[0], readPixel, sizeof(readPixel));
            arrayOfData[i][j] = readPixel[0];
            arrayOfData[i][j + 1] = readPixel[1];
            arrayOfData[i][j + 2] = readPixel[2];

            arrayOfData[i + 1][j] = readPixel[3];
            arrayOfData[i + 1][j + 1] = readPixel[4];
            arrayOfData[i + 1][j + 2] = readPixel[5];

            arrayOfData[i + 2][j] = readPixel[6];
            arrayOfData[i + 2][j + 1] = readPixel[7];
            arrayOfData[i + 2][j + 2] = readPixel[8];

            newarrayOfData[T2_i][T2_j].r = sqrt(pow((arrayOfData[T2_i - 1][T2_j - 1].r + 2 * arrayOfData[T2_i][T2_j - 1].r + arrayOfData[T2_i + 1][T2_j - 1].r) - (arrayOfData[T2_i - 1][T2_j + 1].r + 2 * arrayOfData[T2_i][T2_j + 1].r + arrayOfData[T2_i + 1][T2_j + 1].r), 2) + pow((arrayOfData[T2_i - 1][T2_j - 1].r + 2 * arrayOfData[T2_i - 1][T2_j].r + arrayOfData[T2_i - 1][T2_j + 1].r) - (arrayOfData[T2_i + 1][T2_j - 1].r + 2 * arrayOfData[T2_i + 1][T2_j].r + arrayOfData[T2_i + 1][T2_j + 1].r), 2));
            newarrayOfData[T2_i][T2_j].g = sqrt(pow((arrayOfData[T2_i - 1][T2_j - 1].g + 2 * arrayOfData[T2_i][T2_j - 1].g + arrayOfData[T2_i + 1][T2_j - 1].g) - (arrayOfData[T2_i - 1][T2_j + 1].g + 2 * arrayOfData[T2_i][T2_j + 1].g + arrayOfData[T2_i + 1][T2_j + 1].g), 2) + pow((arrayOfData[T2_i - 1][T2_j - 1].g + 2 * arrayOfData[T2_i - 1][T2_j].g + arrayOfData[T2_i - 1][T2_j + 1].g) - (arrayOfData[T2_i + 1][T2_j - 1].g + 2 * arrayOfData[T2_i + 1][T2_j].g + arrayOfData[T2_i + 1][T2_j + 1].g), 2));
            newarrayOfData[T2_i][T2_j].b = sqrt(pow((arrayOfData[T2_i - 1][T2_j - 1].b + 2 * arrayOfData[T2_i][T2_j - 1].b + arrayOfData[T2_i + 1][T2_j - 1].b) - (arrayOfData[T2_i - 1][T2_j + 1].b + 2 * arrayOfData[T2_i][T2_j + 1].b + arrayOfData[T2_i + 1][T2_j + 1].b), 2) + pow((arrayOfData[T2_i - 1][T2_j - 1].b + 2 * arrayOfData[T2_i - 1][T2_j].b + arrayOfData[T2_i - 1][T2_j + 1].b) - (arrayOfData[T2_i + 1][T2_j - 1].b + 2 * arrayOfData[T2_i + 1][T2_j].b + arrayOfData[T2_i + 1][T2_j + 1].b), 2));

            T2_j++;
            maxt2j = max(maxt2j, T2_j);
        }
        T2_i++;
        maxt2i = max(maxt2i, T2_i);
    }
    T2_i = maxt2i;
    
    while (T2_i <= storeHeader1[0].height - 2){
        T2_j = 1;
        while (T2_j <= storeHeader1[0].width - 2){
            
            newarrayOfData[T2_i][T2_j].r = sqrt(pow((arrayOfData[T2_i - 1][T2_j - 1].r + 2 * arrayOfData[T2_i][T2_j - 1].r + arrayOfData[T2_i + 1][T2_j - 1].r) - (arrayOfData[T2_i - 1][T2_j + 1].r + 2 * arrayOfData[T2_i][T2_j + 1].r + arrayOfData[T2_i + 1][T2_j + 1].r), 2) + pow((arrayOfData[T2_i - 1][T2_j - 1].r + 2 * arrayOfData[T2_i - 1][T2_j].r + arrayOfData[T2_i - 1][T2_j + 1].r) - (arrayOfData[T2_i + 1][T2_j - 1].r + 2 * arrayOfData[T2_i + 1][T2_j].r + arrayOfData[T2_i + 1][T2_j + 1].r), 2));
            newarrayOfData[T2_i][T2_j].g = sqrt(pow((arrayOfData[T2_i - 1][T2_j - 1].g + 2 * arrayOfData[T2_i][T2_j - 1].g + arrayOfData[T2_i + 1][T2_j - 1].g) - (arrayOfData[T2_i - 1][T2_j + 1].g + 2 * arrayOfData[T2_i][T2_j + 1].g + arrayOfData[T2_i + 1][T2_j + 1].g), 2) + pow((arrayOfData[T2_i - 1][T2_j - 1].g + 2 * arrayOfData[T2_i - 1][T2_j].g + arrayOfData[T2_i - 1][T2_j + 1].g) - (arrayOfData[T2_i + 1][T2_j - 1].g + 2 * arrayOfData[T2_i + 1][T2_j].g + arrayOfData[T2_i + 1][T2_j + 1].g), 2));
            newarrayOfData[T2_i][T2_j].b = sqrt(pow((arrayOfData[T2_i - 1][T2_j - 1].b + 2 * arrayOfData[T2_i][T2_j - 1].b + arrayOfData[T2_i + 1][T2_j - 1].b) - (arrayOfData[T2_i - 1][T2_j + 1].b + 2 * arrayOfData[T2_i][T2_j + 1].b + arrayOfData[T2_i + 1][T2_j + 1].b), 2) + pow((arrayOfData[T2_i - 1][T2_j - 1].b + 2 * arrayOfData[T2_i - 1][T2_j].b + arrayOfData[T2_i - 1][T2_j + 1].b) - (arrayOfData[T2_i + 1][T2_j - 1].b + 2 * arrayOfData[T2_i + 1][T2_j].b + arrayOfData[T2_i + 1][T2_j + 1].b), 2));

            T2_j++;
        }
        T2_i++;
    }
    
    //upper right quadrant
    T2_i = 1;
    while (T2_i <= storeHeader1[0].height - 2){
        T2_j = maxt2j;
        while (T2_j <= storeHeader1[0].width - 2){
            newarrayOfData[T2_i][T2_j].r = sqrt(pow((arrayOfData[T2_i - 1][T2_j - 1].r + 2 * arrayOfData[T2_i][T2_j - 1].r + arrayOfData[T2_i + 1][T2_j - 1].r) - (arrayOfData[T2_i - 1][T2_j + 1].r + 2 * arrayOfData[T2_i][T2_j + 1].r + arrayOfData[T2_i + 1][T2_j + 1].r), 2) + pow((arrayOfData[T2_i - 1][T2_j - 1].r + 2 * arrayOfData[T2_i - 1][T2_j].r + arrayOfData[T2_i - 1][T2_j + 1].r) - (arrayOfData[T2_i + 1][T2_j - 1].r + 2 * arrayOfData[T2_i + 1][T2_j].r + arrayOfData[T2_i + 1][T2_j + 1].r), 2));
            newarrayOfData[T2_i][T2_j].g = sqrt(pow((arrayOfData[T2_i - 1][T2_j - 1].g + 2 * arrayOfData[T2_i][T2_j - 1].g + arrayOfData[T2_i + 1][T2_j - 1].g) - (arrayOfData[T2_i - 1][T2_j + 1].g + 2 * arrayOfData[T2_i][T2_j + 1].g + arrayOfData[T2_i + 1][T2_j + 1].g), 2) + pow((arrayOfData[T2_i - 1][T2_j - 1].g + 2 * arrayOfData[T2_i - 1][T2_j].g + arrayOfData[T2_i - 1][T2_j + 1].g) - (arrayOfData[T2_i + 1][T2_j - 1].g + 2 * arrayOfData[T2_i + 1][T2_j].g + arrayOfData[T2_i + 1][T2_j + 1].g), 2));
            newarrayOfData[T2_i][T2_j].b = sqrt(pow((arrayOfData[T2_i - 1][T2_j - 1].b + 2 * arrayOfData[T2_i][T2_j - 1].b + arrayOfData[T2_i + 1][T2_j - 1].b) - (arrayOfData[T2_i - 1][T2_j + 1].b + 2 * arrayOfData[T2_i][T2_j + 1].b + arrayOfData[T2_i + 1][T2_j + 1].b), 2) + pow((arrayOfData[T2_i - 1][T2_j - 1].b + 2 * arrayOfData[T2_i - 1][T2_j].b + arrayOfData[T2_i - 1][T2_j + 1].b) - (arrayOfData[T2_i + 1][T2_j - 1].b + 2 * arrayOfData[T2_i + 1][T2_j].b + arrayOfData[T2_i + 1][T2_j + 1].b), 2));
            T2_j++;
        }
        T2_i++;
    }

    for (int i = storeHeader1[0].height - 1; i > 0; i--){
        for (int j = 1; j < storeHeader1[0].width; j++){
            arrayOfData[i][j].r = newarrayOfData[i][j].r;
            arrayOfData[i][j].g = newarrayOfData[i][j].g;
            arrayOfData[i][j].b = newarrayOfData[i][j].b;
        }
    }

    //writing the modified ppm image file
    FILE *output_image = fopen(DI, "w");
    fprintf(output_image, "%s\n%d %d\n%d\n", storeHeader1[0].PPcheck, storeHeader1[0].width, storeHeader1[0].height, storeHeader1[0].colorMAX);
    
    for (int i = storeHeader1[0].height - 1; i >= 0; i--){
        for (int j = 0; j < storeHeader1[0].width; j++){
            fprintf(output_image, "%d\n", arrayOfData[i][j].r);
            fprintf(output_image, "%d\n", arrayOfData[i][j].g);
            fprintf(output_image, "%d\n", arrayOfData[i][j].b);
        }
    }
    fclose(output_image);
    exit(0);
}

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

int main(int argc, char *argv[]){
    //making a pipe
    
    int pipefds[2]; // 0 -- read , 1 -- write ; pipefds[2] is the file descriptor to the pipe in memory
    int pipeDesc1;

    pipeDesc1 = pipe(pipefds); // pipe creation
    
    if (pipeDesc1 == -1)
        perror("pipe");
    
    int pipefds2[2]; // 0 -- read , 1 -- write ; pipefds[2] is the file descriptor to the pipe in memory
    int pipeDesc2;
    
    pipeDesc2 = pipe(pipefds2); // pipe creation
    
    if (pipeDesc2 == -1)
        perror("pipe");

    auto start = startTime(); //clock start

    RBG_2_Gray_Conversion(argv[1], fork(), pipefds, pipefds2);
    Edge_Detection(argv[2], fork(), pipefds, pipefds2);
    
    wait(NULL); wait(NULL);

    auto stop = stopTime();
    auto duration = duration_cast<microseconds>(stop - start);
    
    cout << "Time Elapsed: " << duration.count() << " microseconds" << endl;

    return 0;
}
