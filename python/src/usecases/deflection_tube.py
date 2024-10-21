"""
Use case : vertical deflection of a tube
========================================
"""

import openturns as ot


class DeflectionTube:
    r"""
    Data class for the deflection of a tube model.


    Attributes
    ----------

    dim : int
        The dimension of the problem: dim=6

    model : :class:`~openturns.SymbolicFunction`
        Model of the deflection.
        The model has input dimension 6 and output dimension 3.
        More precisely, we have :math:`\vect{X} = (F, L, a, D, d, E)`
        and  :math:`Y = (y, \theta_L, \theta_R)`.

    XF : :class:`~openturns.Normal`
        F distribution
        `Normal(1, 0.1)`

    XE : :class:`~openturns.Normal`
        E distribution
        `Normal(200000, 2000)`

    XL : :class:`~openturns.Dirac`
        L distribution
        `Dirac(1.5)`

    Xa : :class:`~openturns.Dirac`
        a distribution
        `Dirac(1.0)`

    XD : :class:`~openturns.Dirac`
        D distribution
        `Dirac(0.8)`

    Xd : :class:`~openturns.Dirac`
        d distribution
        `Dirac(0.1)`

    inputDistribution : :class:`~openturns.JointDistribution`
        The joint distribution of the input parameters.

    Examples
    --------
    >>> from openturns.usecases import deflection_tube
    >>> # Load the deflection tube model
    >>> dt = deflection_tube.DeflectionTube()
    >>> print("Inputs:", dt.model.getInputDescription())
    Inputs: [F,L,a,De,di,E]
    >>> print("Outputs:", dt.model.getOutputDescription())
    [Deflection,Left angle,Right angle]
    """

    def __init__(self):
        self.dim = 6

        formula = "var I := pi_*(De^4 - di^4) / 32;"
        formula += "var b:=L - a; g1 := -F * a^2 * (L - a)^2 / (3 * E * L * I);"
        formula += "g2 := -F * b * (L^2 - b^2) / (6 * E * L * I);"
        formula += "g3 := F * a * (L^2 - a^2) / (6 * E * L * I)"
        self.model = ot.SymbolicFunction(
            ["F", "L", "a", "De", "di", "E"],
            ["g1", "g2", "g3"],
            formula,
        )
        self.model.setOutputDescription(["Deflection", "Left angle", "Right angle"])

        self.XF = ot.Normal(1, 0.1)
        self.XF.setDescription(["Force"])

        self.XE = ot.Normal(200000, 2000)
        self.XE.setDescription(["Young Modulus"])

        self.XL = ot.Dirac(1.5)
        self.XL.setDescription(["Length"])

        self.Xa = ot.Dirac(1.0)
        self.Xa.setDescription(["Location"])

        self.XD = ot.Dirac(0.8)
        self.XD.setDescription(["External diameter"])

        self.Xd = ot.Dirac(0.1)
        self.Xd.setDescription(["Internal diameter"])

        self.inputDistribution = ot.JointDistribution(
            [self.XF, self.XL, self.Xa, self.XD, self.Xd, self.XE]
        )
