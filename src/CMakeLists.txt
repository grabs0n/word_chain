cmake_minimum_required(VERSION 3.23)
project(word_chain C)

set(CMAKE_C_STANDARD 99)

add_executable(word_chain main.c dictionary.c)

