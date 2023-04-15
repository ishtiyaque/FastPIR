
cmake_minimum_required(VERSION 3.10)

project(FastPIR VERSION 1.0 LANGUAGES CXX)

# Executable will be in ../bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)

add_executable(fastpir main.cpp server.cpp client.cpp fastpirparams.cpp)

# Import Microsoft SEAL
find_package(SEAL 4.1 REQUIRED)

# Link Microsoft SEAL
target_link_libraries(fastpir SEAL::seal)
