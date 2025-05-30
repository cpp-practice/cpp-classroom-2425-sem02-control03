cmake_minimum_required (VERSION 3.8)

project(semester-02-control-03)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX") # CMake already contains W3 in its flags
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wextra-semi \
                         -O0 -g -fsanitize=address -fno-omit-frame-pointer")
endif()

include_directories(src)

# add_executable(task-1-test src/task_1/smoke.cpp)
# add_executable(task-2-test src/task_2/smoke.cpp)
add_executable(task-2-test-ncomp-functions src/task_2/ncomp_functions.cpp)
add_executable(task-2-test-ncomp-functors src/task_2/ncomp_functors.cpp)
