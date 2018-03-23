cmake_minimum_required(VERSION 2.8.11)
project(update_verifier)

add_subdirectory(boringssl EXCLUDE_FROM_ALL)
file(GLOB SRCS *.cpp libverifier/*.cpp libverifier/*.h)
include_directories(compat boringssl/include)
link_directories(boringssl/crypto)
add_executable(verifier ${SRCS})
target_link_libraries(verifier crypto)
