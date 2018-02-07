//
// Created by raver119 on 29/10/17.
//
// Modified by GS <sgazeos@gmail.com> 2/7/18
//

#include <ops/declarable/CustomOperations.h>

namespace nd4j {
    namespace ops {
        CONFIGURABLE_OP_IMPL(lrn, 1, 1, true, 3, 1) {

            NDArray<T>* input  = INPUT_VARIABLE(0);
            NDArray<T>* output = OUTPUT_VARIABLE(0);

            REQUIRE_TRUE(input->rankOf() == 4, 0, "Input rank of 4 expected, but got %i instead", input->rankOf());

            T alpha = T_ARG(1);
            T beta = T_ARG(2);
            T bias = T_ARG(0);
            int depth = INT_ARG(0);

//         * TF define this as follow:
//         * sqr_sum[a, b, c, d] =
//         *    sum(input[a, b, c, d - depth_radius : d + depth_radius + 1] ** 2)
//         *    output = input / (bias + alpha * sqr_sum) ** beta
            T dividor;

//            std::unique_ptr<NDArray<T>> patch(NDArrayFactory<T>::createUninitialized(input));
            int totalLength = input->lengthOf();
            int lastDim = input->sizeAt(-1);
            int chunkCount = totalLength / lastDim;
            //std::unique_ptr<NDArray<T>> patch(input->subArray({first, last - 1}); //dup('c'));
            for (int c = 0; c < chunkCount; c++) {
                for (int e = 0; e < lastDim; e++) {
                    int begin = std::max(0, e - depth);
                    int end = std::min(depth + e + 1, lastDim);
                    T quadSum = 0;

                    for (int pos = begin; pos < end; ++pos) {
                        T val = (*input)(c * lastDim + pos);
                        quadSum += val * val;
                    }
                    T dividor = nd4j::math::nd4j_pow(bias + alpha * quadSum, beta);
                    (*output)(c * lastDim + e) = (*input)(c * lastDim + e) / dividor;
                }

//                for (int pos = 0; pos < lastDim; ++pos) {
//                    T dividor = nd4j::math::nd4j_pow(bias + alpha * quadSum, beta);
//                    (*output)(c * lastDim + pos) = (*input)(c * lastDim + pos) / dividor;
//                }
            }
            //input->template applyScalar<simdOps::Divide<T>>(dividor, output, nullptr);
            return ND4J_STATUS_OK;
        }

        CUSTOM_OP_IMPL(lrn_bp, 1, 3, true, 4, 0) {
            // LocalResponseNormalization

            NDArray<T>* input = INPUT_VARIABLE(0);
            NDArray<T>* z = OUTPUT_VARIABLE(0);
            NDArray<T>* unitScale = OUTPUT_VARIABLE(1);
            NDArray<T>* scale = OUTPUT_VARIABLE(2);

            REQUIRE_TRUE(input->rankOf() == 4, 0, "Input rank of 4 expected, but got %i instead", input->rankOf());

            T alpha = T_ARG(0);
            T beta = T_ARG(1);
            T bias = T_ARG(2);
            T depth = T_ARG(3);

            int halfDepth = (int) (depth / (T) 2.f);

            const int channel =  input->sizeAt(1);

            auto activitySqr = NDArrayFactory<T>::createUninitialized(input);
            input->template applyPairwiseTransform<simdOps::Multiply<T>>(input, activitySqr, nullptr);
            auto sumPart = activitySqr->dup('c');

            for (int i = 1; i < halfDepth + 1; i++) {
                IndicesList indA({NDIndex::all(), NDIndex::interval(i, channel), NDIndex::all(), NDIndex::all()});
                IndicesList indB({NDIndex::all(), NDIndex::interval(0, channel - i), NDIndex::all(), NDIndex::all()});

                std::unique_ptr<NDArray<T>> tmp(sumPart->subarray(indA));
                std::unique_ptr<NDArray<T>> addVal(activitySqr->subarray(indB));

                tmp.get()->template applyPairwiseTransform<simdOps::Add<T>>(addVal.get(), nullptr);


                std::unique_ptr<NDArray<T>> tmp2(sumPart->subarray(indB));
                std::unique_ptr<NDArray<T>> addVal2(activitySqr->subarray(indA));

                tmp2.get()->template applyPairwiseTransform<simdOps::Add<T>>(addVal2.get(), nullptr);
            }

            /*
             *  // taken from java
                unitScale = sumPart.mul(alpha).addi(k).leverageTo(ComputationGraph.workspaceExternal);
                // y = x * unitScale**-beta
                scale = Transforms.pow(unitScale, -beta).leverageTo(ComputationGraph.workspaceExternal);
                activations = input.mul(scale).leverageTo(ComputationGraph.workspaceExternal);
             */

            sumPart->template applyScalar<simdOps::Multiply<T>>(alpha, unitScale, nullptr);
            unitScale->template applyScalar<simdOps::Add<T>>(bias);

            T p = -beta;
            unitScale->template applyTransform<simdOps::Pow<T>>(scale, &p);
            input->template applyPairwiseTransform<simdOps::Multiply<T>>(scale, z, nullptr);

            STORE_3_RESULTS(*z, *unitScale, *scale);

            delete activitySqr;
            delete sumPart;

            return ND4J_STATUS_OK;
        }
        DECLARE_SYN(LRN, lrn_bp);

        DECLARE_SHAPE_FN(lrn_bp) {
            int *inp = inputShape->at(0);

            auto shapeList = new ShapeList();
            for(int e = 0; e < 3; e++) {
                int *newShape;
                ALLOCATE(newShape, block.getWorkspace(), shape::shapeInfoLength(inp), int);
                memcpy(newShape, inp, shape::shapeInfoByteLength(inp));

                shapeList->push_back(newShape);
            }

            return shapeList;
        }
    }
}