"""
Use case: SatelliteAttitude
===========================
"""

import os
import openturns as ot


class SatelliteAttitude:
    """
    Spacecraft attitude uncertainty after a Wahba problem (reduced synthetic stand-in).

    Rotation matrices sampled from a Matrix-Fisher distribution, mimicking a
    set of star-tracker attitude estimates with published-type noise
    (10-60 arcsec) around the Wahba solution. Each row is a rotation stored
    as the 9 row-major components ``r11, ..., r33``. The reference parameter
    matrix is ``F = diag(8, 5, 2)``. The sample was drawn with
    :class:`~openturns.experimental.MatrixFisher` (seed 56789). This use case
    complements the attitude and control discipline of the
    :class:`~openturns.usecases.fire_satellite.FireSatelliteModel` use case.

    Attributes
    ----------
    rotations : :class:`~openturns.Sample`
        250 rotation matrices (flattened row-major 9-vectors).

    Examples
    --------
    >>> from openturns.usecases import satellite_attitude
    >>> data = satellite_attitude.SatelliteAttitude().rotations
    >>> print(data.getSize(), data.getDimension())
    250 9
    """

    def __init__(self):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        self.rotations = ot.Sample.ImportFromCSVFile(
            os.path.join(current_dir, "satellite_rotations.csv"), ","
        )

    def getReferenceF(self):
        """Reference parameter matrix."""
        return ot.SquareMatrix([[8.0, 0.0, 0.0], [0.0, 5.0, 0.0], [0.0, 0.0, 2.0]])
