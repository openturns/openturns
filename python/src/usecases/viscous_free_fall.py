"""
Use case : viscous free fall
============================
"""

import openturns as ot
import numpy as np


def AltiFunc(X):
    g = 9.81
    z0 = X[0]
    v0 = X[1]
    m = X[2]
    c = X[3]
    tau = m / c
    vinf = -m * g / c
    t = np.array(AltiFunc.vertices)
    z = z0 + vinf * t + tau * (v0 - vinf) * (1 - np.exp(-t / tau))
    z = np.maximum(z, 0.0)
    return [[zeta[0]] for zeta in z]


class ViscousFreeFall:
    """
    Data class for the viscous free fall.


    Attributes
    ----------

    dim : int
        The dimension of the problem, dim=4.

    outputDimension : int
        The output dimension of the problem, outputDimension=1.

    tmin : float
        Minimum time, tmin = 0.0

    tmax : float
        Maximum time, tmax = 12.0

    gridsize : int
        Number of time steps, gridsize = 100.

    mesh : :class:`~openturns.Mesh`
        Time grid

    vertices : :class:`~openturns.Sample`
        Vertices of the mesh

    distZ0 : :class:`~openturns.Uniform`
        Distribution of the initial altitude: `Uniform(100.0, 150.0)`

    distV0 : :class:`~openturns.Normal`
        Distribution of the initial speed: `Normal(55.0, 10.0)`

    distM : :class:`~openturns.Normal`
        Distribution of the mass: `Normal(80.0, 8.0)`

    distC : :class:`~openturns.Uniform`
        Distribution of the drag: `Uniform(0.0, 30.0)`

    distribution : :class:`~openturns.JointDistribution`
        The joint distribution of the input parameters.

    model : :class:`~openturns.PythonPointToFieldFunction`
        The exact solution of the fall


    Examples
    --------
    >>> from openturns.usecases import viscous_free_fall
    >>> # Load the viscous free fall example
    >>> vff = viscous_free_fall.ViscousFreeFall()
    """

    def __init__(self):
        self.dim = 4  # number of inputs
        self.outputDimension = 1  # dimension of the output

        self.tmin = 0.0  # Minimum time
        self.tmax = 12.0  # Maximum time
        self.gridsize = 100  # Number of time steps
        self.mesh = ot.IntervalMesher([self.gridsize - 1]).build(
            ot.Interval(self.tmin, self.tmax)
        )
        AltiFunc.vertices = self.mesh.getVertices()
        self.vertices = self.mesh.getVertices()

        # Marginals
        self.distZ0 = ot.Uniform(100.0, 150.0)
        self.distV0 = ot.Normal(55.0, 10.0)
        self.distM = ot.Normal(80.0, 8.0)
        self.distC = ot.Uniform(0.0, 30.0)

        # Joint distribution
        self.distribution = ot.JointDistribution(
            [self.distZ0, self.distV0, self.distM, self.distC]
        )
        description = ["z0", "v0", "m", "c"]
        self.distribution.setDescription(description)

        # Exact solution
        self.model = ot.PythonPointToFieldFunction(
            self.dim, self.mesh, self.outputDimension, AltiFunc
        )
        self.model.setInputDescription(description)
        self.model.setOutputDescription(["z"])
        self.alti = self.model  # deprecated
