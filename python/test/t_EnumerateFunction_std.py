#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# LinearEnumerateFunction
assert ot.LinearEnumerateFunction(3) == ot.LinearEnumerateFunction(3), "LEF==LEF"
assert ot.LinearEnumerateFunction(3) != ot.LinearEnumerateFunction(4), "LEF!=LEF dim"

# HyperbolicAnisotropicEnumerateFunction
assert ot.HyperbolicAnisotropicEnumerateFunction([1.0] * 3, 0.5) == ot.HyperbolicAnisotropicEnumerateFunction([1.0] * 3, 0.5), "HAEF==HAEF"
assert ot.HyperbolicAnisotropicEnumerateFunction([1.0] * 3, 0.5) != ot.HyperbolicAnisotropicEnumerateFunction([1.0, 2.0, 3.0], 0.5), "HAEF!=HAEF weight"
assert ot.HyperbolicAnisotropicEnumerateFunction([1.0] * 3, 0.5) != ot.HyperbolicAnisotropicEnumerateFunction([1.0] * 3, 0.8), "HAEF!=HAEF q"

# NormInfEnumerateFunction
assert ot.NormInfEnumerateFunction(3) == ot.NormInfEnumerateFunction(3), "NIEF==NIEF"
assert ot.NormInfEnumerateFunction(3) != ot.NormInfEnumerateFunction(4), "NIEF!=NIEF dim"

# Cross-type inequality
assert ot.LinearEnumerateFunction(3) != ot.HyperbolicAnisotropicEnumerateFunction([1.0] * 3, 0.5), "LEF!=HAEF"

# Handle equality
e1 = ot.EnumerateFunction(ot.LinearEnumerateFunction(3))
e2 = ot.EnumerateFunction(ot.LinearEnumerateFunction(3))
assert e1 == e2, "EF handle =="
assert e1 != ot.EnumerateFunction(), "EF handle !="

# Self equality
ef = ot.LinearEnumerateFunction(3)
assert ef == ef, "self =="
assert not ef != ef, "self !="
