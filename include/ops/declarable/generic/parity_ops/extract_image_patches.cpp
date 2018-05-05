//
// Created by GS <sgazeos@gmail.com> at 3/30/2018
//

#include <ops/declarable/CustomOperations.h>
#include <ops/declarable/helpers/extract_patches.h>

namespace nd4j {
    namespace ops {
        CUSTOM_OP_IMPL(extract_image_patches, 1, 1, false, 0, 7) {
            NDArray<T>* input = INPUT_VARIABLE(0);
            NDArray<T>* output = OUTPUT_VARIABLE(0);
            int ksizeRows = INT_ARG(0);
            int ksizeCols = INT_ARG(1);
            int kstrideRows = INT_ARG(2);
            int kstrideCols = INT_ARG(3);
            int krateRows = INT_ARG(4);
            int krateCols = INT_ARG(5);
            bool isSame = INT_ARG(6) != 0;

            REQUIRE_TRUE(input->rankOf() == 4, 0, "extract_image_patches: The rank of input array should be 4, but %i is given", input->rankOf());
            //
            if (output->isSameShape(input))
                output->assign(input);
            else {
                helpers::extractPatches(input, output, ksizeRows, ksizeCols, kstrideRows, kstrideCols, krateRows, krateCols, isSame);
            }
            return ND4J_STATUS_OK;
        }

        DECLARE_SHAPE_FN(extract_image_patches) {

            auto in = inputShape->at(0);
            int outRank = shape::rank(in);
            Nd4jLong *outputShape = nullptr;

            int ksizeRowsEffective = INT_ARG(0) + (INT_ARG(0) - 1) * (INT_ARG(4) - 1);
            int ksizeColsEffective = INT_ARG(1) + (INT_ARG(1) - 1) * (INT_ARG(5) - 1);

            auto batchSizeDim = shape::sizeAt(in, 0);
            auto inputRowsDim = shape::sizeAt(in, 1);
            auto inputColsDim = shape::sizeAt(in, 2);
            auto outputDepthDim = shape::sizeAt(in, 3) * INT_ARG(0) * INT_ARG(1); // last dim * ksizeRows * ksizeCols

            auto inputRowSize = inputRowsDim; //shape::sizeAt(in, inputRowsDim);
            auto inputColSize = inputColsDim; //shape::sizeAt(in, inputColsDim);
            Nd4jLong outRowSize;
            Nd4jLong outColSize;
            if (INT_ARG(6) == 0) {
                // Padding is "VALID":
                outRowSize = (inputRowSize - ksizeRowsEffective + INT_ARG(2)) / INT_ARG(2);
                outColSize = (inputColSize - ksizeColsEffective + INT_ARG(3)) / INT_ARG(3);
            } else {
                // Padding is "SAME":
                outRowSize = (inputRowSize + INT_ARG(2) - 1) / INT_ARG(2);
                outColSize = (inputColSize + INT_ARG(3) - 1) / INT_ARG(3);
            }


            ALLOCATE(outputShape, block.getWorkspace(), shape::shapeInfoLength(outRank), Nd4jLong);

            outputShape[0] = outRank;
            outputShape[1] = batchSizeDim;
            outputShape[2] = outRowSize;
            outputShape[3] = outColSize;
            outputShape[4] = outputDepthDim;


            shape::updateStrides(outputShape, shape::order(in));

            return SHAPELIST(outputShape);
        }
/*
Status GetWindowedOutputSizeVerboseV2(int64 input_size, int64 filter_size,
                                      int64 dilation_rate = 1, int64 stride,
                                      Padding padding_type, int64* output_size,
                                      int64* padding_before,
                                      int64* padding_after) {
  if (stride <= 0) {
    return errors::InvalidArgument("Stride must be > 0, but got ", stride);
  }


  // See also the parallel implementation in GetWindowedOutputSizeFromDimsV2.
  int64 effective_filter_size = (filter_size - 1) * dilation_rate + 1;

  return Status::OK();
}
*/
    }
}