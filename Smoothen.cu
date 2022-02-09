#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

#include <stdio.h>
#include <stdlib.h>

using namespace std;
#define n_blocks 1 
#define BLOCK_SIZE 1
//We may change this value!!
const int FILTER_WIDTH = 7;



//We may change this value!!!
int FILTER[FILTER_WIDTH*FILTER_WIDTH] = {
	1,4,7,10,7,4,1,
	4,12,26,33,26,12,4,
	7,26,55,71,55,26,7,
	10,33,71,91,71,33,10,
	7,26,55,71,55,26,7,
	4,12,26,33,26,12,4,
	1,4,7,10,7,4,1
};


// Display the first and last 10 items
// For debug only
void displayResult(const int original[], const int result[], int size) {
	cout << "Display result: ";
	cout << "(original -> result)\n";

	for (int i = 0; i < 10; i++) {
		cout << original[i] << " -> " << result[i] << "\n";
	}
	cout << ".\n.\n.\n";

	for (int i = size - 10; i < size; i++) {
		cout << original[i] << " -> " << result[i] << "\n";
	}
}

void initColorData(string file, int **data, int *sizeX, int *sizeY) {
	int x;
	int y;
	long long i = 0;
	cout << "Reading "<< file << "... \n";
	ifstream myfile(file);
	if (myfile.is_open()) {
		myfile >> x;
		myfile >> y;

		int *temp = new int[x * y * 3];
		for( i=0; i < x * y * 3; i++){
			myfile >> temp[(int)i];
		}
		myfile.close();
		*data = temp;
		*sizeX = x;
		*sizeY = y;
	}
	else {
		cout << "ERROR: File " << file << " not found!\n";
		exit(0);
	}
	cout << i << " entries imported\n";
}

void saveResult(string file, int data[], int sizeX, int sizeY) {
	long long i = 0;
	cout << "Saving data to "<< file <<"... \n";
	ofstream myfile(file, std::ofstream::out);
	if (myfile.is_open()) {
		myfile << sizeX << "\n";
		myfile << sizeY << "\n";
		for (i = 0; i < sizeX * sizeY; i++){
			myfile << data[3* i] << " " << data[3* i + 1] << " " << data[3* i+ 2]<< "\n";
		}
		myfile.close();
	}
	else {
		cout << "ERROR: Cannot save to " << file << "!\n";
		exit(0);
	}
	cout << i << " entries saved\n";
}

// TODO: implement the kneral function for 2D smoothing 

__global__ void smoothen(int *d_data, int *d_result, int *filter, int d_sizeX, int d_sizeY, int fwidth, int sum) {

      int x = blockIdx.x * blockDim.x + threadIdx.x;
      int y = blockIdx.y * blockDim.y + threadIdx.y;
      int xf,yf,hf=fwidth/2;

      __syncthreads(); // Synchronize (ensure all the data is available)

      if(y < d_sizeY && x <d_sizeX ) {
           int b=0;
           for (yf=fwidth-1;yf>=0;yf--) {
               for (xf=fwidth-1;xf>=0;xf--) {
			//What is the logic here?
                        int xt= max(0,min(x+(-xf+hf)*3,d_sizeX-1));
                        int yt= max(0,min(y-yf+hf,d_sizeY-1));
                        int w = filter[yf*fwidth+xf];
                        b += w*d_data[yt*d_sizeX+xt];
               }
           }
	   b /= sum;
           d_result[y*d_sizeX+x]=b;
        }



}


// GPU implementation
void GPU_Test(int data[], int result[], int sizeX, int sizeY) {
	// input:
	//	int data[] - int array holding the flattened original image
	//	int sizeX - the width of the image
	//	int sizeY - the height of the image
	// output:
	//	int result[] - int array holding the image

	// TODO: allocate device memory and copy data onto the device
	int *d_data, *d_result, *d_filter, c, sum=0;
	int size=3*sizeX*sizeY*sizeof(int);
        cudaMalloc((void **)&d_data, size);
        cudaMalloc((void **)&d_result, size);
	cudaMalloc((void **)&d_filter, FILTER_WIDTH*FILTER_WIDTH*sizeof(int));

        cudaMemcpy(d_data, data, size, cudaMemcpyHostToDevice);
        cudaMemcpy(d_result, result, size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_filter, FILTER, FILTER_WIDTH*FILTER_WIDTH*sizeof(int), cudaMemcpyHostToDevice);

	// Start timer for kernel
	auto startKernel = chrono::steady_clock::now();

	// TODO: call the kernel function
	dim3 dimBlock(16,16);
	dim3 dimGrid((sizeX*3+15)/16,(sizeY+15)/16);

	for (c=0;c<FILTER_WIDTH*FILTER_WIDTH;c++) {
		sum+=FILTER[c];
	}	

	smoothen<<<dimGrid, dimBlock>>>(d_data, d_result, d_filter, sizeX*3, sizeY, FILTER_WIDTH, sum);
	// End timer for kernel and display kernel time
	cudaDeviceSynchronize(); // <- DO NOT REMOVE
	auto endKernel = chrono::steady_clock::now();
	cout << "Kernel Elapsed time: " << chrono::duration <double, milli>(endKernel - startKernel).count() << "ms\n";

	// TODO: copy reuslt from device to host
	cudaMemcpy(result, d_result, size, cudaMemcpyDeviceToHost);
	// TODO: free device memory
        cudaFree(d_data);
        cudaFree(d_result);
	cudaFree(d_filter);
}


