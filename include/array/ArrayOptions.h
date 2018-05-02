#ifndef ND4J_ARRAY_OPTIONS_H
#define ND4J_ARRAY_OPTIONS_H

#include <op_boilerplate.h>
#include <pointercast.h>
#include <dll.h>
#include <array/DataType.h>
#include <array/ArrayType.h>
#include <array/SpaceType.h>
#include <initializer_list>

#define ARRAY_DENSE 1
#define ARRAY_SPARSE 2
#define ARRAY_COMPRESSED 4

#define ARRAY_CSR 16
#define ARRAY_COO 32

#define ARRAY_CONTINUOUS 256
#define ARRAY_COMPLEX 512
#define ARRAY_QUANTIZED 1024

#define ARRAY_FLOAT 4096
#define ARRAY_DOUBLE 8192
#define ARRAY_HALF 16384
#define ARRAY_HALF2 32768
#define ARRAY_CHAR 65536
#define ARRAY_SHORT 131072
#define ARRAY_INT 262144
#define ARRAY_LONG 524288

#define ARRAY_EXTRAS 2097152

#define ARRAY_UNSIGNED 8388608


namespace nd4j {
    class ND4J_EXPORT ArrayOptions {

    public:
        static bool isNewFormat(int *shapeInfo);
        static bool hasPropertyBitSet(int *shapeInfo, int property);
        static bool togglePropertyBit(int *shapeInfo, int property);
        static bool setPropertyBit(int *shapeInfo, int property);
        static void setPropertyBits(int *shapeInfo, std::initializer_list<int> properties);

        static bool isSparseArray(int *shapeInfo);
        static bool isUnsigned(int *shapeInfo);

        static nd4j::DataType dataType(int *shapeInfo);
        static SpaceType spaceType(int *shapeInfo);
        static ArrayType arrayType(int *shapeInfo);

        static bool hasExtraProperties(int *shapeInfo);
    };
}

#endif // ND4J_ARRAY_OPTIONS_H :)