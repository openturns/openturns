"""
Assemble copulas
================
"""
# %%
# In this example we are going to merge a collection of independent copulas into one.
#

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# create a collection of copulas
R = ot.CorrelationMatrix(3)
R[0, 1] = 0.5
R[0, 2] = 0.25
collection = [ot.FrankCopula(3.0), ot.NormalCopula(R), ot.ClaytonCopula(2.0)]

# %%
# merge the copulas
copula = ot.ComposedCopula(collection)
print(copula)
