"""
Use case : flood model
======================
"""

import openturns as ot


class FloodModel:
    r"""
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

    distributionHdLow : bool, optional
        If True, then the distribution of Hd is uniform in [2, 4] i.e
        the dyke is relatively low.
        Otherwise, the distribution of Hd is uniform in [7, 9] i.e
        the dyke is relatively high.
        The default is True.

    Attributes
    ----------

    dim : int
        The dimension of the problem: dim=4

    Q : :class:`~openturns.TruncatedDistribution`
        `ot.TruncatedDistribution(ot.Gumbel(558.0, 1013.0), 0.0, ot.TruncatedDistribution.LOWER)`

    Ks : :class:`~openturns.TruncatedDistribution`
        `ot.TruncatedDistribution(ot.Normal(30.0, 7.5), 0.0, ot.TruncatedDistribution.LOWER)`

    Zv : :class:`~openturns.Uniform`
        `ot.Uniform(49.0, 51.0)`

    Zm : :class:`~openturns.Uniform`
        `ot.Uniform(54.0, 56.0)`

    B : :class:`~openturns.Uniform`
        `Triangular(295.0, 300.0, 305.0)`

    L : :class:`~openturns.Uniform`
        `ot.Triangular(4990.0, 5000.0, 5010.0)`

    Hd : :class:`~openturns.Uniform`
        `ot.Uniform(54.0, 56.0)`

    Zb : :class:`~openturns.Uniform`
        The distribution depends on `distributionHdLow`.

    model : :class:`~openturns.ParametricFunction`
        The flood model.
        The function has input dimension 4 and output dimension 1.
        More precisely, we have  :math:`\vect{X} = (Q, K_s, Z_v, Z_m)` and
        :math:`Y = H`.
        Its parameters are :math:`\theta = (B, L)`.

    distribution : :class:`~openturns.JointDistribution`
        The joint distribution of the input parameters.

    data : :class:`~openturns.Sample`
        of size 10 and dimension 2
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
    Inputs: [Q, Ks, Zv, Zm, B, L, Zb, Hd]
    >>> print("Output:", fm.model.getOutputDescription())
    Output: [H, S, C]

    Get the height model.

    >>> heightInputDistribution, heightModel = fm.getHeightModel()
    >>> print("Inputs:", heightModel.getInputDescription())
    Inputs: [Q,Ks,Zv,Zm]
    >>> print("Outputs:", heightModel.getOutputDescription())
    Outputs: [H]

    Get the flooding model with high Hd scenario.

    >>> fm = flood_model.FloodModel(distributionHdLow=False)
    """

    def __init__(self, trueKs=30.0, trueZv=50.0, trueZm=55.0, distributionHdLow=True):
        self.trueKs = trueKs
        self.trueZv = trueZv
        self.trueZm = trueZm
        self.distributionHdLow = distributionHdLow

        # Q
        self.Q = ot.TruncatedDistribution(
            ot.Gumbel(558.0, 1013.0), 0.0, ot.TruncatedDistribution.LOWER
        )
        self.Q.setName("Q")
        self.Q.setDescription(["Q (m3/s)"])

        # Ks
        self.Ks = ot.TruncatedDistribution(
            ot.Normal(30.0, 7.5), 0.0, ot.TruncatedDistribution.LOWER
        )
        self.Ks.setName("Ks")
        self.Ks.setDescription(["Ks"])

        # Zv
        self.Zv = ot.Uniform(49.0, 51.0)
        self.Zv.setName("Zv")
        self.Zv.setDescription(["Zv (m)"])

        # Zm
        self.Zm = ot.Uniform(54.0, 56.0)
        self.Zm.setName("Zm")
        self.Zm.setDescription(["Zm (m)"])

        # B
        self.B = ot.Triangular(295.0, 300.0, 305.0)
        self.B.setName("B")
        self.B.setDescription(["B (m)"])

        # L
        self.L = ot.Triangular(4990.0, 5000.0, 5010.0)
        self.L.setName("L")
        self.L.setDescription(["L (m)"])

        # Zb
        self.Zb = ot.Triangular(55.0, 55.5, 56.0)
        self.Zb.setName("Zb")
        self.Zb.setDescription(["Zb (m)"])

        # Hd
        if distributionHdLow:
            self.Hd = ot.Uniform(2.0, 4.0)
        else:
            self.Hd = ot.Uniform(7.0, 9.0)
        self.Hd.setName("Hd")
        self.Hd.setDescription(["Hd (m)"])

        # Formula
        formula = "var alpha := (Zm - Zv) / L;"
        formula += "H := (Q / (Ks * B * sqrt(alpha)))^(3.0 / 5.0);"
        formula += "var Zc := H + Zv;"
        formula += "var Zd := Zb + Hd;"
        formula += "S := Zc - Zd;"
        formula += "if (S < 0)"
        formula += "    var Cost_Flooding := 0.2 - 0.8 * expm1(-1000 / S^4);"
        formula += "else"
        formula += "    Cost_Flooding := 1.0;"
        formula += "if (Hd < 8)"
        formula += "    var Cost_Dyke := 8.0 / 20.0;"
        formula += "else"
        formula += "    Cost_Dyke := Hd / 20.0;"
        formula += "C := Cost_Flooding + Cost_Dyke;"

        self.model = ot.SymbolicFunction(
            ["Q", "Ks", "Zv", "Zm", "B", "L", "Zb", "Hd"], ["H", "S", "C"], formula
        )

        self.distribution = ot.JointDistribution(
            [self.Q, self.Ks, self.Zv, self.Zm, self.B, self.L, self.Zb, self.Hd]
        )
        self.dim = self.distribution.getDimension()

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

    def getHeightModel(
        self,
        L=5000.0,
        B=300.0,
        Zb=55.5,
        Hd=3.0,
    ):
        """
        Return the height model with corresponding input distribution

        Parameters
        ----------
        L : float, optional
            The value of the river length. The default is 5000.0.

        B : float, optional
            The value of the river width. The default is 300.0.

        Zb : float, optional
            The level (altitude) of the bank. The default is 55.5.

        Hd : float, optional
            The height of the dyke. The default is 3.0.

        Returns
        -------
        heightInputDistribution : :class:`~openturns.Distribution`
            The joint input distribution of (Q, Ks, Zv, Zm).
        heightModel : :class:`~openturns.Function`
            The function with (Q, Ks, Zv, Zm) as input and (H) as output.
        """
        parametricModel = ot.ParametricFunction(
            self.model,
            [4, 5, 6, 7],
            [B, L, Zb, Hd],
        )
        heightModel = parametricModel.getMarginal(0)
        heightInputDistribution = self.distribution.getMarginal(range(4))
        return heightInputDistribution, heightModel
