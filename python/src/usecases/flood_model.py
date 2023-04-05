"""
Use case : flood model
======================
"""
import openturns as ot


class FloodModel:
    """
    Data class for the flood model.


    Parameters
    ----------

    L : float
        Length of the river. Default L = 5000.0.

    B : float
        Width of the river. Default B = 300.0.

    trueKs : float
        The true value of the Ks parameter.

    trueZv : float
        The true value of the Zv parameter.

    trueZm : float
        The true value of the Zm parameter.


    Attributes
    ----------

    dim : The dimension of the problem
          dim=4

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

    data : ot.Sample(10, 2)
        A dataset which contains noisy observations of the flowrate (column 0)
        and the height (column 1).

    Examples
    --------
    >>> from openturns.usecases import flood_model
    >>> # Load the flood model
    >>> fm = flood_model.FloodModel()
    """

    def __init__(self, L=5000.0, B=300.0, trueKs=30.0, trueZv=50.0, trueZm=55.0):
        self.trueKs = trueKs
        self.trueZv = trueZv
        self.trueZm = trueZm
        # Length of the river in meters
        self.L = L
        # Width of the river in meters
        self.B = B
        self.dim = 4  # number of inputs
        # Q
        self.Q = ot.TruncatedDistribution(
            ot.Gumbel(558.0, 1013.0), 0, ot.TruncatedDistribution.LOWER
        )
        self.Q.setDescription(["Q (m3/s)"])
        self.Q.setName("Q")

        # Ks
        self.Ks = ot.TruncatedDistribution(
            ot.Normal(30.0, 7.5), 0, ot.TruncatedDistribution.LOWER
        )
        self.Ks.setName("Ks")

        # Zv
        self.Zv = ot.Uniform(49.0, 51.0)
        self.Zv.setName("Zv")

        # Zm
        self.Zm = ot.Uniform(54.0, 56.0)
        # Zm.setDescription(["Zm (m)"])
        self.Zm.setName("Zm")

        g = ot.SymbolicFunction(
            ["Q", "Ks", "Zv", "Zm", "B", "L"],
            ["(Q / (Ks * B * sqrt((Zm - Zv) / L)))^(3.0 / 5.0) + Zv - 58.5"],
        )
        self.model = ot.ParametricFunction(g, [4, 5], [L, B])

        self.distribution = ot.ComposedDistribution([self.Q, self.Ks, self.Zv, self.Zm])
        self.distribution.setDescription(["Q", "Ks", "Zv", "Zm"])
        self.data = ot.Sample(
            [
                [130.0, 0.59],
                [530.0, 1.33],
                [960.0, 2.03],
                [1400.0, 2.72],
                [1830.0, 2.83],
                [2260.0, 3.50],
                [2700.0, 3.82],
                [3130.0, 4.36],
                [3560.0, 4.63],
                [4010.0, 4.96],
            ]
        )
        self.data.setDescription(["Q ($m^3/s$)", "H (m)"])
