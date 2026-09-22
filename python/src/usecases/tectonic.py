"""
Use case: Tectonic
==================
"""

import os
import openturns as ot


class Tectonic:
    """
    Tectonic site-mean directions with elliptical dispersion (reduced synthetic stand-in).

    Kent-distributed unit vectors on the sphere :math:`\\mathcal{S}^2`
    mimicking a paleomagnetic collection from a tectonized area, where the
    directional dispersion is markedly elliptical (same MagIC frame as the
    :class:`~openturns.usecases.paleomagnetism.Paleomagnetism` use case).
    Each row is a site-mean direction stored as Cartesian components
    ``(x, y, z)``. The reference parameters are :math:`\\kappa = 12` and
    :math:`\\beta = 3` with identity orientation. The sample was drawn with
    :class:`~openturns.experimental.Kent` (seed 23456).

    Attributes
    ----------
    directions : :class:`~openturns.Sample`
        150 site-mean directions (unit vectors).

    Examples
    --------
    >>> from openturns.usecases import tectonic
    >>> data = tectonic.Tectonic().directions
    >>> print(data.getSize(), data.getDimension())
    150 3
    """

    def __init__(self):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        self.directions = ot.Sample.ImportFromCSVFile(
            os.path.join(current_dir, "tectonic_directions.csv"), ","
        )

    def getReferenceKappa(self):
        """Reference concentration."""
        return 12.0

    def getReferenceBeta(self):
        """Reference ovalness."""
        return 3.0
