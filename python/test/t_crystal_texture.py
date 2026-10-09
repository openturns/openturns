#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
from openturns.usecases import crystal_texture

ot.TESTPREAMBLE()

use_case = crystal_texture.CrystalTexture()
ti = use_case.ti6al4v
protein = use_case.protein
print(ti.getSize(), ti.getDimension(), protein.getSize(), protein.getDimension())
assert ti.getSize() == 300
assert protein.getSize() == 200

# All the axes are unit vectors (antipodal symmetry: x and -x are equivalent)
for data in [ti, protein]:
    for i in range(data.getSize()):
        ott.assert_almost_equal(data[i].norm(), 1.0, 1e-12, 0.0)

# Bingham factory recovers the rolling texture concentrations
estimated_ti = otexp.BinghamFactory().build(ti).getImplementation().getZeta()
ref_ti = use_case.getReferenceTi6Al4VZeta()
for estimated_value, reference_value in zip(estimated_ti, ref_ti):
    ott.assert_almost_equal(estimated_value, reference_value, 0.0, 1.5)

# ... and the weaker protein preferred orientation
estimated_protein = otexp.BinghamFactory().build(protein).getImplementation().getZeta()
ref_protein = use_case.getReferenceProteinZeta()
for estimated_value, reference_value in zip(estimated_protein, ref_protein):
    ott.assert_almost_equal(estimated_value, reference_value, 0.0, 1.5)
