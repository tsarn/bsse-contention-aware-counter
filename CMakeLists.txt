cmake_minimum_required(VERSION 3.12)
project(contention-aware-counter LANGUAGES C)

add_executable(test ./test.c)
find_package(Threads)
target_link_libraries(test PRIVATE Threads::Threads)
