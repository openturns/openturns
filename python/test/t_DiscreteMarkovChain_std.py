#!/usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

process = ot.DiscreteMarkovChain()
print("Default constructor : process = ")
print(process)

process = ot.DiscreteMarkovChain(0,ot.SquareMatrix([[1.0, 0.0],[0.0, 1.0]]))
print("Constructor from int and SquareMatrix: process = ")
print(process)

transitionMatrix = ot.SquareMatrix([[0.0, 0.5, 0.5], [0.7, 0.0, 0.3], [0.8, 0.0, 0.2]])
print("transition matrix =")
print(transitionMatrix)

origin = 1
print("origin =")
print(origin)

process.setTransitionMatrix(transitionMatrix)
print("Transition matrix accessor : process = ")
print(process)

process.setOrigin(origin)
print("Origin accessor : process = ")
print(process)

process.setTimeGrid(ot.RegularGrid(0, 1, 20))
print("Time grid accessor : process = ")
print(process)

real = process.getRealization()
print("Realization :")
print(real)

future = process.getFuture(20)
print("One future :")
print(future)

futures = process.getFuture(20, 3)
print("3 different futures :")
print(futures)
 
