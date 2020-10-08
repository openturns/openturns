"""
Sobol' sensitivity indices from chaos
=====================================
"""
# %%
# In this example we are going to compute global sensitivity indices from a functional chaos decomposition.
#
# We study the Borehole function that models water flow through a borehole:
#
# .. math::
#    \frac{2 \pi T_u (H_u - H_l)}{\ln{r/r_w}(1+\frac{2 L T_u}{\ln(r/r_w) r^2_w K_w}\frac{T_u}{T_l})}
#   
# With parameters:
#  
# - :math:`r_w`: radius of borehole (m)
# - :math:`r`: radius of influence (m)
# - :math:`T_u`: transmissivity of upper aquifer (:math:`m^2/yr`)
# - :math:`H_u`: potentiometric head of upper aquifer (m)
# - :math:`T_l`: transmissivity of lower aquifer (:math:`m^2/yr`)
# - :math:`H_l`: potentiometric head of lower aquifer (m)
# - :math:`L`: length of borehole (m)
# - :math:`K_w`: hydraulic conductivity of borehole (:math:`m/yr`)

# %%
from __future__ import print_function
import openturns as ot
from operator import itemgetter 
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# borehole model
dimension = 8
input_names = ['rw', 'r', 'Tu', 'Hu', 'Tl', 'Hl', 'L', 'Kw']
model = ot.SymbolicFunction(input_names,
                            ['(2*pi_*Tu*(Hu-Hl))/(ln(r/rw)*(1+(2*L*Tu)/(ln(r/rw)*rw^2*Kw)+Tu/Tl))'])
coll = [ot.Normal(0.1, 0.0161812),
         ot.LogNormal(7.71, 1.0056),
         ot.Uniform(63070.0, 115600.0),
         ot.Uniform(990.0, 1110.0),
         ot.Uniform(63.1, 116.0),
         ot.Uniform(700.0, 820.0),
         ot.Uniform(1120.0, 1680.0),
         ot.Uniform(9855.0, 12045.0)]
distribution = ot.ComposedDistribution(coll)
distribution.setDescription(input_names)

# %%
# Freeze r, Tu, Tl from model to go faster
selection = [1,2,4]
complement = ot.Indices(selection).complement(dimension)
distribution = distribution.getMarginal(complement)
model = ot.ParametricFunction(model, selection, distribution.getMarginal(selection).getMean())
input_names_copy = list(input_names)
input_names = itemgetter(*complement)(input_names)
dimension = len(complement)

# %%
# design of experiment
size = 1000
X = distribution.getSample(size)
Y = model(X)

# %%
# create a functional chaos model
algo = ot.FunctionalChaosAlgorithm(X, Y)
algo.run()
result = algo.getResult()
print(result.getResiduals())
print(result.getRelativeErrors())

# %%
# Quick summary of sensitivity analysis
sensitivityAnalysis = ot.FunctionalChaosSobolIndices(result)
print(sensitivityAnalysis.summary())

# %%
# draw Sobol' indices
first_order = [sensitivityAnalysis.getSobolIndex(i) for i in range(dimension)]
total_order = [sensitivityAnalysis.getSobolTotalIndex(i) for i in range(dimension)]
graph = ot.SobolIndicesAlgorithm.DrawSobolIndices(input_names, first_order, total_order)
view = viewer.View(graph)

# %%
# We saw that total order indices are close to first order,
# so the higher order indices must be all quite close to 0
for i in range(dimension):
    for j in range(i):
        print(input_names[i] + ' & '+ input_names[j], ":", sensitivityAnalysis.getSobolIndex([i, j]))

plt.show()
