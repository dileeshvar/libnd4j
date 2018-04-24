//
// Created by GS <sgazeos@gmail.com> on 16.04.18.
//

#include <ops/declarable/helpers/matmul.h>
#define MAX_CUDA_DEVICE 512
#define BLOCK_SIZE 32
namespace nd4j {
    namespace ops {
        namespace helpers {
            
            //(CblasColMajor, transA, transB, M, N, K, (float) alpha, (float *) pA->getBuffer(), lda, (float *) pB->getBuffer(), ldb, (float) beta, (float *) pC->getBuffer(), ldc)
            template <typename T>
            void GUDA_G device_matmul(int align, int transA, int transB, int M, int N, int K, T alpha, T* pA, int ldA, T* pB, int ldB, T beta, T* pC, int ldC) {
                // Handle to thread block group
                cooperative_groups::thread_block cta = cooperative_groups::this_thread_block();
                // Block index
                int bx = blockIdx.x;
                int by = blockIdx.y;
    
                // Thread index
                int tx = threadIdx.x;
                int ty = threadIdx.y;
    
                // Index of the first sub-matrix of A processed by the block
                int aBegin = K * BLOCK_SIZE * by;
    
                // Index of the last sub-matrix of A processed by the block
                int aEnd   = aBegin + K - 1;
    
                // Step size used to iterate through the sub-matrices of A
                int aStep  = BLOCK_SIZE;
    
                // Index of the first sub-matrix of B processed by the block
                int bBegin = BLOCK_SIZE * bx;
    
                // Step size used to iterate through the sub-matrices of B
                int bStep  = BLOCK_SIZE * N;
    
                // Csub is used to store the element of the block sub-matrix
                // that is computed by the thread
                float Csub = 0;
    
                // Loop over all the sub-matrices of A and B
                // required to compute the block sub-matrix
                for (int a = aBegin, b = bBegin; a <= aEnd; a += aStep, b += bStep)
                {
                    // Declaration of the shared memory array As used to
                    // store the sub-matrix of A
                    __shared__ T As[BLOCK_SIZE][BLOCK_SIZE];
    
                    // Declaration of the shared memory array Bs used to
                    // store the sub-matrix of B
                    __shared__ T Bs[BLOCK_SIZE][BLOCK_SIZE];
    
                    // Load the matrices from device memory
                    // to shared memory; each thread loads
                    // one element of each matrix
                    As[ty][tx] = pA[a + K * ty + tx];
                    Bs[ty][tx] = pB[b + N * ty + tx];
    
                    // Synchronize to make sure the matrices are loaded
                   cooperative_groups::sync(cta);
    
    
                    // Multiply the two matrices together;
                    // each thread computes one element
                    // of the block sub-matrix
#pragma unroll
                    for (int k = 0; k < BLOCK_SIZE; ++k)
                    {
                        Csub += As[ty][k] * Bs[k][tx];
                    }
    
                    // Synchronize to make sure that the preceding
                    // computation is done before loading two new
                    // sub-matrices of A and B in the next iteration
                    cooperative_groups::sync(cta);
                }

                // Write the block sub-matrix to device memory;
                // each thread writes one element
                int c = N * BLOCK_SIZE * by + BLOCK_SIZE * bx;
                pC[c + N * ty + tx] = Csub;
            }

            template <typename T> 
            _CUDA_H
            void _matmul(NDArray<T> *vA, NDArray<T> *vB, NDArray<T> *vC, int transA, int transB, T alpha, T beta) {

                int M = vA->sizeAt(0);
                int N = vB->sizeAt(1);
                int K = vA->sizeAt(1);
                bool transAFlag = transA != CblasNoTrans;
                bool transBFlag = transB != CblasNoTrans;
                int ldA = transAFlag ? K : M;
                int ldB = transBFlag ? N : K;
                int ldC = M;
            
                device_matmul<T><<<MAX_CUDA_DEVICE, MAX_CUDA_DEVICE>>>(CblasColMajor, transA, transB, M, N, K, alpha, vA->getBuffer(), lda, vB->getBuffer(), ldb, beta, vC->getBuffer(), ldC);
            }

            template void _matmul<float>(NDArray<float> *A, NDArray<float> *B, NDArray<float> *C, int transA, int transB, float alpha, float beta);
            template void _matmul<float16>(NDArray<float16> *A, NDArray<float16> *B, NDArray<float16> *C, int transA, int transB, float16 alpha, float16 beta);
            template void _matmul<double>(NDArray<double> *A, NDArray<double> *B, NDArray<double> *C, int transA, int transB, double alpha, double beta);
        }
    }
}
