#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import os

ot.TESTPREAMBLE()

try:
    fileName = 'myStudy.xml'

    # Create a Study Object
    myStudy = ot.Study()
    myStudy.setStorageManager(ot.XMLStorageManager(fileName))

    # Add a PersistentObject to the Study (here a NumericalPoint)
    p1 = ot.NumericalPoint(3, 0.)
    p1.setName('Good')
    p1[0] = 10.
    p1[1] = 11.
    p1[2] = 12.
    myStudy.add(p1)

    # Add another PersistentObject to the Study (here a NumericalSample)
    s1 = ot.NumericalSample(3, 2)
    s1.setName('mySample')
    p2 = ot.NumericalPoint(2, 0.)
    p2.setName('One')
    p2[0] = 100.
    p2[1] = 200.
    s1[0] = p2
    p3 = ot.NumericalPoint(2, 0.)
    p3.setName('Two')
    p3[0] = 101.
    p3[1] = 201.
    s1[1] = p3
    p4 = ot.NumericalPoint(2, 0.)
    p4.setName('Three')
    p4[0] = 102.
    p4[1] = 202.
    s1[2] = p4
    myStudy.add('mySample', s1)

    # Add a point with a description
    pDesc = ot.NumericalPointWithDescription(p1)
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

    # Create a NumericalPoint that we will try to reinstaciate after reloading
    point = ot.NumericalPoint(2, 1000.)
    point.setName('point')
    myStudy.add('point', point)

    # Create a Simulation::Result
    simulationResult = ot.SimulationResult(ot.Event(), 0.5, 0.01, 150, 4)
    myStudy.add('simulationResult', simulationResult)

    cNameList = ['MonteCarlo', 'LHS', 'QuasiMonteCarlo', 'RandomizedQuasiMonteCarlo',
                 'DirectionalSampling', 'RandomizedLHS', 'SimulationSensitivityAnalysis']
    for cName in cNameList:
        otClass = getattr(ot, cName)
        instance = otClass()
        myStudy.add(cName, instance)

    # Create a Beta distribution
    beta = ot.Beta(3.0, 5.0, -1.0, 4.0)
    myStudy.add('beta', beta)

    # Create an analytical NumericalMathFunction
    input = ot.Description(3)
    input[0] = 'a'
    input[1] = 'b'
    input[2] = 'c'
    output = ot.Description(3)
    output[0] = 'squaresum'
    output[1] = 'prod'
    output[2] = 'complex'
    formulas = ot.Description(output.getSize())
    formulas[0] = 'a+b+c'
    formulas[1] = 'a-b*c'
    formulas[2] = '(a+2*b^2+3*c^3)/6'
    analytical = ot.NumericalMathFunction(input, output, formulas)
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
    output2 = ot.Description(1)
    output2[0] = 'd'
    formula2 = ot.Description(1)
    formula2[0] = 'y^2-x'
    model = ot.NumericalMathFunction(input2, output2, formula2)
    model.setName('sum')
    input3 = ot.RandomVector(ot.Normal(2))
    input3.setName('input')
    output3 = ot.RandomVector(model, input3)
    output3.setName('output')
    event = ot.Event(output3, ot.Greater(), 1.0)
    event.setName('failureEvent')
    designPoint = ot.NumericalPoint(2, 0.0)
    designPoint[0] = 1.0
    formResult = ot.FORMResult(ot.NumericalPoint(2, 1.0), event, False)
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

    # Create a GeneralizedLinearModelResult
    generalizedLinearModelResult = ot.GeneralizedLinearModelResult()
    generalizedLinearModelResult.setName('generalizedLinearModelResult')
    myStudy.add('generalizedLinearModelResult', generalizedLinearModelResult)

    # print 'Study = ' , myStudy
    myStudy.save()

    # Create a new Study Object
    myStudy = ot.Study()
    myStudy.setStorageManager(ot.XMLStorageManager(fileName))

    myStudy.load()
    # print 'loaded Study = ' , myStudy

    # Create a NumericalPoint from the one stored in the Study
    point = ot.NumericalPoint()
    myStudy.fillObject('point', point)

    print('point = ', repr(point))

    # Create a NumericalSample from the one stored in the Study
    sample = ot.NumericalSample()
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

    # Create an analytical NumericalMathFunction from the one stored in the
    # Study
    analytical = ot.NumericalMathFunction()
    myStudy.fillObject('analytical', analytical)

    print('analytical = ', analytical)

    # Create a GeneralizedLinearModelResult from the one stored in the Study
    generalizedLinearModelResult = ot.GeneralizedLinearModelResult()
    myStudy.fillObject('generalizedLinearModelResult',
                       generalizedLinearModelResult)

    print('generalizedLinearModelResult = ', generalizedLinearModelResult)

    # cleanup
    os.remove(fileName)

except:
    import os
    import traceback
    traceback.print_exc()
    os._exit(1)
