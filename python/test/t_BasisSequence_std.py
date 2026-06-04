#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

b = ot.Basis()
seq = ot.BasisSequence(b)
print("sequence =", seq)
seq.add(ot.Indices())
b1 = seq.getBasis(0)
print(f"{b1=}")
dim = seq.getDimension()
print(f"{dim=}")
indices = seq.getIndices(0)
print(f"{indices=}")
masterB = seq.getMasterBasis()
print(f"{masterB=}")
size = seq.getSize()
print(f"{size=}")
