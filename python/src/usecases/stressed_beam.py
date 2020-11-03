"""
Use case : axial stressed beam
==============================
"""
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
from typing import Any, List
import numpy as np
from dataclasses import dataclass
from dataclasses import field


@dataclass
class AxialStressedBeam():
    """
    Data class for the axial stressed beam example.


    Attributes
    ----------

    dim : The dimension of the problem
          dim=2.
 
    D : Constant
        Diameter D = 0.02 (m)

    model : `SymbolicFunction`
            The limit state function.

    muR : Constant
          muR=3.0e6, yield strength mean

    sigmaR : Constant
             sigmaR = 3.0e5, yield strength variance

    distribution_R : `LogNormalMuSigma` distribution of the yield strength
                      ot.LogNormalMuSigma(muR, sigmaR, 0.0).getDistribution()

    muF : Constant
          muF=750.0, traction load mean

    sigmaF : Constant
             sigmaR = 50.0, traction load variance

    distribution_F : `Normal` distribution of the traction load
                     ot.Normal(muF, sigmaF)

    distribution : `ComposedDistribution`
                   The joint distribution of the inpput parameters.

    Examples
    --------
    >>> from openturns.usecases import stressed_beam as stressed_beam
    >>> # Load the axial stressed beam
    >>> sm = stressed_beam.AxialStressedBeam()
    """

    dim: int = 2
    D: float = 0.02
    # Random variable : R
    muR: float = 3.0e6
    sigmaR: float = 3.0e5
    # Random variable : F
    muF: float = 750.
    sigmaF: float = 50.
    # create the limit state function model
    model: Any = ot.SymbolicFunction(['R', 'F'], ['R-F/(pi_/10000.0)'])

    # Yield strength
    distribution_R: Any = ot.LogNormalMuSigma(muR, sigmaR, 0.0).getDistribution()
    distribution_R.setName('Yield strength')
    distribution_R.setDescription('R')
    # Traction load
    distribution_F: Any = ot.Normal(muF, sigmaF)
    distribution_F.setName('Traction_load')
    distribution_F.setDescription('F')

    # Joint distribution of the input parameters
    distribution: Any = ot.ComposedDistribution([distribution_R, distribution_F])

