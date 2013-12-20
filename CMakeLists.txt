cmake_minimum_required (VERSION 2.6)
project (rockre)

# -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wfloat-equal -Wpointer-arith
SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -Wall -O2 -g -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare")
SET(CMAKE_CXX_FLAGS_DEBUG   "-std=c++11 -Wall -W -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -g -Werror")
SET(CMAKE_BUILD_TYPE Debug)

SET(serial "0.0.1")
SET(soserial "1")

SET(SRCS rockre_node.cc rockre_codegen.cc rockre_vm.cc rockre_parse.cc)

add_library(rockre STATIC ${SRCS})

add_executable(test_rockre rockre test.cc)
target_link_libraries(test_rockre rockre)

add_custom_target(test DEPENDS test_rockre SOURCES ${SRCS} COMMAND prove -v t/*.t) 

