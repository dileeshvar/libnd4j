//
// Created by raver119 on 13.01.2018.
//

#include "testlayers.h"
#include <array/ArrayOptions.h>
#include <NDArray.h>

using namespace nd4j;


class ArrayOptionsTests : public testing::Test {
public:
    int shape[8] = {2, 5, 5, 5, 1, 0, 1, 99};
};

TEST_F(ArrayOptionsTests, TestShape_Basic_0) {
    shape[5] = 1;


    ASSERT_TRUE(ArrayOptions::isNewFormat(shape));
    ASSERT_FALSE(ArrayOptions::isSparseArray(shape));
}


TEST_F(ArrayOptionsTests, TestShape_Basic_1) {
    shape[5] = 4;
    

    ASSERT_TRUE(ArrayOptions::isNewFormat(shape));
    ASSERT_TRUE(ArrayOptions::isSparseArray(shape));
}


TEST_F(ArrayOptionsTests, TestShape_Basic_2) {
    shape[5] = 260;
    
    ASSERT_TRUE(ArrayOptions::isNewFormat(shape));

    ASSERT_TRUE(ArrayOptions::isSparseArray(shape));
    ASSERT_EQ(SpaceType::CONTINUOUS, ArrayOptions::spaceType(shape));
}

TEST_F(ArrayOptionsTests, TestShape_Basic_3) {
    ASSERT_EQ(0, shape::extra(shape));
    
    ArrayOptions::setPropertyBit(shape, ARRAY_CONTINUOUS);

    ASSERT_EQ(ARRAY_CONTINUOUS, shape::extra(shape));
}

TEST_F(ArrayOptionsTests, TestShape_Basic_4) {

    ArrayOptions::setPropertyBits(shape, {ARRAY_DENSE, ARRAY_HALF, ARRAY_QUANTIZED});

    ASSERT_FALSE(ArrayOptions::isSparseArray(shape));
    ASSERT_TRUE(nd4j::DataType::DataType_HALF == ArrayOptions::dataType(shape));
    ASSERT_EQ(nd4j::ArrayType::DENSE, ArrayOptions::arrayType(shape));
    ASSERT_EQ(nd4j::SpaceType::QUANTIZED, ArrayOptions::spaceType(shape));
}

