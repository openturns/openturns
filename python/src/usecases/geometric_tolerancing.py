"""
Use case: GeometricTolerancing
=============================
"""

import math
import openturns as ot
import openturns.experimental as otexp


class GeometricTolerancing:
    """
    Form-defect tolerancing of a machined plate (fully synthetic).

    The plate is the unit square meshed with a 4x4 grid. The form defect is
    the out-of-plane field ``z = 0.05 sin(2 pi x) sin(2 pi y)`` transported
    by :class:`~openturns.experimental.PushForwardOverMesh`, while the probe
    location is uniform over the plate
    (:class:`~openturns.UniformOverMesh`). A part is non-conforming when the
    local defect amplitude exceeds the ``tolerance`` (default 0.03). No
    external data is required.

    Attributes
    ----------
    tolerance : float
        Conformity threshold on the defect amplitude.

    Examples
    --------
    >>> from openturns.usecases import geometric_tolerancing
    >>> m = geometric_tolerancing.GeometricTolerancing()
    >>> print(m.plate.getSimplicesNumber())
    32
    """

    def __init__(self, tolerance=0.03):
        mesher = ot.IntervalMesher([4, 4])
        self.plate = mesher.build(ot.Interval([0.0, 0.0], [1.0, 1.0]))
        self.tolerance = tolerance
        vertices = self.plate.getVertices()
        values = ot.Sample(vertices.getSize(), 3)
        for i in range(vertices.getSize()):
            x = vertices[i, 0]
            y = vertices[i, 1]
            values[i, 0] = x
            values[i, 1] = y
            values[i, 2] = 0.05 * math.sin(2.0 * math.pi * x) * math.sin(2.0 * math.pi * y)
        self.defect = otexp.PushForwardOverMesh(ot.Field(self.plate, values))
        self.location = ot.UniformOverMesh(self.plate)

    def nonConformityProbability(self, size=20000):
        """Monte Carlo estimate of P(|defect| > tolerance)."""
        sample = self.defect.getSample(size)
        count = sum(1 for i in range(size) if abs(sample[i, 2]) > self.tolerance)
        return count / size
