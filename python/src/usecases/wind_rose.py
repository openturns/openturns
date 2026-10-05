"""
Use case: WindRose
==================
"""

import os
import openturns as ot


class WindRose:
    """
    Hourly wind directions at 100 m (reduced synthetic stand-in).

    Circular data mimicking one year of daily wind headings derived from
    ERA5 reanalysis (Copernicus CDS) ``(u, v)`` components converted to
    compass headings. Each row is a heading in radians in ``[0, 2 pi)``.
    The reference wrapped-normal parameters are ``mu = 2.6`` rad and
    ``sigma^2 = 0.8``. The sample was drawn with
    :class:`~openturns.experimental.WrappedNormal` (seed 78901).

    Attributes
    ----------
    directions : :class:`~openturns.Sample`
        365 daily wind headings (radians).

    Examples
    --------
    >>> from openturns.usecases import wind_rose
    >>> data = wind_rose.WindRose().directions
    >>> print(data.getSize(), data.getDimension())
    365 1
    """

    def __init__(self):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        self.directions = ot.Sample.ImportFromCSVFile(
            os.path.join(current_dir, "wind_directions.csv"), ","
        )

    def getReferenceMu(self):
        """Reference mean heading (radians)."""
        return 2.6

    def getReferenceSigma2(self):
        """Reference circular variance."""
        return 0.8
