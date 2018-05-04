//
// This file contains various helper methods/classes suited for sparse support
//
// @author raver119@gmail.com
//

#ifndef LIBND4J_SPECIALS_SPARSE_H
#define LIBND4J_SPECIALS_SPARSE_H

#include <pointercast.h>

namespace nd4j {
    namespace sparse {

        template <typename T>
        class SparseUtils {
        public:
            /**
        * Just simple helper for debugging :)
        *
        * @param indices
        * @param rank
        * @param x
        */
            static void printIndex(int *indices, int rank, int x);
            static bool ltIndices(int *indices, int rank, Nd4jLong x, Nd4jLong y);

            /**
            * Returns true, if x > y, false otherwise
            * @param indices
            * @param rank
            * @param x
            * @param y
            * @return
            */
            static bool gtIndices(int *indices, int rank, Nd4jLong x, Nd4jLong y);

            static void swapEverything(int *indices, T *array, int rank, Nd4jLong x, Nd4jLong y);

            static void coo_quickSort_parallel_internal(int *indices, T* array, Nd4jLong left, Nd4jLong right, int cutoff, int rank);

            static void coo_quickSort_parallel(int *indices, T* array, Nd4jLong lenArray, int numThreads, int rank);

            static void sortCooIndicesGeneric(int *indices, T *values, Nd4jLong length, int rank);
        };
    }
}


#endif //LIBND4J_SPECIALS_SPARSE_H
