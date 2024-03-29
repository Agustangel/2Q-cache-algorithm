cmake_minimum_required(VERSION 3.14)
project(cache)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_definitions(-DDEBUG)

add_executable(
  cache_bin
  src/cache.cpp
)
add_executable(
  ideal_cache_bin
  src/ideal_cache.cpp
)
add_executable(
  cache_test
  tests/tests.cpp
)
add_executable(
  ideal_cache_test
  tests/ideal_tests.cpp
)
target_link_libraries(
  cache_test
  GTest::gtest_main
)
target_link_libraries(
  ideal_cache_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(cache_test)
gtest_discover_tests(ideal_cache_test)

target_include_directories(cache_bin PUBLIC include)
target_include_directories(ideal_cache_bin PUBLIC include)
target_include_directories(cache_test PUBLIC include)
target_include_directories(ideal_cache_test PUBLIC include)
