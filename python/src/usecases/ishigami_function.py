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
    """
    Data class for the Ishigami model.


    Attributes
    ----------


    dim : The dimension of the problem
          dim = 3

    a : Constant
        a = 7.0

    b : Constant
        b = 0.1

    X1 : `Uniform` distribution
         First marginal, ot.Uniform(-np.pi, np.pi)

    X2 : `Uniform` distribution
         Second marginal, ot.Uniform(-np.pi, np.pi)

    X3 : `Uniform` distribution
         Third marginal, ot.Uniform(-np.pi, np.pi)

    distributionX : `ComposedDistribution`
                    The joint distribution of the input parameters.

    ishigami : `SymbolicFunction`
               The Ishigami model with a, b as variables.

    model : `ParametricFunction`
            The Ishigami model with the a=7.0 and b=0.1 parameters fixed.

    expectation : Constant
                  Expectation of the output variable.

    variance : Constant
               Variance of the output variable.

    S1 : Constant
         First order Sobol index number 1

    S2 : Constant
         First order Sobol index number 2

    S3 : Constant
         First order Sobol index number 3

    S12 : Constant
          Second order Sobol index for marginals 1 and 2.

    S13 : Constant
          Second order Sobol index for marginals 1 and 3.

    S23 : Constant
          Second order Sobol index for marginals 2 and 3.

    S123 : Constant

    ST1 : Constant
          Total order Sobol index number 1.

    ST2 : Constant
          Total order Sobol index number 2.

    ST3 : Constant
          Total order Sobol index number 3.


    Examples
    --------
    >>> from openturns.usecases import ishigami_function as ishigami_function
    >>> # Load the Ishigami model
    >>> im = ishigami_function.IshigamiModel()
    """

    # dimension
    dim: int = 3
    # Fixed parameters for the Ishigami function
    a: float = 7.0
    b: float = 0.1

    # First marginal : X1
    X1: Any = ot.Uniform(-np.pi, np.pi)
    X1.setName("X1")

    # Second marginal : X2
    X2: Any = ot.Uniform(-np.pi, np.pi)
    X2.setName("X2")

    # Third marginal : X3
    X3: Any = ot.Uniform(-np.pi, np.pi)
    X3.setName("X1")

    # Input distribution
    distributionX = ot.ComposedDistribution([X1, X2, X3])
    distributionX.setDescription(['X1', 'X2', 'X3'])

    ishigami: Any = ot.SymbolicFunction(['X1', 'X2', 'X3', 'a', 'b'], ['sin(X1) + a * sin(X2)^2 + b * X3^4 * sin(X1)'])
    # The Ishigami model
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
