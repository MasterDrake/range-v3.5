set(CMAKE_FOLDER "${CMAKE_FOLDER}/utility")

if (EARANGES_COROUTINE_FLAGS)
  EARanges_add_test(test.generator generator generator.cpp)
endif()
