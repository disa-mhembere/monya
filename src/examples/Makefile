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

include ../../Makefile.common

CXXFLAGS +=-I../../SAFS/libsafs -I../common \
		   -I../structures -I../utils -I..
LDFLAGS +=-L../../SAFS/libsafs -L../common -L../structures -L../utils -L..\
		  -lstructures -lmonya -lutils -lsafs

EXAMPLES=kdtree rand-kdtree

all: $(EXAMPLES)

kdtree: kdtree.o
	$(CXX) -o kdtree kdtree.o $(LDFLAGS)

rand-kdtree: rand-kdtree.o
	$(CXX) -o rand-kdtree rand-kdtree.o $(LDFLAGS)

clean:
	rm -f *.d
	rm -f *.o
	rm -f *~
	rm -f $(EXAMPLES)
	rm -f *.a

-include $(DEPS)
