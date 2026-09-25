.. _use-case-wind-rose:

The Wind Rose model
===================

This use case collects wind headings at 100 m for wind-energy siting studies.
The headings derive from ERA5 reanalysis (Copernicus CDS, hourly) ``(u, v)``
components converted to compass headings. Each row is a heading in radians in
``[0, 2 pi)``. The headings are modelled by a wrapped normal distribution.

The dataset is a reduced stand-in (365 daily headings): the reference
parameters are ``mu = 2.6`` rad and ``sigma^2 = 0.8``.

Workflow
--------

- Fit :class:`~openturns.experimental.WrappedNormalFactory` per season.
- Run the Rayleigh test against the uniform distribution and forecast the
  modal direction for turbine yaw alignment.

References
----------

- ERA5 reanalysis, Copernicus Climate Data Store (hourly ``(u, v)`` fields).

Load the use case
-----------------

We can load this model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases.wind_rose import WindRose
    >>> m = WindRose()
    >>> # Load the 365 daily wind headings (radians in [0, 2 pi))
    >>> data = m.directions

API documentation
-----------------

.. currentmodule:: openturns.usecases.wind_rose

.. autoclass:: WindRose
    :noindex:
