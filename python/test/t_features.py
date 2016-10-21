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
    except:
        print('no')

    # check that python can find the Viewer module
    # If it fails, check that matplotlib package is installed
    print('2: Viewer (matplotlib)'.ljust(width), end=' ')
    try:
        import openturns.viewer
        print('OK')
    except:
        print('no')

    # check that OpenTURNS can run R
    # It should produce a file named testDraw.png
    print('3: drawing (R)'.ljust(width), end=' ')
    try:
        graph = ot.Normal().drawPDF()
        fname = 'testDraw.png'
        try:
            graph.draw(fname)
            os.remove(fname)
        except:
            raise
        print('OK')
    except:
        print('no')

    # check that rot package is installed
    print('4: linear model (R.rot)'.ljust(width), end=' ')
    try:
        lm = ot.LinearModelFactory().build(
            ot.Normal(2).getSample(10), ot.Normal().getSample(10))
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

    # check that analytical function are available
    print('6: analytical function (muParser)'.ljust(width), end=' ')
    try:
        f = ot.NumericalMathFunction(['x1', 'x2'], ['y'], ['x1+x2'])
        print('OK')
    except:
        print('no')

    # check that hmat library was found
    print('7: HMatrix (hmat-oss)'.ljust(width), end=' ')
    try:
        # This is a little bit tricky because HMat 1.0 fails with 1x1 matrices
        ot.ResourceMap.SetAsUnsignedInteger(
            'TemporalNormalProcess-SamplingMethod', 1)
        vertices = [[0.0, 0.0, 0.0]]
        vertices.append([1.0, 0.0, 0.0])
        vertices.append([0.0, 1.0, 0.0])
        vertices.append([0.0, 0.0, 1.0])
        simplices = [[0, 1, 2, 3]]
        # Discard messages from HMat
        ot.Log.Show(0)
        process = ot.TemporalNormalProcess(
            ot.ExponentialModel(3), ot.Mesh(vertices, simplices))
        f = process.getRealization()
        print('OK')
    except:
        print('no')

    # check that nlopt library was found
    print('8: optimization (NLopt)'.ljust(width), end=' ')
    try:
        problem = ot.OptimizationProblem()
        algo = ot.SLSQP()
        algo.setProblem(problem)
        print('OK')
    except:
        print('no')

    # check that TBB library was found
    print('9: multithreading (TBB)'.ljust(width), end=' ')
    if ot.TBB.IsAvailable():
        print('OK')
    else:
        print('no')

except:
    import os
    import traceback
    traceback.print_exc()
    os._exit(1)
