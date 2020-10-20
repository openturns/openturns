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
    """Custom class for the beam example.
    """
    D: float = 0.02
    # Random variable : R
    muR: float = 3.0e6
    sigmaR: float = 3.0e5
    # Random variable : F
    muF: float = 750.
    sigmaF: float = 50.
    # create the limit state function model
    model: Any = ot.SymbolicFunction(['R', 'F'], ['R-F/(pi_/10000.0)'])
    # Create the joint distribution of the parameters.
    distribution_R: Any = ot.LogNormalMuSigma(muR, sigmaR, 0.0).getDistribution()
    distribution_F: Any = ot.Normal(muF, sigmaF)
    distribution: Any = ot.ComposedDistribution([distribution_R, distribution_F])

    # Create the event whose probability we want to estimate.
    vect: Any = ot.RandomVector(distribution)
    G: Any = ot.CompositeRandomVector(model, vect)
    event: Any = ot.ThresholdEvent(G, ot.Less(), 0.0)
