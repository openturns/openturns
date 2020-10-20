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

# model


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
    """Custom class for the Ackley test function.
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
