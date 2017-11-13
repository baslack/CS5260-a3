cmake_minimum_required(VERSION 3.8)
project(a3)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(CMAKE_C_STANDARD 99)

set(A31_FILES a31.c)
set(A32_FILES a32.c)
set(A33_FILES a33.c)

add_executable(a31 ${A31_FILES})
add_executable(a32 ${A32_FILES})
add_executable(a33 ${A33_FILES})

target_link_libraries(a33 m)