"""
Create an ARMA process
======================
"""
# %%
# In this example we are going to build an ARMA process defined by its linear recurrence coefficients.
#
# The creation of an ARMA model requires the data of the AR and MA
# coefficients which are:
#
# -  a list of scalars in the unidmensional case :
#    :math:`(a_1, \dots, a_p)` for the AR-coefficients and
#    :math:`(b_1, \dots, b_q)` for the MA-coefficients
#
# -  a list of square matrix
#    :math:`(\underline{\underline{A}}_{\, 1}, \dots, \underline{\underline{A}}{\, _p})` for the
#    AR-coefficients and
#    :math:`(\underline{\underline{B}}_{\, 1}\, \dots, \underline{\underline{B}}_{\, q})` for the
#    MA-coefficients
#
# Il also requires the definition of a white noise
# :math:`\underline{\varepsilon}` that contains the same time grid as the
# one of the process.
# The current state of an ARMA model is characterized by its last
# :math:`p` values and the last :math:`q` values of its white noise. It
# is possible to get that state thanks to the methods *getState*.
# It is possible to create an ARMA with a specific current state. That
# specific current state is taken into account to generate possible
# futurs but not to generate realizations (in order to respect the
# stationarity property of the model).
# At the creation step, we check whether the process
# :math:`ARMA(p,q)` is stationnary.
# When the process is not stationary, the user is warned by a message.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m

# %%
# Define the recurrence coefficients AR, MA (4,2)
myARCoef = ot.ARMACoefficients([0.4, 0.3, 0.2, 0.1])
myMACoef = ot.ARMACoefficients([0.4, 0.3])

# %%
# Define the white noise distribution of the recurrent relation.
myTimeGrid = ot.RegularGrid(0.0, 0.1, 10)
myWhiteNoise = ot.WhiteNoise(ot.Triangular(-1.0, 0.0, 1.0), myTimeGrid)

# %%
# Create the process:
process = ot.ARMA(myARCoef, myMACoef, myWhiteNoise)
print(process)

# %%
# Draw a sample
sample = process.getSample(6)
graph = sample.drawMarginal(0)
view = viewer.View(graph)
plt.show()
