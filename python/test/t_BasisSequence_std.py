#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

b = ot.Basis()
seq = ot.BasisSequence(b)
print("sequence =", seq)
