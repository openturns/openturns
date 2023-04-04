"""
Use case : logistic model
=========================
"""
import openturns as ot
import numpy as np


class LogisticModel:
    """
    Data class for the logistic model.


    Attributes
    ----------

    y0 : Constant
         Initial population (in 1790) y0=3.9e6

    a : Constant
        Parameter of the model a=0.03134

    b : Constant
        Parameter of the model b=1.5887e-10

    nbdates : Constant
        The number of years in the dataset.

    distY0 : `Normal` distribution
             ot.Normal(y0, 0.1 * y0)

    distA : `Normal` distribution
            ot.Normal(a, 0.3 * a)

    distB : `Normal` distribution
            ot.Normal(b, 0.3 * b)

    distX : `ComposedDistribution`
            The joint distribution of the input parameters.

    model : `SymbolicFunction`
            The logistic model of growth.

    data : `Sample`
           22 dates from 1790 to 2000.
           First marginal represents dates and second marginal the population in millions.

    Examples
    --------
    >>> from openturns.usecases import logistic_model
    >>> # Load the logistic model
    >>> lm = logistic_model.LogisticModel()
    """

    def __init__(self):
        # Initial value of the population
        self.y0 = 3.9e6
        self.a = 0.03134
        self.b = 1.5887e-10
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
        self.nbdates = self.data.getSize()

        def logisticModel(X):
            t = [X[i] for i in range(self.nbdates)]
            a = X[22]
            c = X[23]
            t0 = 1790.0
            y0 = 3.9e6
            b = np.exp(c)
            y = [0.0] * self.nbdates
            for i in range(self.nbdates):
                y[i] = a * y0 / (b * y0 + (a - b * y0) * np.exp(-a * (t[i] - t0)))
            z = [yi / 1.0e6 for yi in y]  # Convert into millions
            return z
        self.model = ot.PythonFunction(24, self.nbdates, logisticModel)
        inputLabels = ["t%d" % (i) for i in range(self.nbdates)]
        inputLabels.append("a")
        inputLabels.append("c")
        outputLabels = ["z%d" % (i) for i in range(self.nbdates)]
        self.model.setInputDescription(inputLabels)
        self.model.setOutputDescription(outputLabels)
