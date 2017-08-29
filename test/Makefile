# Copyright 2017 Neurodata (https://neurodata.io)
# Written by Disa Mhembere (disa.mhembere@gmail.com)
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

include ../Makefile.common

CXXFLAGS += -I../SAFS/libsafs
LDFLAGS := -L../SAFS/libsafs -lsafs $(LDFLAGS)
LIBFILE = ../SAFS/libsafs/libsafs.a

all: test

test: test.o $(LIBFILE)
	$(CXX) -o test test.o $(LDFLAGS)

clean:
	rm -f *.d
	rm -f *.o
	rm -f *~
	rm -f test

-include $(DEPS)
