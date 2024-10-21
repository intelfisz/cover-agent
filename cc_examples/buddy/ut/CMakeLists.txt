cmake_minimum_required(VERSION 3.10)
project(BuddyAllocatorTest C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -fprofile-arcs -ftest-coverage")

# Find GoogleTest
find_package(GTest REQUIRED)

add_executable(BuddyAllocatorTest buddy_allocator_test.cpp ../buddy_allocator.c)

target_include_directories(BuddyAllocatorTest PRIVATE
						${GTEST_INCLUDE_DIRS}
						..
)


# Add code coverage flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(BuddyAllocatorTest PRIVATE --coverage)
    target_link_options(BuddyAllocatorTest PRIVATE --coverage)
endif()
target_link_libraries(BuddyAllocatorTest PRIVATE
					 gtest gtest_main pthread gcov)
