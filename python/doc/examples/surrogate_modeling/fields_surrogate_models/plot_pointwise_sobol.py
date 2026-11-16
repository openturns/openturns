"""
Pointwise sensitivity of a field metamodel
========================================
"""

# %%
# A single integrated sensitivity index can hide a change in the important input
# along a field. We illustrate this with two independent standard normal inputs
# and the scalar field :math:`Y(t)=(1-t)X_1+tX_2`, for :math:`0\leq t\leq 1`.
# The first input controls the beginning of the trajectory and the second
# controls the end. There are no interactions, so first and total order indices
# coincide. The analytical first order indices are
# :math:`S_1(t)=(1-t)^2/((1-t)^2+t^2)` and
# :math:`S_2(t)=t^2/((1-t)^2+t^2)`.

# %%
import openturns as ot
import openturns.viewer as otv

# %%
ot.RandomGenerator.SetSeed(42)
mesh = ot.RegularGrid(0.0, 0.025, 41)
distribution = ot.Normal(2)
distribution.setDescription(["X1", "X2"])
model = ot.VertexValuePointToFieldFunction(
    ot.SymbolicFunction(["t", "x1", "x2"], ["(1-t)*x1+t*x2"]), mesh
)
input_sample = distribution.getSample(200)
output_sample = model(input_sample)

# %%
# Fit one KL-PCE metamodel, not an independent surrogate at each vertex.
algorithm = ot.PointToFieldFunctionalChaosAlgorithm(
    input_sample, output_sample, distribution
)
algorithm.setThreshold(1e-12)
algorithm.run()
sensitivity = ot.FieldFunctionalChaosSobolIndices(algorithm.getResult())
first = sensitivity.getPointwiseFirstOrderIndices()
total = sensitivity.getPointwiseTotalOrderIndices()

# %%
# Each returned field has one component per input, on the output mesh.
graph = ot.Graph("Pointwise first order sensitivity", "t", "Sobol index", True, "right")
for j in range(distribution.getDimension()):
    curve = ot.Curve(mesh.getVertices(), first.getValues().getMarginal(j))
    curve.setLegend(distribution.getDescription()[j])
    graph.add(curve)
view = otv.View(graph)

# %%
# The coefficients of each chaos basis function are reconstructed across all
# KL modes before they are squared. This retains the cross-mode contributions
# to pointwise variance. The indices describe the fitted surrogate: accuracy
# for the original model still depends on the KL truncation and PCE fit.
# This API supports independent vector inputs, a componentwise transformation
# to the polynomial basis, and one scalar output KL block. Exact zero-variance
# vertices return NaN, since a Sobol ratio is undefined there. Very small
# positive variances can make these ratios numerically unstable.
otv.View.ShowAll()
