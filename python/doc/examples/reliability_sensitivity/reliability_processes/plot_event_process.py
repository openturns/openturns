"""
Create an event based on a process
==================================
"""
# %%
# This example gives elements to create an event based on a multivariate stochastic process.
# Let :math:`X: \Omega \times \mathcal{D} \rightarrow \mathbb{R}^d` be a stochastic process
# of dimension :math:`d`, where :math:`\mathcal{D} \in \mathbb{R}^n` is discretized on the mesh
# :math:`\mathcal{M}`. We suppose that :math:`\mathcal{M}` contains :math:`N` vertices.
#
# We define the event :math:`\mathcal{E}` as:
#
# .. math::
#    \begin{aligned} 
#      \displaystyle \mathcal{E}(X) = \bigcup_{\underline{t}\in \mathcal{M}}\left\{X_{\underline{t}} \in \mathcal{A} \right\} 
#    \end{aligned}
#   
# where :math:`\mathcal{A}` is a domain of :math:`\mathbb{R}^d`.
#
# A particular domain :math:`\mathcal{A}` is the cartesian product of type:
#
# .. math::
#    \begin{aligned} 
#      \mathcal{A} = \prod_{i=1}^d [a_i,b_i]
#    \end{aligned}
#
# In that case, the library defines :math:`\mathcal{A}` by its both extreme points : :math:`\underline{a}` and :math:`\underline{b}`.
#
# In the general case, :math:`\mathcal{A}` is a *Domain* object that is able to
# check if it contains a given point in :math:`\mathbb{R}^d`.
#
# The library creates an *Event* object from the process :math:`X` and the
# domain :math:`\mathcal{A}`. Then, it is possible to get a realization of the event
# :math:`\mathcal{E}`, which is scalar :math:`1_{\mathcal{E}(X)}(\underline{x}_0, \dots, \underline{x}_{N-1})`
# if :math:`(\underline{x}_0, \dots,\underline{x}_{N-1})` is a realization of :math:`X` on
# :math:`\mathcal{M}`.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# Define a 2-d mesh
indices = [40, 20]
mesher = ot.IntervalMesher(indices)
lowerBound = [0., 0.]
upperBound = [2., 1.]
interval = ot.Interval(lowerBound, upperBound)
mesh = mesher.build(interval)

# %%
# Create the covariance model
amplitude = [1.0, 2.0, 3.0]
scale = [4.0, 5.0]
spatialCorrelation = ot.CorrelationMatrix(3)
spatialCorrelation[0, 1] = 0.8
spatialCorrelation[0, 2] = 0.6
spatialCorrelation[1, 2] = 0.1
covmodel = ot.ExponentialModel(scale, amplitude, spatialCorrelation)

# Create a normal process
process = ot.GaussianProcess(covmodel, mesh)

# %%
# Create a domain A in R^3: [0.8; 1.2]*[1.5; 1.6]*[0.5; 0.7]
lowerBound = [0.8, 1.5, 0.5]
upperBound = [1.2, 1.6, 0.7]
domain = ot.Interval(lowerBound, upperBound)

# Create the event
event = ot.ProcessEvent(process, domain)
