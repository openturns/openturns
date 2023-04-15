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

    L : float, optional
        Length of the river. The default is 5000.0.

    B : float, optional
        Width of the river. The default is 300.0.

    trueKs : float, optional
        The true value of the Ks parameter. The default is 30.0.

    trueZv : float, optional
        The true value of the Zv parameter. The default is 50.0.

    trueZm : float, optional
        The true value of the Zm parameter. The default is 55.0.


    Attributes
    ----------

    dim : The dimension of the problem
          dim=4

    Q : :class:`~openturns.TruncatedDistribution` of a :class:`~openturns.Gumbel` distribution
        `ot.TruncatedDistribution(ot.Gumbel(558.0, 1013.0), 0.0, ot.TruncatedDistribution.LOWER)`

    Ks : :class:`~openturns.TruncatedDistribution` of a :class:`~openturns.Normal` distribution
        `ot.TruncatedDistribution(ot.Normal(30.0, 7.5), 0.0, ot.TruncatedDistribution.LOWER)`

    Zv : :class:`~openturns.Uniform` distribution
        `ot.Uniform(49.0, 51.0)`

    Zm : :class:`~openturns.Uniform` distribution
        `ot.Uniform(54.0, 56.0)`

    model : :class:`~openturns.ParametricFunction`
        The flood model.
        The function has input dimension 4 :math:`\boldsymbol{X} = (Q, K_s, Z_v, Z_m)`
        and output dimension 1 :math:`Y = (H)`.
        Its parameters are :math:`\theta = (B, L)`.

    distribution : :class:`~openturns.ComposedDistribution`
        The joint distribution of the input parameters.

    data : :class:`~openturns.Sample` of size 10 and dimension 2
        A data set which contains noisy observations of the flow rate (column 0)
        and the height (column 1).

    Examples
    --------
    >>> from openturns.usecases import flood_model
    >>> # Load the flood model
    >>> fm = flood_model.FloodModel()
    >>> print(fm.data[:5])
        [ Q ($m^3/s$) H (m)       ]
    0 : [  130           0.59     ]
    1 : [  530           1.33     ]
    2 : [  960           2.03     ]
    3 : [ 1400           2.72     ]
    4 : [ 1830           2.83     ]
    >>> print("Inputs:", fm.model.getInputDescription())
    Inputs: [Q,Ks,Zv,Zm]
    >>> print("Parameters:", fm.model.getParameterDescription())
    Parameters: [B,L]
    >>> print("Outputs:", fm.model.getOutputDescription())
    Outputs: [H]
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
        self.model.setOutputDescription(["H"])

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
