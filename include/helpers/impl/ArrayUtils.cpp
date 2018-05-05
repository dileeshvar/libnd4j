//
// @author raver119@gmail.com
//

#include <helpers/ArrayUtils.h>

namespace nd4j {
    namespace ArrayUtils {
        void toIntPtr(std::initializer_list<int> list, int* target) {
            std::vector<int> vec(list);
            toIntPtr(vec, target);
        }

        void toIntPtr(std::vector<int>& list, int* target) {
            memcpy(target, list.data(), list.size() * sizeof(int));
        }

        void toLongPtr(std::initializer_list<Nd4jLong> list, Nd4jLong* target) {
            std::vector<Nd4jLong> vec(list);
            toIntPtr(vec, target);
        }

        void toLongPtr(std::vector<Nd4jLong>& list, Nd4jLong* target) {
            memcpy(target, list.data(), list.size() * sizeof(Nd4jLong));
        }
    }
}
