"""
Use case : the Chaboche mechanical model
========================================
"""
from __future__ import print_function
import openturns as ot
from typing import Any, List
import numpy as np
from dataclasses import dataclass
from dataclasses import field


def g(X):
    strain, R, C, gamma = X
    stress = R + C * (1 - np.exp(-gamma * strain)) / gamma
    return [stress]


@dataclass
class ChabocheModel() :
    """Custom class for the Chaboche model
    """

    dim : int = 4 # number of inputs

    Strain : Any = ot.Uniform(0.0, 0.07)
    Strain.setDescription(["Strain"])

    unknownR : float = 750.0e6
    R : Any = ot.Dirac(unknownR)
    R.setDescription(["R"])

    unknownC : float = 2750.0e6
    C : Any = ot.Dirac(unknownC)
    C.setDescription(["C"])

    unknownGamma : float = 10.0
    Gamma : Any = ot.Dirac(unknownGamma)
    Gamma.setDescription(["Gamma"])

    inputDistribution : Any = ot.ComposedDistribution([Strain, R, C, Gamma])

    model : Any = ot.PythonFunction(4, 1, g)

