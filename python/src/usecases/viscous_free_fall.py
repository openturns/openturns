"""
Use case : viscous free fall
============================
"""
from __future__ import print_function
import openturns as ot
from typing import Any, List
import numpy as np
from dataclasses import dataclass
from dataclasses import field

def AltiFunc(X):
    g  = 9.81
    z0 = X[0]
    v0 = X[1]
    m  = X[2]
    c  = X[3]
    tau = m / c
    vinf = - m * g / c
    t = np.array(vertices)
    z = z0 + vinf * t + tau * (v0 - vinf) * (1 - np.exp( - t / tau))
    z = np.maximum(z,0.)
    return [[zeta[0]] for zeta in z]

@dataclass
class ViscousFreeFall() :
    """Custom class for the viscous free fall
    """
    dim : int = 4 # number of inputs

    tmin : float = 0.0 # Minimum time
    tmax : float = 12.0 # Maximum time
    gridsize : int = 100 # Number of time steps
    mesh : Any = ot.IntervalMesher([gridsize-1]).build(ot.Interval(tmin, tmax))
    vertices : Any = mesh.getVertices()
    distZ0 : Any = ot.Uniform(100.0, 150.0)
    distV0 : Any = ot.Normal(55.0, 10.0)
    distM : Any = ot.Normal(80.0, 8.0)
    distC : Any = ot.Uniform(0.0, 30.0)
    distribution : Any= ot.ComposedDistribution([distZ0, distV0, distM, distC])
    dimension = distribution.getDimension()

    outputDimension : int = 1
    alti : Any = ot.PythonPointToFieldFunction(dim, mesh, outputDimension, AltiFunc)


