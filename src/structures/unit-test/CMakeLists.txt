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

project (monya)

include_directories("${PROJECT_SOURCE_DIR}")

add_executable(cppperftest cppperftest.cpp)
add_executable(perftest perftest.cpp)
add_executable(testBinaryNode testBinaryNode.cpp)
add_executable(testBinaryTree testBinaryTree.cpp)
add_executable(testBuildTaskQueue testBuildTaskQueue.cpp)
add_executable(testNAryNode testNAryNode.cpp)
add_executable(testRBNode testRBNode.cpp)
add_executable(testRBTree testRBTree.cpp)
add_executable(testScheduler testScheduler.cpp)
add_executable(testStack testStack.cpp)
add_executable(testVecBinaryTree testVecBinaryTree.cpp)
add_executable(testWorkerThread testWorkerThread.cpp)

target_link_libraries(cppperftest structures safs)
target_link_libraries(perftest structures safs)
target_link_libraries(testBinaryNode structures safs)
target_link_libraries(testBinaryTree structures safs)
target_link_libraries(testBuildTaskQueue structures safs)
target_link_libraries(testNAryNode structures safs)
target_link_libraries(testRBNode structures safs)
target_link_libraries(testRBTree structures safs)
target_link_libraries(testScheduler structures safs)
target_link_libraries(testStack structures)
target_link_libraries(testVecBinaryTree structures)
target_link_libraries(testWorkerThread structures utils numa)
