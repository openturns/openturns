#! /usr/bin/env python

from __future__ import print_function
import os

try:
    width = 40

    # check that python can load OpenTURNS module
    print('1: Python module load'.ljust(width), end=' ')
    try:
        import openturns as ot
        print('OK')
    except ImportError:
        print('no')

    # check that python can find the Viewer module
    # If it fails, check that matplotlib package is installed
    print('2: Viewer (matplotlib)'.ljust(width), end=' ')
    try:
        import openturns.viewer
        print('OK')
    except ImportError:
        print('no')

    # check that OpenTURNS can run R
    # It should produce a file named testDraw.png
    print('3: drawing (R)'.ljust(width), end=' ')
    try:
        graph = ot.Normal().drawPDF()
        fname = 'testDraw.png'
        graph.draw(fname)
        os.remove(fname)
        print('OK')
    except:
        print('no')

    # check XML support
    print('5: serialization (LibXML2)'.ljust(width), end=' ')
    try:
        storageManager = ot.XMLStorageManager('myFile.xml')
        print('OK')
    except:
        print('no')

    # check that math parser is available
    print('6: analytical function (muParser)'.ljust(width), end=' ')
    try:
        ot.ResourceMap.Set("SymbolicParser-Backend", "MuParser")
        f = ot.SymbolicFunction(['x1', 'x2'], ['x1+x2'])
        print('OK')
    except:
        print('no')

    # check that hmat library was found
    print('7: HMatrix (hmat-oss)'.ljust(width), end=' ')
    if ot.HMatrixFactory.IsAvailable():
        print('OK')
    else:
        print('no')

    # check that nlopt library was found
    print('8: optimization (NLopt)'.ljust(width), end=' ')
    if ot.NLopt.IsAvailable():
        print('OK')
    else:
        print('no')

    # check that TBB library was found
    print('9: multithreading (TBB)'.ljust(width), end=' ')
    if ot.TBB.IsAvailable():
        print('OK')
    else:
        print('no')

    # check that psutil was found
    print('10: process control (psutil)'.ljust(width), end=' ')
    try:
        import psutil
        print('OK')
    except ImportError:
        print('no')

    # check that opt++ was found
    print('11: optimization (OPT++)'.ljust(width), end=' ')
    print('OK' if ot.OPTpp.IsAvailable() else 'no')
except:
    import os
    import traceback
    traceback.print_exc()
    os._exit(1)
