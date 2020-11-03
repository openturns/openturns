"""
Use case : Ackley test function
===============================
"""
from __future__ import print_function
import openturns as ot
from typing import Any, List
import numpy as np
from dataclasses import dataclass
from dataclasses import field



def g(X):
    a = 20.0
    b = 0.2
    c = 2.0 * np.pi
    d = len(X)
    sumOfSquared = sum(x**2 for x in X) / d
    sumOfCos = sum(np.cos(c * x) for x in X) / d
    f = - a * np.exp(- b * np.sqrt(sumOfSquared)) \
        - np.exp(sumOfCos) + a + np.exp(1.0)
    return [f]


@dataclass
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

    dim: int = 2
    a = 20.0
    b = 0.2
    c = 2.0 * np.pi

    model: Any = ot.PythonFunction(dim, 1, g)

    # Bounds
    lowerbound: Any = ot.Point([-15.0] * dim)
    upperbound: Any = ot.Point([15.0] * dim)

    # The Ackley function has many local minimas
    # The global minimum is unique and located at the center of the domain
    x0: List[float] = field(default_factory=list)

    def __post_init__(self):
        self.x0 = [0.0] * self.dim
