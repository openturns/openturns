"""
Use case: Paleomagnetism
========================
"""

import math
import os
import openturns as ot


class Paleomagnetism:
    """
    Paleomagnetic site-mean directions (reduced synthetic stand-in).

    Fisher-distributed unit vectors on the sphere :math:`\\mathcal{S}^2`
    mimicking a paleomagnetic site collection as archived by the MagIC
    database (earthref.org): each row is a site-mean direction stored as
    Cartesian components ``(x, y, z)``. The reference direction is
    declination 35 degrees, inclination 50 degrees, with a concentration
    :math:`\\kappa = 18` typical of a well-defined volcanic site
    (see e.g. the PmagPy Fisher-mean notebook). The sample was drawn with
    :class:`~openturns.experimental.VonMisesFisher` (seed 12345).

    Attributes
    ----------
    directions : :class:`~openturns.Sample`
        150 site-mean directions (unit vectors).

    Examples
    --------
    >>> from openturns.usecases import paleomagnetism
    >>> data = paleomagnetism.Paleomagnetism().directions
    >>> print(data.getSize(), data.getDimension())
    150 3
    """

    def __init__(self):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        self.directions = ot.Sample.ImportFromCSVFile(
            os.path.join(current_dir, "paleomagnetism_directions.csv"), ","
        )

    def getReferenceMeanDirection(self):
        """Reference Fisher mean direction (dec 35 deg, inc 50 deg)."""
        dec = math.radians(35.0)
        inc = math.radians(50.0)
        return [
            math.cos(inc) * math.cos(dec),
            math.cos(inc) * math.sin(dec),
            math.sin(inc),
        ]

    def getReferenceKappa(self):
        """Reference concentration."""
        return 18.0
