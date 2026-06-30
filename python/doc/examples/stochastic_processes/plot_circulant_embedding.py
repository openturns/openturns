"""
Circulant embedding Gaussian process
====================================
"""

# %%
# The :class:`~openturns.experimental.CirculantEmbeddingGaussianProcess` class
# implements the fast and exact simulation of stationary Gaussian processes on
# regular grids using the circulant embedding method, also known as the
# Davies-Harte algorithm.
#
# Instead of discretizing and factorizing the full covariance matrix, the
# Toeplitz covariance matrix is embedded into a circulant matrix which is
# diagonalized by a fast Fourier transform in :math:`\cO(N \log N)` operations,
# where :math:`N` is the number of vertices.
#
# The process is defined on a :class:`~openturns.Interval` discretized by a
# regular grid built with an :class:`~openturns.IntervalMesher`. Both scalar and
# vector-valued output covariance models are supported.

# %%
from openturns.viewer import View
import openturns as ot
import openturns.experimental as otexp

ot.RandomGenerator.SetSeed(0)

# %%
# One-dimensional process with scalar output
# ------------------------------------------
# We first define a stationary covariance model, an interval and the number of
# cells per dimension.

covarianceModel = ot.ExponentialModel([1.0], [1.0])
interval = ot.Interval(0.0, 20.0)
n = 200
process = otexp.CirculantEmbeddingGaussianProcess(covarianceModel, interval, [n])
print("circular size = ", process.getCircularSize())

# %%
# A single realization is a :class:`~openturns.Field` on the regular grid.

field = process.getRealization()
graph = field.drawMarginal(0)
graph.setTitle("One realization of a CE Gaussian process")
view = View(graph)

# %%
# We can draw several realizations at once with
# :meth:`~openturns.ProcessSample.drawMarginal`.

sample = process.getSample(5)
graph = sample.drawMarginal(0)
graph.setTitle("5 realizations of a CE Gaussian process")
view = View(graph)

# %%
# Two-dimensional process with scalar output
# ------------------------------------------
# The algorithm extends to any dimension :math:`d` by embedding the covariance
# matrix into a block-circulant matrix of size :math:`N_1 \cdots N_d`.

covarianceModel2D = ot.MaternModel([1.0, 1.0], [1.0], 2.5)
interval2D = ot.Interval([0.0, 0.0], [4.0, 4.0])
process2D = otexp.CirculantEmbeddingGaussianProcess(covarianceModel2D, interval2D, [40, 40])
print("circular size = ", process2D.getCircularSize())

field2D = process2D.getRealization()
graph = field2D.drawMarginal(0, True)
graph.setTitle("One realization of a 2D CE Gaussian process")
view = View(graph)

# %%
# Two-dimensional process with vector-valued output
# -------------------------------------------------
# Vector-valued covariance models lead to correlated outputs at each vertex.

covarianceModelVec = ot.ExponentialModel([1.0, 1.0], [1.0, 0.7])
processVec = otexp.CirculantEmbeddingGaussianProcess(covarianceModelVec,
                                                     ot.Interval([0.0] * 2, [3.0] * 2),
                                                     [24, 24])
fieldVec = processVec.getRealization()
graph0 = fieldVec.drawMarginal(0, True)
graph0.setTitle("Marginal 0 of a 2D vector CE process")
graph1 = fieldVec.drawMarginal(1, True)
graph1.setTitle("Marginal 1 of a 2D vector CE process")
grid = ot.GridLayout(1, 2)
grid.setGraph(0, 0, graph0)
grid.setGraph(0, 1, graph1)
view = View(grid)

# %%
# The realizations produced by the circulant embedding method are statistically
# equivalent to those produced by a :class:`~openturns.GaussianProcess` with the
# same covariance model on the same mesh, but they are generated in near linear
# time thanks to the FFT.
View.ShowAll()
