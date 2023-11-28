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


    dim : The dimension of the problem
          dim=4.

    outputDimension : The output dimension of the problem
                      outputDimension=1.

    tmin : Constant
           Minimum time, tmin = 0.0

    tmax : Constant
           Maximum time, tmax = 12.0

    gridsize : Constant
               Number of time steps, gridsize = 100.

    mesh : `IntervalMesher`

    vertices : Vertices of the mesh

    distZ0 : `Uniform` distribution of the initial altitude
             ot.Uniform(100.0, 150.0)

    distV0 : `Normal` distribution of the initial speed
             ot.Normal(55.0, 10.0)

    distM : `Normal` distribution of the mass
            ot.Normal(80.0, 8.0)

    distC : `Uniform` distribution of the drag
            ot.Uniform(0.0, 30.0)

    distribution : `JointDistribution`
                   The joint distribution of the input parameters.

    model : `PythonPointToFieldFunction`, the exact solution of the fall
           ot.PythonPointToFieldFunction(dim, mesh, outputDimension, AltiFunc)


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
