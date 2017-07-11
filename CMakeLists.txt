cmake_minimum_required (VERSION 2.6)
project (GeneticAlgo)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -fdiagnostics-color=auto -fmax-errors=2 -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fmax-errors=2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

#UNIT TESTS
enable_testing()
function(add_test_helper test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} libgtest libgtest_main)
    add_test(${test_name} ${test_name})
endfunction()
add_subdirectory(test)
add_subdirectory(bin)
