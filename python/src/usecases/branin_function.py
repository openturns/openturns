"""
Use case : Branin test function
===============================
"""
from __future__ import print_function
import openturns as ot
import numpy as np


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

    def __init__(self):
        self.dim = 2
        self.trueNoiseFunction = 0.1

        self.branin = ot.SymbolicFunction(['x1', 'x2'],
                                      ['((x2-(5.1/(4*pi_^2))*x1^2+5*x1/pi_-6)^2+10*(1-1/8*pi_)*cos(x1)+10-54.8104)/51.9496',
                                       str(self.trueNoiseFunction)])
        self.transfo = ot.SymbolicFunction(['u1', 'u2'],
                                       ['15*u1-5', '15*u2'])

        self.model = ot.ComposedFunction(self.branin, self.transfo)

        # %%
        self.lowerbound = ot.Point([0.0] * self.dim)
        self.upperbound = ot.Point([1.0] * self.dim)

        self.objectiveFunction = self.model.getMarginal(0)

        # Minimas
        self.xexact1 = ot.Point([0.123895, 0.818329])
        self.xexact2 = ot.Point([0.542773, 0.151666])
        self.xexact3 = ot.Point([0.961652, 0.165000])
        self.xexact = ot.Sample([self.xexact1, self.xexact2, self.xexact3])
