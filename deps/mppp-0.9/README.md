# mp++

[![Build Status](https://travis-ci.org/bluescarni/mppp.svg?branch=master)](https://travis-ci.org/bluescarni/mppp)
[![Build status](https://ci.appveyor.com/api/projects/status/github/bluescarni/mppp?branch=master&svg=true)](https://ci.appveyor.com/project/bluescarni/mppp)
[![codecov.io](https://codecov.io/github/bluescarni/mppp/coverage.svg?branch=master)](https://codecov.io/github/bluescarni/mppp?branch=master)
[![Docs](https://media.readthedocs.org/static/projects/badges/passing.svg)](http://bluescarni.github.io/mppp/)
[![Join the chat at https://gitter.im/bluescarni/mppp](https://badges.gitter.im/bluescarni/mppp.svg)](https://gitter.im/bluescarni/mppp?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/mppp/badges/version.svg)](https://anaconda.org/conda-forge/mppp)
[![DOI](https://zenodo.org/badge/66504757.svg)](https://doi.org/10.5281/zenodo.1043579)

mp++ is a C++11 library for multiprecision arithmetic, currently supporting arbitrary-precision integers,
rationals and floats, and quadruple-precision floats.

Based on well-known libraries such as [GMP](https://gmplib.org/), [MPFR](http://www.mpfr.org), and others,
mp++ was initially conceived as a [GMP](https://gmplib.org/) wrapper with a special focus on performance with
small operands. In particular, a small buffer optimisation and custom implementations of basic mathematical primitives are
instrumental in achieving a performance increase, with respect to GMP and other integer multiprecision libraries, which can be
substantial (see the [benchmarks](https://bluescarni.github.io/mppp/benchmarks.html) section of the documentation).
The price to pay is a small overhead when operating on large integers.

Eventually, a multiprecision rational class and two multiprecision floating-point classes were added, and today a secondary objective
of mp++ is to provide a modern, consistent and unified C++ interface to several lower-level multiprecision libraries.

mp++ is a spinoff of the [Piranha](https://github.com/bluescarni/piranha) library, released under the
[MPL2](https://www.mozilla.org/en-US/MPL/2.0/FAQ/) license.

If you are using mp++ as part of your research, teaching, or other activities, we would be grateful if you could star
the repository and/or cite our work. The DOI of the latest version and other citation resources are available
at [this link](https://doi.org/10.5281/zenodo.1043579).

The documentation is available [here](https://bluescarni.github.io/mppp/).
