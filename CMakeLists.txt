cmake_minimum_required(VERSION 3.17)
project(learnedlsm LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

option(LLSM_BUILD_TESTS "Set to build the Learned LSM test suite." OFF)
option(LLSM_BUILD_SHARED "Set to build the Learned LSM database as a shared library." OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Load any third party dependencies
add_subdirectory(third_party)

# The Learned LSM embedded database library target
if (LLSM_BUILD_SHARED)
  add_library(llsm SHARED)
else()
  add_library(llsm STATIC)
endif()
target_include_directories(llsm
  PUBLIC include
  PRIVATE .)

# Add API headers to the target for IDE support
set(llsm_inc include/llsm)
set(llsm_api
  ${llsm_inc}/db.h
  ${llsm_inc}/options.h
  ${llsm_inc}/slice.h
  ${llsm_inc}/status.h)
target_sources(llsm PUBLIC ${llsm_api})

# Add our sources by traversing the repository
add_subdirectory(bufmgr)
add_subdirectory(db)
add_subdirectory(util)

# Build the tests iff requested
if(LLSM_BUILD_TESTS)
  add_subdirectory(tests)
endif()