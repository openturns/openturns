"""
Use case : Ishigami test function
=================================
"""
from __future__ import print_function
import openturns as ot
from typing import Any, List
import numpy as np
from dataclasses import dataclass
from dataclasses import field


@dataclass
class IshigamiModel():
    """Custom class for the Ishigami test function
    """
    dim: int = 3
    a: float = 7.0
    b: float = 0.1

    # X1
    X1: Any = ot.Uniform(-np.pi, np.pi)
    X1.setName("X1")

    # X2
    X2: Any = ot.Uniform(-np.pi, np.pi)
    X2.setName("X2")

    # X3
    X3: Any = ot.Uniform(-np.pi, np.pi)
    X3.setName("X1")

    distributionX = ot.ComposedDistribution([X1, X2, X3])
    distributionX.setDescription(['X1', 'X2', 'X3'])

    ishigami: Any = ot.SymbolicFunction(['X1', 'X2', 'X3', 'a', 'b'], ['sin(X1) + a * sin(X2)^2 + b * X3^4 * sin(X1)'])

    model: Any = ot.ParametricFunction(ishigami, [3, 4], [a, b])

    expectation: float = a / 2.0
    variance: float = 1.0/2 + a**2/8.0 + b*np.pi**4/5.0 + b**2*np.pi**8/18.0
    S1: float = (1.0/2.0 + b*np.pi**4/5.0+b**2*np.pi**8/50.0)/variance
    S2: float = (a**2/8.0)/variance
    S3: float = 0.0
    S12: float = 0.0
    S13: float = b**2*np.pi**8/2.0*(1.0/9.0-1.0/25.0)/variance
    S23: float = 0.0
    S123: float = 0.0
    ST1: float = S1 + S13
    ST2: float = S2
    ST3: float = S3 + S13
