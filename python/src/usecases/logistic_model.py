"""
Use case : logistic model
=========================
"""

import openturns as ot
import math


class LogisticModel:
    r"""
    Data class for the logistic model.

    In the physical model, the inputs and parameters are ordered as
    presented in the next table.
    Notice that there are no parameters in the physical model.

    +-------+----------------+
    | Index | Input variable |
    +=======+================+
    | 0     | t1             |
    +-------+----------------+
    | 1     | t2             |
    +-------+----------------+
    | ...   | ...            |
    +-------+----------------+
    | 21    | t22            |
    +-------+----------------+
    | 22    | a              |
    +-------+----------------+
    | 23    | c              |
    +-------+----------------+

    Attributes
    ----------

    t0 : float, optional
        Initial time. The default is 1790.

    y0 : float, optional
        Initial population (at t0). The default is 3.9e6.

    a : float, optional
        8Parameter of the model. The default is 0.03134.

    b : float, optional
        Parameter of the model. The default is 1.5887e-10.

    populationFactor : float, optional
        The multiplication factor to scale the population.
        The default is 1.0e6.

    distY0 : :class:`~openturns.Normal`
        `ot.Normal(y0, 0.1 * y0)`

    distA : :class:`~openturns.Normal`
        `ot.Normal(a, 0.3 * a)`

    distB : :class:`~openturns.Normal`
        `ot.Normal(b, 0.3 * b)`

    distX : :class:`~openturns.JointDistribution`
        The joint distribution of the input parameters.

    model : :class:`~openturns.PythonFunction`
        The logistic model of growth.
        The input has input dimension 24 and output dimension 22.
        More precisely, we have :math:`\vect{X} = (t_1, ..., t_{22}, a, c)`
        and  :math:`\vect{Y} = (y_1, ..., y_{22})`.

    data : :class:`~openturns.Sample`
        A data set containing 22 dates from 1790 to 2000.
        First marginal represents dates and second marginal the population in millions.

    Examples
    --------
    >>> from openturns.usecases import logistic_model
    >>> # Load the logistic model
    >>> lm = logistic_model.LogisticModel()
    >>> print(lm.data[:5])
        [ Time            U.S. Population ]
    0 : [ 1790               3.9          ]
    1 : [ 1800               5.3          ]
    2 : [ 1810               7.2          ]
    3 : [ 1820               9.6          ]
    4 : [ 1830              13            ]
    >>> print("Inputs:", lm.model.getInputDescription())
    Inputs: [t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,a,c]#24
    >>> print("Outputs:", lm.model.getOutputDescription())
    Outputs: [z0,z1,z2,z3,z4,z5,z6,z7,z8,z9,z10,z11,z12,z13,z14,z15,z16,z17,z18,z19,z20,z21]#22
    """

    def __init__(
        self, t0=1790.0, y0=3.9e6, a=0.03134, b=1.5887e-10, populationFactor=1.0e6
    ):
        # Initial value of the population
        self.t0 = t0
        self.y0 = y0
        self.populationFactor = populationFactor
        self.a = a
        self.b = b
        self.distY0 = ot.Normal(self.y0, 0.1 * self.y0)
        self.distA = ot.Normal(self.a, 0.3 * self.a)
        self.distB = ot.Normal(self.b, 0.3 * self.b)
        self.distX = ot.JointDistribution([self.distY0, self.distA, self.distB])
        # Observation points
        self.data = ot.Sample(
            [
                [1790, 3.9],
                [1800, 5.3],
                [1810, 7.2],
                [1820, 9.6],
                [1830, 13],
                [1840, 17],
                [1850, 23],
                [1860, 31],
                [1870, 39],
                [1880, 50],
                [1890, 62],
                [1900, 76],
                [1910, 92],
                [1920, 106],
                [1930, 123],
                [1940, 132],
                [1950, 151],
                [1960, 179],
                [1970, 203],
                [1980, 221],
                [1990, 250],
                [2000, 281],
            ]
        )
        self.data.setDescription(["Time", "U.S. Population"])
        nbdates = self.data.getSize()

        def logisticModel(X):
            t = [X[i] for i in range(nbdates)]
            a = X[22]
            c = X[23]
            b = math.exp(c)
            y = [0.0] * nbdates
            for i in range(nbdates):
                y[i] = a * y0 / (b * y0 + (a - b * y0) * math.exp(-a * (t[i] - t0)))
            z = [yi / populationFactor for yi in y]  # Convert into millions
            return z

        self.model = ot.PythonFunction(24, nbdates, logisticModel)
        inputLabels = [f"t{i}" for i in range(nbdates)]
        inputLabels.append("a")
        inputLabels.append("c")
        outputLabels = [f"z{i}" for i in range(nbdates)]
        self.model.setInputDescription(inputLabels)
        self.model.setOutputDescription(outputLabels)
