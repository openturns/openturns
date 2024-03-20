"""
Create a parametric function
============================
"""
# %%
# In this example, we show how to use the :class:`~openturns.ParametricFunction` class.
# This is a tool which is very convenient when we perform
# calibration, e.g. with :class:`~openturns.NonLinearLeastSquaresCalibration`
# or :class:`~openturns.RandomWalkMetropolisHastings`.

# %%
# In this example we create a parametric function:
#
# .. math::
#    d_{L,I}(E, F):  \Rset^2  \rightarrow \Rset
#
# function from an existing "full" function:
#
# .. math::
#  d(E, F, L, I):  \Rset^4  \rightarrow \Rset.
#

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)


# %%
# Define the function
# ~~~~~~~~~~~~~~~~~~~


# %%
# Create the function with all parameters d(E, F, L, I)
def d_func(X):
    E, F, L, II = X
    d = -F * L**3 / (3.0 * E * II)
    return [d]


beam = ot.PythonFunction(4, 1, d_func)
beam.setInputDescription(["E", "F", "L", "I"])

# %%
# Evaluate d
x = [50.0, 1.0, 10.0, 5.0]
beam(x)

# %%
# In the physical model, the inputs and parameters are ordered as
# presented in the next table.
# Notice that there are no parameters in the physical model.
#
# +-------+----------------+
# | Index | Input variable |
# +=======+================+
# | 0     | E              |
# +-------+----------------+
# | 1     | F              |
# +-------+----------------+
# | 2     | L              |
# +-------+----------------+
# | 3     | I              |
# +-------+----------------+
#
# +-------+-----------+
# | Index | Parameter |
# +=======+===========+
# | ∅     | ∅         |
# +-------+-----------+
#
# **Table 1.** Indices and names of the inputs and parameters of the physical model.
#

# %%
# The next cell presents the description of the input variables
# and the description of the parameters of the physical model.
# We see that there is no parameter at this stage in this function.
print("Physical Model Inputs:", beam.getInputDescription())
print("Physical Model Parameters:", beam.getParameterDescription())


# %%
# Define the parametric function
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# %%
# We create a :class:`~openturns.ParametricFunction`
# where the parameters are `L` and `I` which have the indices 2 and
# and 3 in the physical model.
#
# +-------+----------------+
# | Index | Input variable |
# +=======+================+
# | 0     | E              |
# +-------+----------------+
# | 1     | F              |
# +-------+----------------+
#
# +-------+-----------+
# | Index | Parameter |
# +=======+===========+
# | 0     | L         |
# +-------+-----------+
# | 1     | I         |
# +-------+-----------+
#
# **Table 2.** Indices and names of the inputs and parameters of the parametric model.
#

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
# The next cell presents the description of the input variables
# and the description of the parameters of the parametric function.
# We see that the parametric function has 2 parameters: L and I.
print("Physical Model Inputs:", beam_LI.getInputDescription())
print("Physical Model Parameters:", beam_LI.getParameterDescription())

# %%
# Evaluate d on (E,F) with fixed parameters (L,I)
beam_LI([50.0, 1.0])
