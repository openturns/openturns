"""
Use case : vertical deflection of a tube
========================================
"""
from __future__ import print_function
import openturns as ot
import numpy as np


class DeflectionTube():
    """
    Data class for the deflection of a tube model.


    Attributes
    ----------

    dim : Constant, the dimension of the problem.
          dim=6

    model : `SymbolicFunction`
            Model of the deflection.

    XF : `Normal` distribution
         ot.Normal(1, 0.1)

    XE : `Normal` distribution
         ot.Normal(200000, 2000)

    XL : `Dirac` distribution
         ot.Dirac(1.5)

    Xa : `Dirac` distribution
         ot.Dirac(1.0)

    XD : `Dirac` distribution
         ot.Dirac(0.8)

    Xd : `Dirac` distribution
         ot.Dirac(0.1)

    inputDistribution : `ComposedDistribution`
                        The joint distribution of the input parameters.

    Examples
    --------
    >>> from openturns.usecases import deflection_tube as deflection_tube
    >>> # Load the deflection tube model
    >>> dt = deflection_tube.DeflectionTube()
    """

    def __init__(self):
        self.dim = 6

        self.model = ot.SymbolicFunction(["F", "L", "a", "De", "di", "E"], ["g1", "g2", "g3"], "var I:=pi_*(De^4-di^4)/32; var b:=L-a; g1:=-F*a^2*(L-a)^2/(3*E*L*I); g2:=-F*b*(L^2-b^2)/(6*E*L*I); g3:=F*a*(L^2-a^2)/(6*E*L*I)")
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

        self.inputDistribution = ot.ComposedDistribution([self.XF, self.XL, self.Xa, self.XD, self.Xd, self.XE])
