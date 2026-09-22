.. _use-case-paleomagnetism:

The Paleomagnetism model
========================

This use case collects site-mean remanent magnetization directions as archived
by the MagIC database (earthref.org). Each direction is a unit vector of the
sphere :math:`\mathcal{S}^2`, stored through its Cartesian components
:math:`(x, y, z)`. For a tectonically stable volcanic site the dispersion
around the mean direction is isotropic and modelled by a von Mises-Fisher
distribution :math:`\mathrm{vMF}(\mu, \kappa)` with density
:math:`f(x) = C(\kappa) \exp(\kappa x^T \mu)`.

The dataset is a reduced stand-in (150 directions): the reference mean
direction is declination 35 degrees, inclination 50 degrees, with a
concentration :math:`\kappa = 18` typical of a well-defined site (see the
PmagPy Fisher-mean notebook for the same computation on real data).

Workflow
--------

- Fit :class:`~openturns.experimental.VonMisesFisherFactory` on the directions.
- Compare the estimated mean direction and concentration to the PmagPy
  Fisher mean of the corresponding MagIC site.

References
----------

- MagIC database, earthref.org (open site-mean direction compilations).
- PmagPy Fisher-mean notebook (open reference implementation).
- Fisher R.A., "Dispersion on a sphere", Proc. R. Soc. Lond. A 217, 1953.

Load the use case
-----------------

We can load this model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases.paleomagnetism import Paleomagnetism
    >>> m = Paleomagnetism()
    >>> # Load the 150 site-mean directions (unit vectors)
    >>> data = m.directions

API documentation
-----------------

.. currentmodule:: openturns.usecases.paleomagnetism

.. autoclass:: Paleomagnetism
    :noindex:
