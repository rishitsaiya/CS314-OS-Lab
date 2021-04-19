#include <bits/stdc++.h>
#include <chrono>
#include <cmath>
#include <thread>
#include <vector>

using namespace std;
using namespace std::chrono;

// Global variables
atomic_flag atomicFlag2False = ATOMIC_FLAG_INIT; // Initializes the atomicFlag to FALSE
int goBack[2] = {0,0}; // Bits intialized to check if the selected bit is correct

struct pixel{
    int r, g, b; //Red, Green, Blue Color Defined
};


//Function for RGB_to_Grayscale
void RBG_2_Gray_Conversion(vector<vector<pixel>> &arrayOfData, int height, int width){ 
    
    // RGB_to_Grayscale
    
    for (int i = 0; i < height; i++){      // Traverse through arrayofData (given image in height)
        for (int j = 0; j < width; j++){   // Traverse through arrayofData (given image in width)
            
            while (atomic_flag_test_and_set(&atomicFlag2False)); // Spin Lock Insert

            //Store r, g, b values in each iteration temp in rData, gData, bData
            int rData = arrayOfData[i][j].r; 
            int gData = arrayOfData[i][j].g;
            int bData = arrayOfData[i][j].b;

    // By considering weights for grayscale, appropriate weighted means are taken
            arrayOfData[i][j].r = ((rData * 0.2989) + (gData * 0.5870) + (bData * 0.1140));
            arrayOfData[i][j].g = ((rData * 0.2989) + (gData * 0.5870) + (bData * 0.1140));
            arrayOfData[i][j].b = ((rData * 0.2989) + (gData * 0.5870) + (bData * 0.1140));

            goBack[0] = i + 1; goBack[1] = j + 1; // Assignment of goBack to signal signal lock

            atomic_flag_clear(&atomicFlag2False); // Signal Flag Done
        }
    }
}

void Edge_Detection(vector<vector<pixel>> &arrayOfData, int height, int width){ 
    
    // Edge Detection
    // Ref: https://stackoverflow.com/questions/16385570/sobel-edge-detector-using-c-without-any-special-library-or-tool
    vector<vector<pixel>> newArrayofData (height, vector<pixel>(width));
    int pixelWorked = 0;

        for(int i = 1; i <= height - 2; i++){
            for(int j = 1; j <= width - 2; j++){

                while (atomic_flag_test_and_set(&atomicFlag2False)); // Spin Lock Insert
                if(j + 1 <= goBack[1] || i + 1 <= goBack[0]){
            
                    newArrayofData[i][j].r = sqrt(pow((arrayOfData[i - 1][j - 1].r + 2 * arrayOfData[i][j - 1].r + arrayOfData[i + 1][j - 1].r) - (arrayOfData[i - 1][j + 1].r + 2 * arrayOfData[i][j + 1].r + arrayOfData[i + 1][j + 1].r), 2) + pow((arrayOfData[i - 1][j - 1].r + 2 * arrayOfData[i - 1][j].r + arrayOfData[i - 1][j + 1].r) - (arrayOfData[i + 1][j - 1].r + 2 * arrayOfData[i + 1][j].r + arrayOfData[i + 1][j + 1].r), 2));
                    newArrayofData[i][j].g = sqrt(pow((arrayOfData[i - 1][j - 1].g + 2 * arrayOfData[i][j - 1].g + arrayOfData[i + 1][j - 1].g) - (arrayOfData[i - 1][j + 1].g + 2 * arrayOfData[i][j + 1].g + arrayOfData[i + 1][j + 1].g), 2) + pow((arrayOfData[i - 1][j - 1].g + 2 * arrayOfData[i - 1][j].g + arrayOfData[i - 1][j + 1].g) - (arrayOfData[i + 1][j - 1].g + 2 * arrayOfData[i + 1][j].g + arrayOfData[i + 1][j + 1].g), 2));
                    newArrayofData[i][j].b = sqrt(pow((arrayOfData[i - 1][j - 1].b + 2 * arrayOfData[i][j - 1].b + arrayOfData[i + 1][j - 1].b) - (arrayOfData[i - 1][j + 1].b + 2 * arrayOfData[i][j + 1].b + arrayOfData[i + 1][j + 1].b), 2) + pow((arrayOfData[i - 1][j - 1].b + 2 * arrayOfData[i - 1][j].b + arrayOfData[i - 1][j + 1].b) - (arrayOfData[i + 1][j - 1].b + 2 * arrayOfData[i + 1][j].b + arrayOfData[i + 1][j + 1].b), 2));

                    pixelWorked = 1;
                }
                else
                    pixelWorked = 0;
                atomic_flag_clear(&atomicFlag2False);
                if(pixelWorked == 0)
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
            arrayOfData[i][j].r = newArrayofData[i][j].r;
            arrayOfData[i][j].g = newArrayofData[i][j].g;
            arrayOfData[i][j].b = newArrayofData[i][j].b;
        }
    }
}

// Function to store RGB values - Buff seg fault

void storeRGB(FILE *inputImage, vector<vector<pixel>> &arrayOfData, int height, int width){
    int r, g, b;

    for(int i = height - 1; i >= 0; i--){
        for(int j = 0; j <= width - 1; j++){
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
        for(int j = 0; j <= width - 1; j++){
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

    vector<vector<pixel>> arrayOfData(height, vector<pixel>(width));

    // storeRGB(inputImage, arrayOfData, height, width);

    // Loop to store the read values in vectors
    for (int i = height - 1; i >= 0; i--){
        for (int j = 0; j <= width - 1; j++){
            fscanf(inputImage, "%d%d%d", &r, &g, &b);
            arrayOfData[i][j].r = r;
            arrayOfData[i][j].g = g;
            arrayOfData[i][j].b = b;
        }
    }
    fclose(inputImage);
    
    // Time connection start

    auto start = startTime();

    // Threads made for each image transformation

    thread T1 (RBG_2_Gray_Conversion, std::ref(arrayOfData), height, width);
    thread T2 (Edge_Detection, std::ref(arrayOfData), height, width);

    // Waiting for T1 & T2 call by using join()

    T1.join();
    T2.join();

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
        for (int j = 0; j <= width - 1; j++){
            fprintf(outputImage, "%d ", arrayOfData[i][j].r);
            fprintf(outputImage, "%d ", arrayOfData[i][j].g);
            fprintf(outputImage, "%d ", arrayOfData[i][j].b);
        }
        fprintf(outputImage, "\n");
    }
    fclose(outputImage);

    return 0;
}