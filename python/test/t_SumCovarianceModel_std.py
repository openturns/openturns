#! /usr/bin/env python

import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Nominal 1D sum: operators return a base handle on the summed model,
# explicit construction gives access to the Sum-specific API
m1 = ot.ExponentialModel([1.0], [2.0])
m2 = ot.MaternModel([3.0], [4.0], 1.5)
total = m1 + m2
explicit = otexp.SumCovarianceModel([m1, m2])
assert len(explicit.getCollection()) == 2, "wrong collection size"
ott.assert_almost_equal(total.getFullParameter(), explicit.getFullParameter())
assert total.getInputDimension() == 1, "wrong input dim"
assert total.getOutputDimension() == 1, "wrong output dim"
s = [0.5]
t = [1.5]
ott.assert_almost_equal(
    total.computeAsScalar(s, t),
    m1.computeAsScalar(s, t) + m2.computeAsScalar(s, t),
)
ott.assert_almost_equal(total(s, t)[0, 0], total.computeAsScalar(s, t))
ott.assert_almost_equal(total.computeAsScalar(0.5, 1.5), total.computeAsScalar(s, t))
tau = [0.25]
ott.assert_almost_equal(
    total.computeAsScalar(tau),
    m1.computeAsScalar(tau) + m2.computeAsScalar(tau),
)
assert total.isStationary(), "sum of stationary models must be stationary"
assert total.isDiagonal(), "sum of diagonal models must be diagonal"

# Collection constructor keeps members, single member allowed for persistence
coll = explicit.getCollection()
assert len(coll) == 2, "wrong collection size"
single = otexp.SumCovarianceModel([m1])
ott.assert_almost_equal(single.computeAsScalar(s, t), m1.computeAsScalar(s, t))

# Flattening of nested sums, as LinearCombinationFunction
m3 = ot.ExponentialModel([2.0], [1.0])
nested = total + m3
nested_desc = nested.getFullParameterDescription()
assert any(d.startswith("model_2_") for d in nested_desc), "sums must flatten"
assert not any("model_0_model_" in d for d in nested_desc), "sums must flatten"

# Folding of identical atoms into a scaled model
double = m1 + m1
assert double.getFullParameter().getSize() == m1.getFullParameter().getSize() + 1
fp_double = double.getFullParameterDescription()
assert fp_double[fp_double.getSize() - 1] == "factor", "folded sum must carry a factor"
ott.assert_almost_equal(
    double.getFullParameter()[double.getFullParameter().getSize() - 1], 2.0
)
ott.assert_almost_equal(
    double.computeAsScalar(s, t), 2.0 * m1.computeAsScalar(s, t)
)

# Handle/implementation operator combinations: concrete proxies already
# derive from the implementation proxy, so m1 + m2 is the impl+impl case
h1 = ot.CovarianceModel(m1)
h2 = ot.CovarianceModel(m2)
ott.assert_almost_equal((h1 + h2).computeAsScalar(s, t), total.computeAsScalar(s, t))
ott.assert_almost_equal((h1 + m2).computeAsScalar(s, t), total.computeAsScalar(s, t))
ott.assert_almost_equal((m1 + h2).computeAsScalar(s, t), total.computeAsScalar(s, t))

# Folding unwraps all nesting levels on both sides: (m1*2)*3 + m1 -> 7*m1
nested_atom = (m1 * 2.0) * 3.0
refolded = nested_atom + m1
refolded_desc = refolded.getFullParameterDescription()
assert refolded_desc[refolded_desc.getSize() - 1] == "factor"
ott.assert_almost_equal(
    [refolded.getFullParameter()[refolded.getFullParameter().getSize() - 1]], [7.0]
)
ott.assert_almost_equal(
    refolded.computeAsScalar(s, t), 7.0 * m1.computeAsScalar(s, t)
)

# Dimension mismatch is rejected
bad_input = ot.ExponentialModel([1.0, 2.0], [2.0])
with ott.assert_raises(TypeError):
    bad_input + m1
with ott.assert_raises(TypeError):
    otexp.SumCovarianceModel([])

# Multivariate output: matrix evaluation is the member sum
d1 = ot.DiracCovarianceModel(2, [1.0, 2.0])
d2 = ot.DiracCovarianceModel(2, [3.0, 1.0])
dsum = d1 + d2
assert dsum.getOutputDimension() == 2, "wrong multivariate output dim"
p = [0.0, 0.0]
q = [1.0, 2.0]
ott.assert_almost_equal(dsum(p, q), d1(p, q) + d2(p, q))
with ott.assert_raises(TypeError):
    dsum.computeAsScalar(p, q)
with ott.assert_raises(TypeError):
    m1 + d1

