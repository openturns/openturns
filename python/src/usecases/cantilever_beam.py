"""
Use case : cantilever beam
==========================
"""
from __future__ import print_function
import openturns as ot
from typing import Any, List
import numpy as np
from dataclasses import dataclass
from dataclasses import field


@dataclass
class CantileverBeam():
    """Custom class for the cantilever beam example.
    """
    dim: int = 4  # number of inputs
    # Young's modulus E
    E: Any = ot.Beta(0.9, 2.27, 2.5e7, 5.0e7)  # in N/m^2
    E.setDescription("E")
    E.setName("Young modulus")

    # Load F
    F: Any = ot.LogNormal()  # in N
    F.setParameter(ot.LogNormalMuSigma()([30.e3, 9e3, 15.e3]))
    F.setDescription("F")
    F.setName("Load")

    # Length L
    L: Any = ot.Uniform(250., 260.)  # in cm
    L.setDescription("L")
    L.setName("Length")

    # Moment of inertia I
    I: Any = ot.Beta(2.5, 1.5, 310, 450)  # in cm^4
    I.setDescription("I")
    I.setName("Inertia")

    model: Any = ot.SymbolicFunction(['E', 'F', 'L', 'I'], ['F*L^3/(3*E*I)'])

    R: Any = ot.CorrelationMatrix(dim)
    R[2, 3] = -0.2
    copula: Any = ot.NormalCopula(ot.NormalCopula.GetCorrelationFromSpearmanCorrelation(R))
    distribution: Any = ot.ComposedDistribution([E, F, L, I], copula)

    X: Any = ot.RandomVector(distribution)
    X.setDescription(['E', 'F', 'L', 'I'])
    Y: Any = ot.CompositeRandomVector(model, X)
    Y.setDescription('Y')
    event: Any = ot.ThresholdEvent(Y, ot.Greater(), 30.0)
