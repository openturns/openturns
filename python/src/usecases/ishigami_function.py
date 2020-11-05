"""
Use case : Ishigami test function
=================================
"""
from __future__ import print_function
import openturns as ot
import numpy as np


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

    def __init__(self):
        # dimension
        self.dim = 3
        # Fixed parameters for the Ishigami function
        self.a = 7.0
        self.b = 0.1

        # First marginal : X1
        self.X1 = ot.Uniform(-np.pi, np.pi)
        self.X1.setName("X1")

        # Second marginal : X2
        self.X2 = ot.Uniform(-np.pi, np.pi)
        self.X2.setName("X2")

        # Third marginal : X3
        self.X3 = ot.Uniform(-np.pi, np.pi)
        self.X3.setName("X1")

        # Input distribution
        self.distributionX = ot.ComposedDistribution([self.X1, self.X2, self.X3])
        self.distributionX.setDescription(['X1', 'X2', 'X3'])

        self.ishigami = ot.SymbolicFunction(['X1', 'X2', 'X3', 'a', 'b'], ['sin(X1) + a * sin(X2)^2 + b * X3^4 * sin(X1)'])
        # The Ishigami model
        self.model = ot.ParametricFunction(self.ishigami, [3, 4], [self.a, self.b])

        self.expectation = self.a / 2.0
        self.variance = 1.0/2 + self.a**2/8.0 + self.b*np.pi**4/5.0 + self.b**2*np.pi**8/18.0
        self.S1 = (1.0/2.0 + self.b*np.pi**4/5.0+self.b**2*np.pi**8/50.0)/self.variance
        self.S2 = (self.a**2/8.0)/self.variance
        self.S3 = 0.0
        self.S12 = 0.0
        self.S13 = self.b**2*np.pi**8/2.0*(1.0/9.0-1.0/25.0)/self.variance
        self.S23 = 0.0
        self.S123 = 0.0
        self.ST1 = self.S1 + self.S13
        self.ST2 = self.S2
        self.ST3 = self.S3 + self.S13
