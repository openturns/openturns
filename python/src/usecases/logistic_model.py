"""
Use case : logistic model
=========================
"""
import openturns as ot
import math


class LogisticModel:
    """
    Data class for the logistic model.


    Attributes
    ----------

    y0 : float, optional
         Initial population (in 1790). The default is 3.9e6.

    a : float, optional
        Parameter of the model. The default is 0.03134.

    b : float, optional
        Parameter of the model. The default is 1.5887e-10.

    distY0 : :class:`~openturns.Normal` distribution
             ot.Normal(y0, 0.1 * y0)

    distA : :class:`~openturns.Normal` distribution
            ot.Normal(a, 0.3 * a)

    distB : :class:`~openturns.Normal` distribution
            ot.Normal(b, 0.3 * b)

    distX : :class:`~openturns.ComposedDistribution`
            The joint distribution of the input parameters.

    model : :class:`~openturns.PythonFunction`
            The logistic model of growth.

    data : :class:`~openturns.Sample` of size 22 and dimension 2
           A data set containing 22 dates from 1790 to 2000.
           First marginal represents dates and second marginal the population in millions.

    Examples
    --------
    >>> from openturns.usecases import logistic_model
    >>> # Load the logistic model
    >>> lm = logistic_model.LogisticModel()
    """

    def __init__(self, y0=3.9e6, a=0.03134, b=1.5887e-10):
        # Initial value of the population
        self.y0 = y0
        self.a = a
        self.b = b
        self.distY0 = ot.Normal(self.y0, 0.1 * self.y0)
        self.distA = ot.Normal(self.a, 0.3 * self.a)
        self.distB = ot.Normal(self.b, 0.3 * self.b)
        self.distX = ot.ComposedDistribution([self.distY0, self.distA, self.distB])
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
            t0 = 1790.0
            b = math.exp(c)
            y = [0.0] * nbdates
            for i in range(nbdates):
                y[i] = a * y0 / (b * y0 + (a - b * y0) * math.exp(-a * (t[i] - t0)))
            z = [yi / 1.0e6 for yi in y]  # Convert into millions
            return z

        self.model = ot.PythonFunction(24, nbdates, logisticModel)
        inputLabels = ["t%d" % (i) for i in range(nbdates)]
        inputLabels.append("a")
        inputLabels.append("c")
        outputLabels = ["z%d" % (i) for i in range(nbdates)]
        self.model.setInputDescription(inputLabels)
        self.model.setOutputDescription(outputLabels)
