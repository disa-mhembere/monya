# Copyright 2017 Neurodata (https://neurodata.io)
# Written by Disa Mhembere (disa@cs.jhu.edu)
#
# This file is part of Monya.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required (VERSION 3.10.1)

project (monya)

set(CMAKE_CXX_STANDARD 11)
include(CheckCCompilerFlag)

# The version number.
set (monya_VERSION_MAJOR 0)
set (monya_VERSION_MINOR 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.5.0")
		message(FATAL_ERROR "Insufficient gcc version")
	endif()
else()
	message(FATAL_ERROR "We require GCC")
endif()

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
find_package(LibRt REQUIRED)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -fPIC" )

execute_process(COMMAND bash ${PROJECT_SOURCE_DIR}/get_num_nodes.sh OUTPUT_VARIABLE num_nodes)
message("#NUMA nodes: ${num_nodes}")

find_package(OpenMP)
if (OPENMP_FOUND)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

if (${num_nodes} GREATER 1)
	find_package(hwloc)
	if (hwloc_FOUND)
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_HWLOC")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_HWLOC")
	endif()
endif()

if (${num_nodes} GREATER 1)
	find_package(LibNuma)
	if (LIBNUMA_FOUND)
		message(STATUS "Found libnuma")
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_NUMA")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_NUMA")
	endif()
endif()

find_package(LibAio)
if (LIBAIO_FOUND)
	message(STATUS "Found libaio.")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_LIBAIO")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_LIBAIO")
endif()

check_c_compiler_flag("-mavx" HAVE_FLAG_M_AVX)
if(HAVE_FLAG_M_AVX)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
endif()

#set(CMAKE_BUILD_TYPE Release)

include_directories(SAFS/libsafs src)

subdirs(SAFS/libsafs src)
