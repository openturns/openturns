"""
Use case : logistic model
=========================
"""
from __future__ import print_function
import openturns as ot
from typing import Any, List
import numpy as np
from dataclasses import dataclass
from dataclasses import field


class Popu(ot.OpenTURNSPythonPointToFieldFunction):
    """
    """

    def __init__(self, t0=1790.0, tfinal=2000.0, nt=1000):
        grid = ot.RegularGrid(t0, (tfinal - t0) / (nt - 1), nt)
        super(Popu, self).__init__(3, grid, 1)
        self.setInputDescription(['y0', 'a', 'b'])
        self.setOutputDescription(['N'])
        self.ticks_ = [t[0] for t in grid.getVertices()]
        self.phi_ = ot.SymbolicFunction(['t', 'y', 'a', 'b'], ['a*y - b*y^2'])

    def _exec(self, X):
        y0 = X[0]
        a = X[1]
        b = X[2]
        phi_ab = ot.ParametricFunction(self.phi_, [2, 3], [a, b])
        phi_t = ot.ParametricFunction(phi_ab, [0], [0.0])
        solver = ot.RungeKutta(phi_t)
        initialState = [y0]
        values = solver.solve(initialState, self.ticks_)
        return values * [1.0e-6]


@dataclass
class LogisticModel():
    """Custom class for the logistic model
    """

    y0: float = 3.9e6
    a: float = 0.03134
    b: float = 1.5887e-10
    distY0: Any = ot.Normal(y0, 0.1 * y0)
    distA: Any = ot.Normal(a, 0.3 * a)
    distB: Any = ot.Normal(b, 0.3 * b)
    distX: Any = ot.ComposedDistribution([distY0, distA, distB])

    F: Any = Popu(1790.0, 2000.0, 1000)
    popu: Any = ot.PointToFieldFunction(F)
