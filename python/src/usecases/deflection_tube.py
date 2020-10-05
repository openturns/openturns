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
class DeflectionTube() :
    """Custom class for the cantilever beam example
    """
    dim : int = 6

    model : Any = ot.SymbolicFunction(["F","L","a","De","di","E"],["g1","g2","g3"],"var I:=pi_*(De^4-di^4)/32; var b:=L-a; g1:=-F*a^2*(L-a)^2/(3*E*L*I); g2:=-F*b*(L^2-b^2)/(6*E*L*I); g3:=F*a*(L^2-a^2)/(6*E*L*I)")
    model.setOutputDescription(["Deflection","Left angle","Right angle"])

    XF : Any = ot.Normal(1,0.1)
    XF.setDescription(["Force"])

    XE : Any =ot.Normal(200000,2000)
    XE.setDescription(["Young Modulus"])


    XL : Any = ot.Dirac(1.5)
    XL.setDescription(["Length"])

    Xa : Any = ot.Dirac(1.0)
    Xa.setDescription(["Location"])

    XD : Any = ot.Dirac(0.8)
    XD.setDescription(["External diameter"])

    Xd : Any = ot.Dirac(0.1)
    Xd.setDescription(["Internal diameter"])

    inputDistribution : Any = ot.ComposedDistribution([XF,XL,Xa,XD,Xd,XE])

if __name__ == "__main__" :
    dt = DeflectionTube()
    print(dt)
