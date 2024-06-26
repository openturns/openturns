"""
Assemble copulas
================
"""
# %%
# In this example we are going to merge a collection of independent copulas into one.
#

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)

# %%
# Create a collection of copulas
R = ot.CorrelationMatrix(3)
R[0, 1] = 0.5
R[0, 2] = 0.25
collection = [ot.FrankCopula(3.0), ot.NormalCopula(R), ot.ClaytonCopula(2.0)]

# %%
# Merge the copulas
copula = ot.BlockIndependentCopula(collection)
print(copula)
