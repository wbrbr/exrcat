cmake_minimum_required(VERSION 3.5)

project(exrcat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(exrcat main.cpp)

find_package(OpenEXR REQUIRED)

target_link_libraries(exrcat IlmImf Half)
