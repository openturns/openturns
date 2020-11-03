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
class BraninModel():
    """
    Data class for the Branin test function.


    Attributes
    ----------

    dim : The dimension of the problem
          dim=2.

    trueNoiseFunction : Constant, small noise
                        :math:`\epsilon=0.1`.

    model : `ComposedFunction`, the Branin function and the noise.

    objectiveFunction : First marginal of the model.
                        The Branin function without noise.

    lowerbound : `Point` in dimension dim.
                 Default is 0.0 for each dimension.

    upperbound : `Point` in dimension dim.
                 Default is 1.0 for each dimension.

    xexact1 : `Point`
              First minima location :math:`[0.123895, 0.818329]`.

    xexact2 : `Point`
              Second minima location :math:`[0.542773, 0.151666]`.

    xexact3 : `Point`
              Third minima location :math:`[0.961652, 0.165000]`.

    xexact : `Sample`
             All global minimas location gathered.


    Examples
    --------
    >>> from openturns.usecases import branin_function as branin_function
    >>> # Load the Branin model
    >>> bm = branin_function.BraninModel()
    """

    dim: int = 2
    trueNoiseFunction: float = 0.1

    branin: Any = ot.SymbolicFunction(['x1', 'x2'],
                                      ['((x2-(5.1/(4*pi_^2))*x1^2+5*x1/pi_-6)^2+10*(1-1/8*pi_)*cos(x1)+10-54.8104)/51.9496',
                                       str(trueNoiseFunction)])
    transfo: Any = ot.SymbolicFunction(['u1', 'u2'],
                                       ['15*u1-5', '15*u2'])

    model: Any = ot.ComposedFunction(branin, transfo)

    # %%
    lowerbound = ot.Point([0.0] * dim)
    upperbound = ot.Point([1.0] * dim)

    objectiveFunction = model.getMarginal(0)

    # Minimas
    xexact1: Any = ot.Point([0.123895, 0.818329])
    xexact2: Any = ot.Point([0.542773, 0.151666])
    xexact3: Any = ot.Point([0.961652, 0.165000])
    xexact: Any = ot.Sample([xexact1, xexact2, xexact3])
