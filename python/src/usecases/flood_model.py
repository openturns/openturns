"""
Use case : flood model
======================
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
class FloodModel():
    """
    Data class for the flood model.


    Attributes
    ----------

    dim : The dimension of the problem
          dim=4

    L : Constant
        Length of the river, L = 5000.0

    B : Constant
        Width of the river, B = 300.0

    Q : `TruncatedDistribution` of a `Gumbel` distribution
        ot.TruncatedDistribution(ot.Gumbel(558., 1013.), 0, ot.TruncatedDistribution.LOWER)

    Ks : `TruncatedDistribution` of a `Normal` distribution
         ot.TruncatedDistribution(ot.Normal(30.0, 7.5), 0, ot.TruncatedDistribution.LOWER)

    Zv : `Uniform` distribution
         ot.Uniform(49.0, 51.0)

    Zm : `Uniform` distribution
         ot.Uniform(54.0, 56.0)

    model : `SymbolicFunction`
            The flood model.

    distribution : `ComposedDistribution`
                   The joint distribution of the input parameters.


    Examples
    --------
    >>> from openturns.usecases import flood_model as flood_model
    >>> # Load the flood model
    >>> fm = flood_model.FloodModel()
    """

    # Length of the river in meters
    L: float = 5000.0
    # Width of the river in meters
    B: float = 300.0
    dim: int = 4  # number of inputs
    # Q
    Q: Any = ot.TruncatedDistribution(ot.Gumbel(558., 1013.), 0, ot.TruncatedDistribution.LOWER)
    Q.setDescription(["Q (m3/s)"])
    Q.setName("Q")

    # Ks
    Ks: Any = ot.TruncatedDistribution(ot.Normal(30.0, 7.5), 0, ot.TruncatedDistribution.LOWER)
    Ks.setName("Ks")

    # Zv
    Zv: Any = ot.Uniform(49.0, 51.0)
    Zv.setName("Zv")

    # Zm
    Zm: Any = ot.Uniform(54.0, 56.0)
    #Zm.setDescription(["Zm (m)"])
    Zm.setName("Zm")

    model: Any = ot.SymbolicFunction(['Q', 'Ks', 'Zv', 'Zm'],
                                     ['(Q/(Ks*300.*sqrt((Zm-Zv)/5000)))^(3.0/5.0)+Zv-58.5'])

    distribution: Any = ot.ComposedDistribution([Q, Ks, Zv, Zm])
    distribution.setDescription(['Q', 'Ks', 'Zv', 'Zm'])

