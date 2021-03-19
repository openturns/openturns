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
try:
    ot.XMLStorageManager('myFile.xml')
    print('ok')
except:
    print('n/a')

# check HDF5 support
print(' 5: HDF5 compatibility '.ljust(width), end=' ')
try:
    ot.XMLH5StorageManager('myFile.xml')
    print('ok')
except:
    print('n/a')

# check that math parser is available
print(' 6: legacy symbolic function (muParser)'.ljust(width), end=' ')
try:
    ot.ResourceMap.Set("SymbolicParser-Backend", "MuParser")
    f = ot.SymbolicFunction(['x1', 'x2'], ['x1+x2'])
    print('ok')
except:
    print('n/a')

# check that hmat library was found
print(' 7: h-matrix (hmat-oss)'.ljust(width), end=' ')
print('ok' if ot.HMatrixFactory.IsAvailable() else 'n/a')

# check that spectra library was found
print(' 8: iterative SVD (Spectra)'.ljust(width), end=' ')
ot.ResourceMap.SetAsString(
    "KarhunenLoeveP1Algorithm-EigenvaluesSolver", "SPECTRA")
mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
cov1D = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov1D, 1e-3)
try:
    algo.run()
    result = algo.getResult()
    lambd = result.getEigenvalues()
    print('ok')
except:
    print('n/a')

# check that TBB library was found
print(' 9: multithreading (TBB)'.ljust(width), end=' ')
print('ok' if ot.TBB.IsAvailable() else 'n/a')

# check that psutil was found
print('10: process control (psutil)'.ljust(width), end=' ')
try:
    import psutil
    print('ok')
except ImportError:
    print('n/a')

# check that dill was found
print('11: serialization (dill)'.ljust(width), end=' ')
try:
    import dill
    print('ok')
except ImportError:
    print('n/a')

# check that nlopt library was found
print('12: optimization (NLopt)'.ljust(width), end=' ')
print('ok' if ot.NLopt.IsAvailable() else 'n/a')

# check that cminpack was found
print('13: optimization (CMinpack)'.ljust(width), end=' ')
print('ok' if ot.CMinpack.IsAvailable() else 'n/a')

# check that ceres was found
print('14: optimization (Ceres Solver)'.ljust(width), end=' ')
print('ok' if ot.Ceres.IsAvailable() else 'n/a')

# check that dlib was found
print('15: optimization (Dlib)'.ljust(width), end=' ')
print('ok' if ot.Dlib.IsAvailable() else 'n/a')

# check that dlib was found
print('16: optimization (Bonmin)'.ljust(width), end=' ')
print('ok' if ot.Bonmin.IsAvailable() else 'n/a')

# check that ipopt was found
print('17: optimization (Ipopt)'.ljust(width), end=' ')
print('ok' if ot.Ipopt.IsAvailable() else 'n/a')

# check that bison/flex were found
print('18: CSV parser (bison/flex)'.ljust(width), end=' ')
ot.Normal(2).getSample(30).exportToCSVFile('feat.csv')
try:
    sample = ot.Sample.ImportFromCSVFile('feat.csv')
    print('ok')
except RuntimeError:
    print('n/a')
os.remove('feat.csv')