# No unique scale/amplitude/correlation/nugget at the sum level
for accessor in [
    total.getScale,
    total.getAmplitude,
    total.getOutputCorrelation,
    total.getNuggetFactor,
]:
    with ott.assert_raises(RuntimeError):
        accessor()
with ott.assert_raises(RuntimeError):
    total.setScale([1.0])
with ott.assert_raises(RuntimeError):
    total.setAmplitude([1.0])
with ott.assert_raises(RuntimeError):
    total.setOutputCorrelation(ot.CorrelationMatrix(1))
with ott.assert_raises(RuntimeError):
    total.setNuggetFactor(0.0)
assert total != m1, "sum must compare unequal to a member"

# Full parameter is the concatenation of member full parameters
fp = total.getFullParameter()
assert fp.getSize() == m1.getFullParameter().getSize() + m2.getFullParameter().getSize()
desc = total.getFullParameterDescription()
assert desc[0].startswith("model_0_"), "descriptions must be prefixed by member index"
assert any(d.startswith("model_1_") for d in desc), "missing member 1 prefix"
offset = m1.getFullParameter().getSize()
mutated = otexp.SumCovarianceModel([m1, m2])
new_fp = mutated.getFullParameter()
new_fp[0] = 7.0
mutated.setFullParameter(new_fp)
m1b = ot.ExponentialModel([7.0], [2.0])
ott.assert_almost_equal(
    mutated.computeAsScalar(s, t),
    (m1b + m2).computeAsScalar(s, t),
)
with ott.assert_raises(TypeError):
    mutated.setFullParameter([1.0])

# Active set is the shifted union of member active sets
expected_active = list(m1.getActiveParameter()) + [
    offset + i for i in m2.getActiveParameter()
]
assert list(total.getActiveParameter()) == expected_active, "wrong default active set"
gated = otexp.SumCovarianceModel([m1, m2])
gated.setActiveParameter([0, offset])
assert list(gated.getCollection()[0].getActiveParameter()) == [0]
assert list(gated.getCollection()[1].getActiveParameter()) == [0]
with ott.assert_raises(TypeError):
    gated.setActiveParameter([fp.getSize()])
with ott.assert_raises(TypeError):
    gated.setActiveParameter([0, 0])

# Gradients follow the sum rule
ott.assert_almost_equal(
    total.partialGradient(s, t), m1.partialGradient(s, t) + m2.partialGradient(s, t)
)
pg = total.parameterGradient(s, t)
off = m1.getFullParameter().getSize()
m1_active = list(m1.getActiveParameter())
m2_active = list(m2.getActiveParameter())
m1_pg = m1.parameterGradient(s, t)
m2_pg = m2.parameterGradient(s, t)
actives = list(total.getActiveParameter())
expected = ot.Matrix(len(actives), pg.getNbColumns())
for r in range(len(actives)):
    g = actives[r]
    src = m1_pg if g < off else m2_pg
    local_list = m1_active if g < off else m2_active
    lrow = local_list.index(g if g < off else g - off)
    for c in range(pg.getNbColumns()):
        expected[r, c] = src[lrow, c]
ott.assert_almost_equal(pg, expected)
# Restricted actives reorder the rows accordingly
gated_pg = gated.parameterGradient(s, t)
assert gated_pg.getNbRows() == 2, "wrong restricted gradient rows"
ott.assert_almost_equal(
    [[gated_pg[0, 0]], [gated_pg[1, 0]]],
    [[m1_pg[0, 0]], [m2_pg[0, 0]]],
)

# Discretization matches the member sum, Dirac term adds scaled identity
grid = ot.RegularGrid(0.0, 0.5, 4)
smooth = m1 + m2
dirac = ot.DiracCovarianceModel(1)
dirac.setAmplitude([0.5])
dirac.setNuggetFactor(0.0)
noisy = smooth + dirac
expected = smooth.discretize(grid)
for k in range(4):
    expected[k, k] += 0.25
ott.assert_almost_equal(noisy.discretize(grid), expected)

# Marginals sum member marginals
marg = dsum.getMarginal(1)
ott.assert_almost_equal(marg(p, q), d1.getMarginal(1)(p, q) + d2.getMarginal(1)(p, q))
margs = dsum.getMarginal([0, 1])
ott.assert_almost_equal(margs(p, q), dsum(p, q))
assert explicit.isParallel(), "sum of parallel members must be parallel"
assert "SumCovarianceModel" in str(total), "wrong __str__"

# Equality and persistence round-trip
assert total == total, "reflexive equality failed"
assert not total == m1, "sum must differ from a member"
study = ot.Study()
fname = "study_sum_covariance.xml"
study.setStorageManager(ot.XMLStorageManager(fname))
study.add("total", total)
study.save()
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(fname))
study.load()
reloaded = otexp.SumCovarianceModel()
study.fillObject("total", reloaded)
assert reloaded == total, "persistence round-trip failed"
os.remove(fname)
