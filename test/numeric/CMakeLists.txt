set(CMAKE_FOLDER "${CMAKE_FOLDER}/numeric")

EARanges_add_test(test.num.accumulate num.accumulate accumulate.cpp)
EARanges_add_test(test.num.adjacent_difference num.adjacent_difference adjacent_difference.cpp)
EARanges_add_test(test.num.inner_product num.inner_product inner_product.cpp)
EARanges_add_test(test.num.iota num.iota iota.cpp)
EARanges_add_test(test.num.partial_sum num.partial_sum partial_sum.cpp)
