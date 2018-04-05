#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import os
import sys
import math as m

ot.TESTPREAMBLE()

try:
    fileName = 'myStudy.xml'

    # Create a Study Object by name
    myStudy = ot.Study(fileName)
    point = ot.Point(2, 1.0)
    myStudy.add("point", point)
    myStudy.save()
    myStudy2 = ot.Study(fileName)
    myStudy2.load()
    point2 = ot.Point()
    myStudy2.fillObject("point", point2)
    # cleanup
    os.remove(fileName)

    # Create a Study Object with compression
    myStudy = ot.Study()
    compressionLevel = 5
    myStudy.setStorageManager(ot.XMLStorageManager(fileName, compressionLevel))
    point = ot.Point(2, 1.0)
    myStudy.add("point", point)
    myStudy.save()
    myStudy2 = ot.Study(fileName)
    myStudy2.load()
    point2 = ot.Point()
    myStudy2.fillObject("point", point2)
    # cleanup
    os.remove(fileName)

    # Create a Study Object
    myStudy = ot.Study()
    myStudy.setStorageManager(ot.XMLStorageManager(fileName))

    # Add a PersistentObject to the Study (here a Point)
    p1 = ot.Point(3, 0.)
    p1.setName('Good')
    p1[0] = 10.
    p1[1] = 11.
    p1[2] = 12.
    myStudy.add(p1)

    # Add another PersistentObject to the Study (here a Sample)
    s1 = ot.Sample(3, 2)
    s1.setName('mySample')
    p2 = ot.Point(2, 0.)
    p2.setName('One')
    p2[0] = 100.
    p2[1] = 200.
    s1[0] = p2
    p3 = ot.Point(2, 0.)
    p3.setName('Two')
    p3[0] = 101.
    p3[1] = 201.
    s1[1] = p3
    p4 = ot.Point(2, 0.)
    p4.setName('Three')
    p4[0] = 102.
    p4[1] = 202.
    s1[2] = p4
    myStudy.add('mySample', s1)

    # Add a point with a description
    pDesc = ot.PointWithDescription(p1)
    desc = pDesc.getDescription()
    desc[0] = 'x'
    desc[1] = 'y'
    desc[2] = 'z'
    pDesc.setDescription(desc)
    myStudy.add(pDesc)

    # Add a matrix
    matrix = ot.Matrix(2, 3)
    matrix[0, 0] = 0
    matrix[0, 1] = 1
    matrix[0, 2] = 2
    matrix[1, 0] = 3
    matrix[1, 1] = 4
    matrix[1, 2] = 5
    myStudy.add('m', matrix)

    # Create a Point that we will try to reinstaciate after reloading
    point = ot.Point(2, 1000.)
    point.setName('point')
    myStudy.add('point', point)

    # Create a Simulation::Result
    simulationResult = ot.SimulationResult(ot.Event(), 0.5, 0.01, 150, 4)
    myStudy.add('simulationResult', simulationResult)

    cNameList = [
        'LHS', 'DirectionalSampling', 'SimulationSensitivityAnalysis', 'ProbabilitySimulationAlgorithm']
    for cName in cNameList:
        otClass = getattr(ot, cName)
        instance = otClass()
        myStudy.add(cName, instance)

    # Create a Beta distribution
    beta = ot.Beta(3.0, 5.0, -1.0, 4.0)
    myStudy.add('beta', beta)

    # Create an analytical Function
    input = ot.Description(3)
    input[0] = 'a'
    input[1] = 'b'
    input[2] = 'c'
    formulas = ot.Description(3)
    formulas[0] = 'a+b+c'
    formulas[1] = 'a-b*c'
    formulas[2] = '(a+2*b^2+3*c^3)/6'
    analytical = ot.SymbolicFunction(input, formulas)
    analytical.setName('analytical')
    myStudy.add('analytical', analytical)

    # Create a TaylorExpansionMoments algorithm
    antecedent = ot.RandomVector(
        ot.IndependentCopula(analytical.getInputDimension()))
    antecedent.setName('antecedent')
    composite = ot.RandomVector(analytical, antecedent)
    composite.setName('composite')
    taylorExpansionsMoments = ot.TaylorExpansionMoments(composite)
    taylorExpansionsMoments.setName('taylorExpansionsMoments')
    taylorExpansionsMoments.getMeanFirstOrder()
    taylorExpansionsMoments.getMeanSecondOrder()
    taylorExpansionsMoments.getCovariance()

    myStudy.add('taylorExpansionsMoments', taylorExpansionsMoments)

    # Create a FORMResult
    input2 = ot.Description(2)
    input2[0] = 'x'
    input2[1] = 'y'
    formula2 = ot.Description(1)
    formula2[0] = 'y^2-x'
    model = ot.SymbolicFunction(input2, formula2)
    model.setName('sum')
    input3 = ot.RandomVector(ot.Normal(2))
    input3.setName('input')
    output3 = ot.RandomVector(model, input3)
    output3.setName('output')
    event = ot.Event(output3, ot.Greater(), 1.0)
    event.setName('failureEvent')
    designPoint = ot.Point(2, 0.0)
    designPoint[0] = 1.0
    formResult = ot.FORMResult(ot.Point(2, 1.0), event, False)
    formResult.setName('formResult')
    formResult.getImportanceFactors()
    formResult.getEventProbabilitySensitivity()
    myStudy.add('formResult', formResult)

    # Create a SORMResult
    sormResult = ot.SORMResult([1.0] * 2, event, False)
    sormResult.setName('sormResult')
    sormResult.getEventProbabilityBreitung()
    sormResult.getEventProbabilityHohenBichler()
    sormResult.getEventProbabilityTvedt()
    sormResult.getGeneralisedReliabilityIndexBreitung()
    sormResult.getGeneralisedReliabilityIndexHohenBichler()
    sormResult.getGeneralisedReliabilityIndexTvedt()
    myStudy.add('sormResult', sormResult)

    # Create a RandomGeneratorState
    ot.RandomGenerator.SetSeed(0)
    randomGeneratorState = ot.RandomGeneratorState(
        ot.RandomGenerator.GetState())
    myStudy.add('randomGeneratorState', randomGeneratorState)

    # Create a GeneralLinearModelResult
    generalizedLinearModelResult = ot.GeneralLinearModelResult()
    generalizedLinearModelResult.setName('generalizedLinearModelResult')
    myStudy.add('generalizedLinearModelResult', generalizedLinearModelResult)

    # KDTree
    sample = ot.Normal(3).getSample(10)
    kDTree = ot.KDTree(sample)
    myStudy.add('kDTree', kDTree)

    # TensorApproximationAlgorithm/Result
    dim = 1
    model = ot.SymbolicFunction(['x'], ['x*sin(x)'])
    distribution = ot.ComposedDistribution([ot.Uniform()] * dim)
    factoryCollection = [ot.FourierSeriesFactory()] * dim
    functionFactory = ot.OrthogonalProductFunctionFactory(factoryCollection)
    size = 10
    X = distribution.getSample(size)
    Y = model(X)
    nk = [5] * dim
    rank = 1
    algo = ot.TensorApproximationAlgorithm(
        X, Y, distribution, functionFactory, nk, rank)
    algo.run()
    tensorResult = algo.getResult()
    myStudy.add('tensorResult', tensorResult)
    tensorIn = [0.4]
    tensorRef = tensorResult.getMetaModel()(tensorIn)

    # print 'Study = ' , myStudy
    myStudy.save()

    # Create a new Study Object
    myStudy = ot.Study()
    myStudy.setStorageManager(ot.XMLStorageManager(fileName))

    myStudy.load()
    # print 'loaded Study = ' , myStudy

    # Create a Point from the one stored in the Study
    point = ot.Point()
    myStudy.fillObject('point', point)

    print('point = ', repr(point))

    # Create a Sample from the one stored in the Study
    sample = ot.Sample()
    myStudy.fillObject('mySample', sample)

    print('sample = ', repr(sample))

    # Create a Matrix from the one stored in the Study
    matrix = ot.Matrix()
    myStudy.fillObject('m', matrix)

    print('matrix = ', repr(matrix))

    # Create a Simulation::Result from the one stored in the Study
    simulationResult = ot.SimulationResult()
    myStudy.fillObject('simulationResult', simulationResult)

    print('simulation result = ', simulationResult)

    for cName in cNameList:
        otClass = getattr(ot, cName)
        instance = otClass()
        saved = repr(instance)
        myStudy.fillObject(cName, instance)
        loaded = repr(instance)
        if saved != loaded:
            print('saved=', saved)
            print('loaded=', loaded)

    # Create a Beta distribution from the one stored in the Study
    beta = ot.Beta()
    myStudy.fillObject('beta', beta)

    print('beta = ', beta)

    randomGeneratorState = ot.RandomGeneratorState()

    myStudy.fillObject('randomGeneratorState', randomGeneratorState)

    print('randomGeneratorState = ', randomGeneratorState)

    # Create an analytical Function from the one stored in the
    # Study
    analytical = ot.Function()
    myStudy.fillObject('analytical', analytical)

    print('analytical = ', analytical)

    # Create a GeneralLinearModelResult from the one stored in the Study
    generalizedLinearModelResult = ot.GeneralLinearModelResult()
    myStudy.fillObject('generalizedLinearModelResult',
                       generalizedLinearModelResult)

    print('generalizedLinearModelResult = ', generalizedLinearModelResult)

    # KDTree
    kDTree = ot.KDTree()
    myStudy.fillObject('kDTree', kDTree)
    print('kDTree = ', kDTree)

    # Tensor
    tensorResult = ot.MetaModelResult()
    myStudy.fillObject('tensorResult', tensorResult)
    ot.testing.assert_almost_equal(
        tensorResult.getMetaModel()(tensorIn), tensorRef)

    # cleanup
    os.remove(fileName)


    # test nan/inf
    myStudy = ot.Study(fileName)
    point = ot.Point([float(x) for x in ['nan', 'inf', '-inf']])
    myStudy.add("point", point)
    myStudy.save()
    myStudy2 = ot.Study(fileName)
    myStudy2.load()
    point2 = ot.Point()
    myStudy2.fillObject("point", point2)
    for j in range(len(point2)):
        print('j=', j,
              'isnormal=', ot.SpecFunc.IsNormal(point2[j]),
              'isnan=', m.isnan(point2[j]),
              'isinf=', m.isinf(point2[j]))
    # cleanup
    os.remove(fileName)

except:
    import os
    import traceback
    traceback.print_exc()
    os._exit(1)
