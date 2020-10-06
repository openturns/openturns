"""
Use case : Branin test function
===============================
"""
from __future__ import print_function
import openturns as ot
from typing import Any, List
import numpy as np
from dataclasses import dataclass
from dataclasses import field

@dataclass
class BraninModel() :
    """Custom class for the Branin test function
    """
    dim : int = 2
    trueNoiseFunction : float = 0.1

    branin : Any = ot.SymbolicFunction(['x1', 'x2'],
                             ['((x2-(5.1/(4*pi_^2))*x1^2+5*x1/pi_-6)^2+10*(1-1/8*pi_)*cos(x1)+10-54.8104)/51.9496',
                              str(trueNoiseFunction)])
    transfo : Any = ot.SymbolicFunction(['u1', 'u2'],
                              ['15*u1-5', '15*u2'])
   
    model : Any = ot.ComposedFunction(branin, transfo) 

    # %%
    lowerbound = ot.Point([0.0] * dim)
    upperbound = ot.Point([1.0] * dim)

    objectiveFunction = model.getMarginal(0)

    # Minimas
    xexact1 : Any = ot.Point([0.123895,0.818329])
    xexact2 : Any = ot.Point([0.542773,0.151666])
    xexact3 : Any = ot.Point([0.961652,0.165000])
    xexact : Any = ot.Sample([xexact1, xexact2, xexact3])


