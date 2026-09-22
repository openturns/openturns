.. _use-case-satellite-attitude:

The Satellite Attitude model
============================

This use case quantifies the orientation uncertainty of a spacecraft after a
Wahba attitude-estimation problem. A set of star-tracker attitude estimates
(published-type noise, 10-60 arcsec) is modelled by a Matrix-Fisher
distribution on SO(3). Each row is a rotation stored as its 9 row-major
components ``r11, ..., r33``. The reference parameter matrix is
``F = diag(8, 5, 2)`` (250 rotations). This use case complements the attitude
and control discipline of the
:ref:`fire satellite use case <use-case-fire-satellite>`.

Workflow
--------

- Fit :class:`~openturns.experimental.MatrixFisherFactory` on the rotations.
- Compare the estimated polar mode with the Davenport q-method solution and
  propagate the dispersion to the residual control torque.

References
----------

- Wahba G., "A least squares estimate of satellite attitude", SIAM Review,
  1965.
- Davenport P.B., "A vector approach to the algebra of rotations with
  applications", NASA report X-546-65-437, 1965.

Load the use case
-----------------

We can load this model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases.satellite_attitude import SatelliteAttitude
    >>> m = SatelliteAttitude()
    >>> # Load the 250 rotation matrices (flattened row-major 9-vectors)
    >>> data = m.rotations

API documentation
-----------------

.. currentmodule:: openturns.usecases.satellite_attitude

.. autoclass:: SatelliteAttitude
    :noindex:
