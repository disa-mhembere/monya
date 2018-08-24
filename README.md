# Monya &nbsp; &nbsp; &nbsp;![monya](img/monya-thumbnail.png)
[![Build
Status](https://travis-ci.org/disa-mhembere/monya.svg?branch=master)](https://travis-ci.org/disa-mhembere/monya)

Optimizes Semi-External Memory Tree Operations for machine learning.

**It does the heavy lifting for you!**

## Monya's backbone

- [p-threads](https://computing.llnl.gov/tutorials/pthreads/) for
multithreading.
- [numa](https://linux.die.net/man/3/numa) for NUMA optimization.
- [SAFS](https://github.com/flashxio/FlashX/tree/release/libsafs) for a semi-external memory
vertex-centric interface.

## System Requirements

- Linux or Mac OS 10.*
- At least **4 (GB) of RAM**
- Administrative privileges


## Installation

The following is Tested on **Ubuntu 14.04 - 16.04**. We require a minimum of
`gcc-5` for compilation.

## Problems ?

Post an [issue](https://github.com/disa-mhembere/monya/issues) and we'll get to
it as soon as possible.


# Appendix

Uses `libSAFS`
([[pdf](http://www.cs.jhu.edu/~zhengda/sc13.pdf)][[bib](http://dl.acm.org/downformats.cfm?id=2503225&parent_id=2503210&expformat=bibtex&CFID=445591569&CFTOKEN=95321450)])
as the io backend for external memory applications.
