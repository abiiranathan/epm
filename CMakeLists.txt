cmake_minimum_required(VERSION 3.0.0)
project(epm VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
file(GLOB SRCS ${SRC_DIR}/*.cpp)

add_executable(epm ${SRCS})
target_include_directories(epm PRIVATE include)
target_compile_options(epm PRIVATE -Wall -Wextra -Wpedantic -Werror -O3 -Wno-unused-value)
target_link_libraries(epm PRIVATE stdc++fs ssl crypto sodium )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
