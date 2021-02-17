"""
Use case : flood model
======================
"""
from __future__ import print_function
import openturns as ot


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

    def __init__(self):
        # Length of the river in meters
        self.L = 5000.0
        # Width of the river in meters
        self.B = 300.0
        self.dim = 4  # number of inputs
        # Q
        self.Q = ot.TruncatedDistribution(ot.Gumbel(558., 1013.), 0, ot.TruncatedDistribution.LOWER)
        self.Q.setDescription(["Q (m3/s)"])
        self.Q.setName("Q")

        # Ks
        self.Ks = ot.TruncatedDistribution(ot.Normal(30.0, 7.5), 0, ot.TruncatedDistribution.LOWER)
        self.Ks.setName("Ks")

        # Zv
        self.Zv = ot.Uniform(49.0, 51.0)
        self.Zv.setName("Zv")

        # Zm
        self.Zm = ot.Uniform(54.0, 56.0)
        #Zm.setDescription(["Zm (m)"])
        self.Zm.setName("Zm")

        self.model = ot.SymbolicFunction(['Q', 'Ks', 'Zv', 'Zm'],
                                         ['(Q/(Ks*300.*sqrt((Zm-Zv)/5000)))^(3.0/5.0)+Zv-58.5'])

        self.distribution = ot.ComposedDistribution([self.Q, self.Ks, self.Zv, self.Zm])
        self.distribution.setDescription(['Q', 'Ks', 'Zv', 'Zm'])

