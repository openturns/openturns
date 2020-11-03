"""
Use case : logistic model
=========================
"""
from __future__ import print_function
import openturns as ot
from typing import Any, List
import numpy as np
from dataclasses import dataclass
from dataclasses import field


@dataclass
class LogisticModel():
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
    >>> from openturns.usecases import logistic_model as logistic_model
    >>> # Load the logistic model
    >>> lm = logistic_model.LogisticModel()
    """

    # Initial value of the population
    y0: float = 3.9e6
    a: float = 0.03134
    b: float = 1.5887e-10
    distY0: Any = ot.Normal(y0, 0.1 * y0)
    distA: Any = ot.Normal(a, 0.3 * a)
    distB: Any = ot.Normal(b, 0.3 * b)
    distX: Any = ot.ComposedDistribution([distY0, distA, distB])
    model: Any = ot.SymbolicFunction(['t', 'y', 'a', 'b'], ['a*y - b*y^2'])

    # Observation points
    data: Any=ot.Sample([\
          [1790,3.9], \
          [1800,5.3], \
          [1810,7.2], \
          [1820,9.6], \
          [1830,13], \
          [1840,17], \
          [1850,23], \
          [1860,31], \
          [1870,39], \
          [1880,50], \
          [1890,62], \
          [1900,76], \
          [1910,92], \
          [1920,106], \
          [1930,123], \
          [1940,132], \
          [1950,151], \
          [1960,179], \
          [1970,203], \
          [1980,221], \
          [1990,250], \
          [2000,281]])

