#include <stdio.h>
#include <cuda.h>
#include <cfloat>
#include <iostream>
using namespace std;
// The number of threads per blocks in the kernel
const int threads_per_block = 256;

float *out_CPU=0;
float *in_GPU=0, *out_GPU=0;

// Forward function declarations
float GPU_vector_max(float *A, int N);
float *get_random_vector(int N);
float *get_increasing_vector(int N);

void die(const char *message);
void checkError();

// Main program
int main(int argc, char **argv) {

    // Parse vector length and kernel options
    int N=threads_per_block;
    int vecN,NvecN;
    int vp=0;
    float *tmpv;

    // Seed the random generator (for repeatable results)
    int seed;
    if(argc == 3) {
        vecN = atoi(argv[1]); // user-specified value
        seed = atoi(argv[2]);
    } else {
        die("USAGE: ./vecmax <vector_length> <rand_seed>");
    }

    NvecN= vecN/N; //Modified
    tmpv= new float[N];
    memset(tmpv,0,sizeof(float)*N);
    srand(seed);

    //float *vec = get_random_vector(N);

    // for debug, use the arrary of 0.0, 1.0, 2.0, ... as input
    // float *vec = get_increasing_vector(N);
    for (int i=0;i< NvecN;i++) {
        float *vec =get_random_vector(N); // get_increasing_vector(N);
        float result_GPU = GPU_vector_max(vec, N);
        tmpv[vp++] = result_GPU;
        if (vp > N-1) {
                float result_GPU = GPU_vector_max(tmpv, N);
                vp=0;
                memset(tmpv,0,sizeof(float)*N);
                tmpv[vp]=result_GPU;
                vp++;
        }
        cudaFree(vec);
    }

    if (vecN%N) {
        float *vec =get_random_vector(vecN%N); // get_increasing_vector(N);
        float result_GPU = GPU_vector_max(vec, vecN%N);
        tmpv[vp++] = result_GPU;
        if (vp > N-1) {
                float result_GPU = GPU_vector_max(tmpv, N);
                vp=0;
                memset(tmpv,0,sizeof(float)*N);
                tmpv[vp]=result_GPU;
                vp++;
        }
    }

    if (vp > 1) {
        float result_GPU = GPU_vector_max(tmpv, N);
        tmpv[0] = result_GPU;
    }
    std:: cout << "max = "<<tmpv[0] << std::endl;
    delete tmpv;
    cudaFree(in_GPU);
    cudaFree(out_GPU);
    cudaFree(out_CPU);
}


// A simple GPU kernel that computes the maximum value of a vector
// (each lead thread (threadIdx.x == 0) computes a single value
__global__ void vector_max_kernel(float *in, float *out, int N) {

    __shared__ float f[threads_per_block];
    // Determine the "flattened" block id and thread id
    int block_id = blockIdx.x + gridDim.x * blockIdx.y;
    int thread_id = blockDim.x * block_id + threadIdx.x;

    f[thread_id]=in[thread_id];
    __syncthreads();
    // A single "lead" thread in each block finds the maximum value over a range of size threads_per_block
    float max = 0.0;
    if (threadIdx.x == 0) {
        //calculate out of bounds guard
        int end = threads_per_block;
        if(thread_id + threads_per_block > N)
            end = N - thread_id;
        max = f[thread_id];

        for(int i = 1; i < end; i++) {
            if(max < f[thread_id + i])
                max = f[thread_id + i];
        }
        out[block_id] = max;
    }
}

// Returns the maximum value within a vector of length N
float GPU_vector_max(float *in_CPU, int N){

    int vector_size = N * sizeof(float);

    if(! out_CPU)
                cudaMallocHost((void **) &out_CPU, vector_size );
    if (out_CPU == NULL)
                die("Error allocating CPU memory");

    if(!in_GPU) {
        if (cudaMalloc((void **) &in_GPU, vector_size) != cudaSuccess)
                die("Error allocating GPU memory");
    }

    if(!out_GPU) {
        if (cudaMalloc((void **) &out_GPU, vector_size) != cudaSuccess)
                die("Error allocating GPU memory");
    }

    cudaMemcpy(in_GPU, in_CPU, vector_size, cudaMemcpyHostToDevice);

    // Determine the number of thread blocks in the x- and y-dimension
    int num_blocks = (int) ((float) (N + threads_per_block - 1) / (float) threads_per_block);
    int max_blocks_per_dimension = 65535;
    int num_blocks_y = (int) ((float) (num_blocks + max_blocks_per_dimension - 1) / (float) max_blocks_per_dimension);
    int num_blocks_x = (int) ((float) (num_blocks + num_blocks_y - 1) / (float) num_blocks_y);
    dim3 grid_size(num_blocks_x, num_blocks_y, 1);

    vector_max_kernel <<< grid_size , threads_per_block >>> (in_GPU, out_GPU, N);


    cudaMemcpy(out_CPU, out_GPU, vector_size, cudaMemcpyDeviceToHost);

    float max = out_CPU[0];

    return max;
}
// Returns a randomized vector containing N elements
float *get_random_vector(int N) {
    if (N < 1) die("Number of elements must be greater than zero");

    float *V;
    cudaMallocHost((void **) &V, N * sizeof(float));
    if (V == NULL) die("Error allocating CPU memory");

    for (int i = 0; i < N; i++) {
      V[i] = (float) rand() / (1+(float) rand());
      cout<<"V["<<i<<"] = "<<V[i]<<endl;
    }

    return V;
}

void checkError() {
    // Check for kernel errors
    cudaError_t error = cudaGetLastError();
    if (error) {
        char message[256];
        sprintf(message, "CUDA error: %s", cudaGetErrorString(error));
        die(message);
    }
}

float *get_increasing_vector(int N) {
    if (N < 1) die("Number of elements must be greater than zero");

    float *V;
    cudaMallocHost((void **) &V, N * sizeof(float));
    if (V == NULL) die("Error allocating CPU memory");

    for (int i = 0; i < N; i++) V[i] = (float) i;

    return V;
}

// Prints the specified message and quits
void die(const char *message) {
    printf("%s\n", message);
    exit(1);
}


