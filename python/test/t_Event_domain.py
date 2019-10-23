#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

dim = 2
distribution = ot.Normal(dim)
#
# Case 1: composite random vector based event
#

# The input vector
X = ot.RandomVector(distribution)
# The model: the identity function
inVars = ot.Description(dim)
for i in range(dim):
    inVars[i] = "x" + str(i)
model = ot.SymbolicFunction(inVars, inVars)
# The output vector
Y = ot.CompositeRandomVector(model, X)
# The domain: [0, 1]^dim
domain = ot.Interval(dim)
# The event
event = ot.DomainEvent(Y, domain)

print("sample=", event.getSample(10))

#
# Case 2: process based event
#

# The input process
X = ot.WhiteNoise(distribution)
# The domain: [0, 1]^dim
domain = ot.Interval(dim)
# The event
event = ot.ProcessEvent(X, domain)

print("sample=", event.getSample(10))

# 3. from distribution
antecedent = ot.RandomVector(ot.Normal(2))
domain = ot.LevelSet(ot.SymbolicFunction(
    ['x', 'y'], ['x^2+y^2']), ot.Less(), 1.0)
event = ot.DomainEvent(antecedent, domain)
print('sample=', event.getSample(10))
