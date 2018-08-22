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

TESTFILES = testIOFactory testIO test_vecs_reader

all: $(TESTFILES)

CXXFLAGS +=-I..
LDFLAGS := -L..

test:
	for f in $(TESTFILES); do ./$$f; done

testIOFactory: testIOFactory.o
	$(CXX) -o testIOFactory testIOFactory.o $(LDFLAGS)

testIO: testIO.o
	$(CXX) -o testIO testIO.o $(LDFLAGS)

test_vecs_reader: test_vecs_reader.o
	$(CXX) -o test_vecs_reader test_vecs_reader.o $(LDFLAGS)

clean:
	rm -f *.d
	rm -f *.o
	rm -f *~
	rm -f $(TESTFILES)

-include $(DEPS)
