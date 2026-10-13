"""
Use case : Ishigami test function
=================================
"""

import openturns as ot
import math as m


class IshigamiModel:
    """
    Data class for the Ishigami model.


    Attributes
    ----------


    dim : The dimension of the problem
          dim = 3

    a : float
        Constant: a = 7.0

    b : float
        Constant: b = 0.1

    X1 : :class:`~openturns.Uniform`
        First marginal, ot.Uniform(-np.pi, np.pi)

    X2 : :class:`~openturns.Uniform`
        Second marginal, ot.Uniform(-np.pi, np.pi)

    X3 : :class:`~openturns.Uniform`
        Third marginal, ot.Uniform(-np.pi, np.pi)

    distribution : :class:`~openturns.JointDistribution`
        The joint distribution of the input parameters.

    ishigami : :class:`~openturns.SymbolicFunction`
        The Ishigami model with a, b as variables.

    model : :class:`~openturns.ParametricFunction`
        The Ishigami model with the a=7.0 and b=0.1 parameters fixed.

    expectation : float
        Expectation of the output variable.

    variance : float
        Variance of the output variable.

    getNonZeroPolynomialChaosExpansionCoefficients : function
        Returns the non-zero coefficients of the polynomial chaos expansion for each given rank.

    getNonZeroPolynomialChaosExpansionIndices : function
        Returns the non-zero indices of the polynomial chaos expansion for each given rank.


    S1 : float
         First order Sobol index number 1

    S2 : float
         First order Sobol index number 2

    S3 : float
         First order Sobol index number 3

    S12 : float
          Second order Sobol index for marginals 1 and 2.

    S13 : float
          Second order Sobol index for marginals 1 and 3.

    S23 : float
          Second order Sobol index for marginals 2 and 3.

    S123 : float

    ST1 : float
          Total order Sobol index number 1.

    ST2 : float
          Total order Sobol index number 2.

    ST3 : float
          Total order Sobol index number 3.


    Examples
    --------
    >>> from openturns.usecases import ishigami_function
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
        self.X1 = ot.Uniform(-m.pi, m.pi)
        self.X1.setName("X1")
        self.X1.setDescription(["X1"])

        # Second marginal : X2
        self.X2 = ot.Uniform(-m.pi, m.pi)
        self.X2.setName("X2")
        self.X2.setDescription(["X2"])

        # Third marginal : X3
        self.X3 = ot.Uniform(-m.pi, m.pi)
        self.X3.setName("X3")
        self.X3.setDescription(["X3"])

        # Input distribution
        self.distribution = ot.JointDistribution([self.X1, self.X2, self.X3])
        self.distribution.setDescription(["X1", "X2", "X3"])
        self.inputDistribution = self.distribution  # deprecated

        self.ishigami = ot.SymbolicFunction(
            ["X1", "X2", "X3", "a", "b"],
            ["sin(X1) + a * sin(X2)^2 + b * X3^4 * sin(X1)"],
        )
        self.ishigami.setOutputDescription(["y"])
        # The Ishigami model
        self.model = ot.ParametricFunction(self.ishigami, [3, 4], [self.a, self.b])

        self.expectation = self.a / 2.0
        self.variance = (
            1.0 / 2
            + self.a**2 / 8.0
            + self.b * m.pi**4 / 5.0
            + self.b**2 * m.pi**8 / 18.0
        )
        self.S1 = (
            1.0 / 2.0 + self.b * m.pi**4 / 5.0 + self.b**2 * m.pi**8 / 50.0
        ) / self.variance
        self.S2 = (self.a**2 / 8.0) / self.variance
        self.S3 = 0.0
        self.S12 = 0.0
        self.S13 = self.b**2 * m.pi**8 / 2.0 * (1.0 / 9.0 - 1.0 / 25.0) / self.variance
        self.S23 = 0.0
        self.S123 = 0.0
        self.ST1 = self.S1 + self.S13
        self.ST2 = self.S2
        self.ST3 = self.S3 + self.S13

    def getNonZeroPolynomialChaosExpansionCoefficients(self, index):
        coefficients = [
            3.50000000000000000e00,
            1.62541782669813939e00,
            -5.94722767235643324e-01,
            -1.29063821501878118e00,
            1.37242049383657827e00,
            -1.95229073087452409e00,
            1.94928729486692709e-01,
            -1.08974954459471918e00,
            4.09176735724322727e-01,
            1.35740781523858622e00,
            -1.26683810377066063e-02,
            1.64587947044059490e-01,
            -3.24900541355097439e-01,
            -3.39390470877308947e-01,
            4.61416845109263469e-04,
            -1.06965393600965530e-02,
            4.90706450490203464e-02,
            4.59147258927077376e-02,
            -1.07818513564668195e-05,
            3.89597015627524113e-04,
            -3.18909188442373284e-03,
            -3.95538116085697444e-03,
            1.75613797732120148e-07,
            -9.10364924025335292e-06,
            1.16155388103225598e-04,
            2.37643088813989530e-04,
            -2.10953290307027228e-09,
            1.48279396872145311e-07,
            -2.71418380593610885e-06,
            -1.05632875166840454e-05,
            1.94696846454018182e-11,
            -1.78118274639415706e-09,
            4.42083748091715618e-08,
            3.62190537281325296e-07,
            -1.42427585282122589e-13,
            1.64392156754948388e-11,
            -5.31046093504947162e-10,
            -9.88327332800136835e-09,
            8.46302176689299647e-16,
            -1.20258639789819033e-13,
            4.90122716629154560e-12,
            2.19913017773790883e-10,
            7.14574697157300122e-16,
            -3.58541991268928596e-14,
            -4.06875638545441148e-12,
            2.13045013046006517e-16,
        ]
        if index < -len(coefficients) or index >= len(coefficients):
            raise ValueError(
                "Index out of range for polynomial chaos expansion coefficients."
            )
        return coefficients[index]

    def getNonZeroPolynomialChaosExpansionIndices(self, index):
        multiIndices = [
            [0, 0, 0],
            [1, 0, 0],
            [0, 2, 0],
            [3, 0, 0],
            [1, 0, 2],
            [0, 4, 0],
            [5, 0, 0],
            [3, 0, 2],
            [1, 0, 4],
            [0, 6, 0],
            [7, 0, 0],
            [5, 0, 2],
            [3, 0, 4],
            [0, 8, 0],
            [9, 0, 0],
            [7, 0, 2],
            [5, 0, 4],
            [0, 10, 0],
            [11, 0, 0],
            [9, 0, 2],
            [7, 0, 4],
            [0, 12, 0],
            [13, 0, 0],
            [11, 0, 2],
            [9, 0, 4],
            [0, 14, 0],
            [15, 0, 0],
            [13, 0, 2],
            [11, 0, 4],
            [0, 16, 0],
            [17, 0, 0],
            [15, 0, 2],
            [13, 0, 4],
            [0, 18, 0],
            [19, 0, 0],
            [17, 0, 2],
            [15, 0, 4],
            [0, 20, 0],
            [21, 0, 0],
            [19, 0, 2],
            [17, 0, 4],
            [0, 22, 0],
            [21, 0, 2],
            [19, 0, 4],
            [0, 24, 0],
            [21, 0, 4],
        ]
        if index < -len(multiIndices) or index >= len(multiIndices):
            raise ValueError(
                "Index out of range for polynomial chaos expansion indices."
            )
        return multiIndices[index]
