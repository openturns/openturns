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
    """
    Data class for the cantilever beam example.


    Attributes
    ----------

    dim : The dimension of the problem
          dim=2.

    E : `Beta` distribution
        ot.Beta(0.9, 3.5, 65.0e9, 75.0e9)
        
    F : `LogNormal` distribution
        ot.LogNormalMuSigma()([300.0, 30.0, 0.0])

    L : `Uniform` distribution
        ot.Uniform(2.5, 2.6)

    I : `Beta` distribution
        ot.Beta(2.5, 4.0, 1.3e-7, 1.7e-7)

    model : `SymbolicFunction`, the physical model of the cantilever beam.

    R : `CorrelationMatrix`
        Correlation matrix used to define the copula.
 
    copula : `NormalCopula`
             Copula of the model.

    distribution : `ComposedDistribution`
                   The joint distribution of the parameters.

    independentDistribution : `ComposedDistribution`
                              The joint distribution of the parameters with independent copula.


    Examples
    --------
    >>> from openturns.usecases import cantilever_beam as cantilever_beam
    >>> # Load the cantilever beam model
    >>> cb = cantilever_beam.CantileverBeam()
    """                                                                                       

    dim: int = 4  # number of inputs
    # Young's modulus E
    E: Any = ot.Beta(0.9, 3.5, 65.0e9, 75.0e9)  # in N/m^2
    E.setDescription("E")
    E.setName("Young modulus")

    # Load F
    F: Any = ot.LogNormal()  # in N
    F.setParameter(ot.LogNormalMuSigma()([300.0, 30.0, 0.0]))
    F.setDescription("F")
    F.setName("Load")

    # Length L
    L: Any = ot.Uniform(2.5, 2.6)  # in m
    L.setDescription("L")
    L.setName("Length")

    # Moment of inertia I
    I: Any = ot.Beta(2.5, 4.0, 1.3e-7, 1.7e-7)  # in m^4
    I.setDescription("I")
    I.setName("Inertia")

    # physical model
    model: Any = ot.SymbolicFunction(['E', 'F', 'L', 'I'], ['F*L^3/(3*E*I)'])

    # correlation matrix
    R: Any = ot.CorrelationMatrix(dim)
    R[2, 3] = -0.2
    copula: Any = ot.NormalCopula(ot.NormalCopula.GetCorrelationFromSpearmanCorrelation(R))
    distribution: Any = ot.ComposedDistribution([E, F, L, I], copula)

    # special case of an independent copula
    independentDistribution: Any = ot.ComposedDistribution([E, F, L, I])

