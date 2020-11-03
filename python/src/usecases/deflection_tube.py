"""
Use case : vertical deflection of a tube
========================================
"""
from __future__ import print_function
import openturns as ot
from typing import Any, List
import numpy as np
from dataclasses import dataclass
from dataclasses import field


@dataclass
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
    dim: int = 6

    model: Any = ot.SymbolicFunction(["F", "L", "a", "De", "di", "E"], ["g1", "g2", "g3"], "var I:=pi_*(De^4-di^4)/32; var b:=L-a; g1:=-F*a^2*(L-a)^2/(3*E*L*I); g2:=-F*b*(L^2-b^2)/(6*E*L*I); g3:=F*a*(L^2-a^2)/(6*E*L*I)")
    model.setOutputDescription(["Deflection", "Left angle", "Right angle"])

    XF: Any = ot.Normal(1, 0.1)
    XF.setDescription(["Force"])

    XE: Any = ot.Normal(200000, 2000)
    XE.setDescription(["Young Modulus"])

    XL: Any = ot.Dirac(1.5)
    XL.setDescription(["Length"])

    Xa: Any = ot.Dirac(1.0)
    Xa.setDescription(["Location"])

    XD: Any = ot.Dirac(0.8)
    XD.setDescription(["External diameter"])

    Xd: Any = ot.Dirac(0.1)
    Xd.setDescription(["Internal diameter"])

    inputDistribution: Any = ot.ComposedDistribution([XF, XL, Xa, XD, Xd, XE])
