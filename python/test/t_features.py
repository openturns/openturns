#! /usr/bin/env python

from __future__ import print_function
import os

width = 40

# check that python can load OpenTURNS module
print(' 1: import openturns'.ljust(width), end=' ')
try:
    import openturns as ot
    print(ot.__version__)
except ImportError:
    print('n/a')
    raise

# check that python can find the Viewer module
# If it fails, check that matplotlib package is installed
print(' 2: plotting (matplotlib)'.ljust(width), end=' ')
try:
    import openturns.viewer
    print('ok')
except ImportError:
    print('n/a')

# check that OpenTURNS can run R
# It should produce a file named testDraw.png
print(' 3: legacy plotting (R)'.ljust(width), end=' ')
try:
    fname = 'testDraw.png'
    ot.Normal().drawPDF().draw(fname)
    graph.draw(fname)
    os.remove(fname)
    print('ok')
except:
    print('n/a')

# check XML support
print(' 4: serialization (LibXML2)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('libxml2') else 'n/a')

# check HDF5 support
print(' 5: serialization (HDF5)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('hdf5') else 'n/a')

# check that dill was found
print(' 6: serialization (dill)'.ljust(width), end=' ')
try:
    import dill
    print('ok')
except ImportError:
    print('n/a')

# check that math parser is available
print(' 7: legacy symbolic function (muParser)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('muparser') else 'n/a')

# check that hmat library was found
print(' 8: h-matrix (hmat-oss)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('hmat') else 'n/a')

# check that spectra library was found
print(' 9: iterative SVD (Spectra)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('spectra') else 'n/a')

# check that TBB library was found
print(' 10: multithreading (TBB)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('tbb') else 'n/a')

# check that psutil was found
print('11: process control (psutil)'.ljust(width), end=' ')
try:
    import psutil
    print('ok')
except ImportError:
    print('n/a')

# check that nlopt library was found
print('12: optimization (NLopt)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('nlopt') else 'n/a')

# check that cminpack was found
print('13: optimization (CMinpack)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('cminpack') else 'n/a')

# check that ceres was found
print('14: optimization (Ceres Solver)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('ceres') else 'n/a')

# check that dlib was found
print('15: optimization (Dlib)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('dlib') else 'n/a')

# check that dlib was found
print('16: optimization (Bonmin)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('bonmin') else 'n/a')

# check that ipopt was found
print('17: optimization (Ipopt)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('ipopt') else 'n/a')

# check that bison/flex were found
print('18: CSV parser (bison/flex)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('bison') else 'n/a')

# check that boost library was found
print('19: special functions (Boost)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('boost') else 'n/a')

# check that mpfr library was found
print('20: special functions (MPFR)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('mpfr') else 'n/a')

# check that mpc library was found
print('21: special functions (MPC)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('mpc') else 'n/a')

# check that primesieve library was found
print('22: fast prime numbers (Primesieve)'.ljust(width), end=' ')
print('ok' if ot.PlatformInfo.HasFeature('primesieve') else 'n/a')
