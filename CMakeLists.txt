cmake_minimum_required (VERSION 2.6)
project (rockre)

enable_testing()

include_directories("include")

# -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wfloat-equal -Wpointer-arith
if(!WIN32)
	SET(SUPPORT_PIC, "-fPIC")
endif()
SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -O2 -g -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare ${SUPPORT_PIC}")
SET(CMAKE_CXX_FLAGS_DEBUG   "-Wall -W -Wextra -Wno-unused-function -g -Werror ${SUPPORT_PIC}")
SET(CMAKE_C_FLAGS   "-std=c99 -Wall -W -Wextra -Wno-unused-function -g -Werror ${SUPPORT_PIC}")
SET(CMAKE_BUILD_TYPE Debug)

SET(serial "0.0.1")
SET(soserial "1")

SET(SRCS src/rockre_node.cc src/rockre_codegen.cc src/rockre_vm.cc src/rockre_parse.cc src/api.cc)

add_library(rockre SHARED ${SRCS})
ADD_LIBRARY(rockre-static STATIC ${SRCS})
SET_TARGET_PROPERTIES(rockre-static PROPERTIES OUTPUT_NAME "rockre")

add_executable(test_rockre test.c)
target_link_libraries(test_rockre rockre)

add_test(NAME prove COMMAND prove)
add_custom_target(prove DEPENDS test_rockre)

