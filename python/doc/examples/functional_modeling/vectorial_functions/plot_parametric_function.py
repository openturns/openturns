"""
Create a parametric function
============================
"""
# %%
# In this example we are going to create a parametric function:
#
# .. math::
#    d_{L,I}(E, F):  \mathbb{R}^2  \rightarrow \mathbb{R}
#
# function from an existing "full" function:
#
# .. math::
#  d(E, F, L, I):  \mathbb{R}^4  \rightarrow \mathbb{R}
#

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)


# %%
# Create the function with all parameters d(E, F, L, I)
def d_func(X):
    E, F, L, II = X
    d = -F * L**3 / (3.0 * E * II)
    return [d]


beam = ot.PythonFunction(4, 1, d_func)

# %%
# Evaluate d
x = [50.0, 1.0, 10.0, 5.0]
beam(x)

# %%
# Create the indices of the frozen parameters (L,I) from the full parameter list
indices = [2, 3]

# %%
# Create the values of the frozen parameters (L,I)
referencePoint = [10.0, 5.0]

# %%
# Create the parametric function
beam_LI = ot.ParametricFunction(beam, indices, referencePoint)

# %%
# Evaluate d on (E,F) with fixed parameters (L,I)
beam_LI([50.0, 1.0])
