#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

# Time grid creation and White Noise
Tmin = 0.0
deltaT = 0.1
steps = 11

# Initialization of the TimeGrid timeGrid
timeGrid = ot.RegularGrid(Tmin, deltaT, steps)

# Creation of the Antecedent
myARMAProcess = ot.ARMA()
myARMAProcess.setTimeGrid(timeGrid)
print('myAntecedentProcess = ',  myARMAProcess)

# Creation of a function
f = ot.NumericalMathFunction(['x'], ['2 * x + 5.0'])

# We build a dynamical function
myDynamicalFunction = ot.DynamicalFunction(ot.SpatialFunction(f))

# finally we get the compsite process
myCompositeProcess = ot.CompositeProcess(myDynamicalFunction, myARMAProcess)
print('myCompositeProcess = ',  repr(myCompositeProcess))

# Test realization
print('One realization= ')
print(myCompositeProcess.getRealization())


##########################################
# Create a spatial  dynamical function
# Create the function g : R^2 --> R^2
#               (x1,x2) --> (x1^2, x1+x2)
g = ot.NumericalMathFunction(['x1', 'x2'],  ['x1^2', 'x1+x2'])

# Convert g : R --> R into a spatial fucntion
# n is the dimension of the mesh
# of the field on wich g will be applied
nSpat = 2
myDynFunc = ot.SpatialFunction(g, nSpat)

# Then g acts on processes X: Omega * R^nSpat --> R^2


##########################################
# Create a trend function fTrend: R^n --> R^q
# for example for  myXtProcess of dimension 2
# defined on a bidimensional mesh
# fTrend : R^2 --> R^2
#          (t1, t2) --> (1+2t1, 1+3t2)
fTrend = ot.NumericalMathFunction(['t1', 't2'], ['1+2*t1', '1+3*t2'])


##########################################
# Create a normal process of dimension 2
# which mesh is of box of dimension 2

myIndices = ot.Indices([80, 40])
myMesher = ot.IntervalMesher(myIndices)
lowerBound = [0., 0.]
upperBound = [2., 1.]
myInterval = ot.Interval(lowerBound, upperBound)
myMesh = myMesher.build(myInterval)

# Define a bidimensional temporal normal process on the mesh
# with independent components
amplitude = [1.0, 1.0]
scale = [0.2, 0.3]
myCovModel = ot.ExponentialModel(scale, amplitude)

myXtProcess_temp = ot.GaussianProcess(myCovModel, myMesh)

# Non linear transformation of myXtProcess
# to get a positive process
g2 = ot.NumericalMathFunction(['x1', 'x2'],  ['x1^2', 'abs(x2)'])
myDynTransform = ot.SpatialFunction(g2, 2)
myXtProcess = ot.CompositeProcess(myDynTransform, myXtProcess_temp)

# Create the  image Y
myYtProcess = ot.CompositeProcess(myDynFunc, myXtProcess)

# Get the antecedent : myXtProcess
print('My antecedent process = ', myYtProcess.getAntecedent())

# Get the dynamical function
# which performs the transformation
print('My dynamical function = ', myYtProcess.getFunction())