// CPU implementation
void CPU_Test(int data[], int result[], int sizeX, int sizeY) {
	// input:
	//	int data[] - int array holding the flattened original image
	//	int sizeX - the width of the image
	//	int sizeY - the height of the image
	// output:
	//	int result[] - int array holding the image

        int c,x,y,xf,yf,sum=0;
	int hf=FILTER_WIDTH/2;
	memset(result,0,sizeof(int)*sizeX*sizeY*3);
        for(c=0;c<FILTER_WIDTH*FILTER_WIDTH;c++) {
		sum+=FILTER[c];
	}
        for (y=0;y<sizeY;y++) {
		for (x=0;x<sizeX;x++) {
			for (yf=FILTER_WIDTH-1;yf>=0;yf--) {
        			for (xf=FILTER_WIDTH-1;xf>=0;xf--) {
        				if ((y-yf+hf)<sizeY && (x-xf+hf)<sizeX && (y-yf+hf)>=0 && (x-xf+hf)>=0) {
                                                result[y*sizeX*3+x*3]+=data[(y-yf+hf)*sizeX*3 + (x-xf+hf)*3]*
                                                        FILTER[yf*FILTER_WIDTH+xf];
                                                result[y*sizeX*3+x*3+1]+=data[(y-yf+hf)*sizeX*3 + (x-xf+hf)*3+1]*
                                                        FILTER[yf*FILTER_WIDTH+xf];
                                                result[y*sizeX*3+x*3+2]+=data[(y-yf+hf)*sizeX*3 + (x-xf+hf)*3+2]*
                                                        FILTER[yf*FILTER_WIDTH+xf];
                                        }
                                }
                        }
			result[y*sizeX*3+x*3]/=sum;
			result[y*sizeX*3+x*3+1]/=sum;
                        result[y*sizeX*3+x*3+2]/=sum;
                }
        }


	// TODO: smooth the image with filter size = FILTER_WIDTH
	//       apply zero padding for the border
}

// The image is flattened into a text file of pixel values.
int main(int argc, char *argv[]) {
	string inputFile = (argc == 1) ? "image_color.txt" : argv[1];

	int sizeX;
	int sizeY;
	int *dataForCPUTest;
	int *dataForGPUTest;	

	initColorData(inputFile, &dataForCPUTest, &sizeX, &sizeY);
	initColorData(inputFile, &dataForGPUTest, &sizeX, &sizeY);

	int size = sizeX * sizeY * 3;
	int *resultForCPUTest = new int[size];
	int *resultForGPUTest = new int[size];

	cout << "\n";

	cout << "CPU Implementation\n";

	auto startCPU = chrono::steady_clock::now();
	CPU_Test(dataForCPUTest, resultForCPUTest, sizeX, sizeY);
	auto endCPU = chrono::steady_clock::now();

	cout << "Elapsed time: " << chrono::duration <double, milli>(endCPU - startCPU).count() << "ms\n";

	// displayResult(dataForCPUTest, resultForCPUTest, size);

	saveResult("color_result_CPU.txt",resultForCPUTest, sizeX, sizeY);

	cout << "\n";
	cout << "GPU Implementation\n";

	auto startGPU = chrono::steady_clock::now();
	GPU_Test(dataForGPUTest, resultForGPUTest, sizeX, sizeY);
	auto endGPU = chrono::steady_clock::now();

	cout << "Elapsed time: " << chrono::duration <double, milli>(endGPU - startGPU).count() << "ms\n";

	// displayResult(dataForGPUTest, resultForGPUTest, size);
	saveResult("color_result_GPU.txt",resultForGPUTest, sizeX, sizeY);

        delete resultForCPUTest;
        delete resultForGPUTest;
        delete dataForCPUTest;
        delete dataForGPUTest;
	
	return 0;
}
