"""
Create a composed function
==========================
"""

# %%
# In this example we are going to create a composed function :math:`f\circ g`
#

# %%
import openturns as ot


# %%
# assume f, g functions
g = ot.SymbolicFunction(["x1", "x2"], ["x1 + x2", "3 * x1 * x2"])
f = ot.SymbolicFunction(["x1", "x2"], ["2 * x1 - x2"])

# %%
# create the composed function
function = ot.ComposedFunction(f, g)

# %%
# evaluate the function
x = [3.0, 4.0]
y = function(x)
print("x=", x, "y=", y)
