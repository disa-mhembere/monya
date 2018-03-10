# Copyright 2017 Neurodata (https://neurodata.io)
# Written by Disa Mhembere (disa@cs.jhu.edu)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include ../../../Makefile.common

CXXFLAGS +=-I.. -I../../../SAFS/libsafs
LDFLAGS := -L.. -L../../../SAFS/libsafs -lsafs -lstructures $(LDFLAGS)

TESTFILES = testBinaryNode testRBNode testRBTree testNAryNode \
			perftest cppperftest testScheduler

all: $(TESTFILES)

test: all
	./testBinaryNode
	./testRBNode
	./testRBTree
	./testNAryNode

testBinaryNode: testBinaryNode.o
	$(CXX) -o testBinaryNode testBinaryNode.o $(LDFLAGS)

testRBNode: testRBNode.o
	$(CXX) -o testRBNode testRBNode.o $(LDFLAGS)

testRBTree: testRBTree.o
	$(CXX) -o testRBTree testRBTree.o $(LDFLAGS)

testScheduler: testScheduler.o
	$(CXX) -o testScheduler testScheduler.o $(LDFLAGS)

testNAryNode: testNAryNode.o
	$(CXX) -o testNAryNode testNAryNode.o $(LDFLAGS)

perftest: perftest.o
	$(CXX) -o perftest perftest.o $(LDFLAGS)

cppperftest: cppperftest.o
	$(CXX) -o cppperftest cppperftest.o $(LDFLAGS)

clean:
	rm -f *.d
	rm -f *.o
	rm -f *~
	rm -f $(TESTFILES)

-include $(DEPS)
