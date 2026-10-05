#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
from openturns.usecases import geometric_tolerancing

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

use_case = geometric_tolerancing.GeometricTolerancing()
print(use_case.plate.getSimplicesNumber())
assert use_case.plate.getSimplicesNumber() == 32

# The defect density is positive at the plate center, zero far away
center_pdf = use_case.defect.computePDF([0.25, 0.25, 0.05])
print("center pdf=", center_pdf)
assert center_pdf > 0.0
ott.assert_almost_equal(use_case.defect.computePDF([5.0, 5.0, 5.0]), 0.0, 0.0, 1e-12)

# Realizations lie on the deformed plate: |z| <= 0.05
realization = use_case.defect.getRealization()
assert abs(realization[2]) <= 0.05 + 1e-10

# The non-conformity probability is strictly inside (0, 1)
probability = use_case.nonConformityProbability(20000)
print("non-conformity probability=", probability)
assert 0.05 < probability < 0.6
