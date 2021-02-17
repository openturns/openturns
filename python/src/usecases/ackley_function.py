"""
Use case : Ackley test function
===============================
"""
from __future__ import print_function
import openturns as ot
import math as m



def g(X):
    a = 20.0
    b = 0.2
    c = 2.0 * m.pi
    d = len(X)
    sumOfSquared = sum(x**2 for x in X) / d
    sumOfCos = sum(m.cos(c * x) for x in X) / d
    f = - a * m.exp(- b * m.sqrt(sumOfSquared)) \
        - m.exp(sumOfCos) + a + m.exp(1.0)
    return [f]


class AckleyModel():
    """
    Data class for the Ackley test function.


    Attributes
    ----------

    dim : The dimension of the problem
          dim=2.

    a : Constant
        :math:`a=20.0`.

    b : Constant
        :math:`b=0.2`.

    c : Constant
        :math:`c=2\pi`.

    model : `PythonFunction`, the Ackley function.

    lowerbound : `Point` in dimension dim.
                 Default is -15.0 for each dimension.

    upperbound : `Point` in dimension dim.
                 Default is +15.0 for each dimension.

    x0 : `Point` 
         The global minimum :math:`x_0 = 0 \in \mathbb{R}^{dim}`.

    Examples
    --------
    >>> from openturns.usecases import ackley_function as ackley_function
    >>> # Load the Ackley model
    >>> am = ackley_function.AckleyModel()
    """

    def __init__(self):
        self.dim = 2
        self.a = 20.0
        self.b = 0.2
        self.c = 2.0 * m.pi

        self.model = ot.PythonFunction(self.dim, 1, g)

        # Bounds
        self.lowerbound = ot.Point([-15.0] * self.dim)
        self.upperbound = ot.Point([15.0] * self.dim)

        # The Ackley function has many local minimas
        # The global minimum is unique and located at the center of the domain
        self.x0 = [0.0] * self.dim
