#include <array/ArrayOptions.h>
#include <helpers/shape.h>

namespace nd4j {
    bool ArrayOptions::isNewFormat(int *shapeInfo) {
        return (shape::offset(shapeInfo) != 0);
    }

    bool ArrayOptions::isSparseArray(int *shapeInfo) {
        return hasPropertyBitSet(shapeInfo, ARRAY_SPARSE);
    }

    bool ArrayOptions::hasExtraProperties(int *shapeInfo) {
        return hasPropertyBitSet(shapeInfo, ARRAY_EXTRAS);
    }

    bool ArrayOptions::hasPropertyBitSet(int *shapeInfo, int property) {
        if (!isNewFormat(shapeInfo))
            return false;

        return ((shape::extra(shapeInfo) & property) == property);
    }

    bool ArrayOptions::isUnsigned(int *shapeInfo) {
        if (!isNewFormat(shapeInfo))
            return false;

        return hasPropertyBitSet(shapeInfo, ARRAY_UNSIGNED);
    }

    nd4j::DataType ArrayOptions::dataType(int *shapeInfo) {
        if (hasPropertyBitSet(shapeInfo, ARRAY_FLOAT))
            return nd4j::DataType::DataType_FLOAT;
        else if (hasPropertyBitSet(shapeInfo, ARRAY_DOUBLE))
            return nd4j::DataType::DataType_DOUBLE;
        else if (hasPropertyBitSet(shapeInfo, ARRAY_HALF))
            return nd4j::DataType::DataType_HALF;
        else
            return nd4j::DataType::DataType_INHERIT;
    }

    SpaceType ArrayOptions::spaceType(int *shapeInfo) {
        if (hasPropertyBitSet(shapeInfo, ARRAY_QUANTIZED))
            return SpaceType::QUANTIZED;
        else // by default we return continuous type here
            return SpaceType::CONTINUOUS;
    }

    ArrayType ArrayOptions::arrayType(int *shapeInfo) {
        if (hasPropertyBitSet(shapeInfo, ARRAY_SPARSE))
            return ArrayType::SPARSE;
        else // by default we return DENSE type here
            return ArrayType::DENSE;
    }

    bool ArrayOptions::togglePropertyBit(int *shapeInfo, int property) {
        shape::extra(shapeInfo) ^= property;

        return hasPropertyBitSet(shapeInfo, property);
    }

    bool ArrayOptions::setPropertyBit(int *shapeInfo, int property) {
        shape::extra(shapeInfo) |= property;

        return hasPropertyBitSet(shapeInfo, property);
    }

    void ArrayOptions::setPropertyBits(int *shapeInfo, std::initializer_list<int> properties) {
        for (auto v: properties) {
            if (!hasPropertyBitSet(shapeInfo, v))
                setPropertyBit(shapeInfo, v);
        }
    }
}