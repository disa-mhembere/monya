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

TESTFILES = testIndexVector metric-test testNNVector testTypes

all: $(TESTFILES)

test:
	for f in $(TESTFILES); do ./$$f; done

testIndexVector: testIndexVector.o
	$(CXX) -o testIndexVector testIndexVector.o $(LDFLAGS)

metric-test: metric-test.o
	$(CXX) -o metric-test metric-test.o $(LDFLAGS)

testNNVector: testNNVector.o
	$(CXX) -o testNNVector testNNVector.o $(LDFLAGS)

testTypes: testTypes.o
	$(CXX) -o testTypes testTypes.o $(LDFLAGS)

clean:
	rm -f *.d
	rm -f *.o
	rm -f *~
	rm -f $(TESTFILES)

-include $(DEPS)
