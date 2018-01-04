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

include Makefile.common

all: build_lib testdir src

build_lib:
	$(MAKE) -C SAFS

testdir: build_lib
	$(MAKE) -C test

src: build_lib
	$(MAKE) -C src

clean:
	rm -f *.d
	rm -f *.o
	rm -f *~
	find -name core -delete
	$(MAKE) --ignore-errors -C SAFS clean
	$(MAKE) --ignore-errors -C test clean
	$(MAKE) --ignore-errors -C src clean
-include $(DEPS)
