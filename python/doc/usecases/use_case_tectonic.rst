.. _use-case-tectonic:

The Tectonic model
==================

This use case collects site-mean directions from a tectonized area, in the
same MagIC frame (earthref.org) as the
:ref:`paleomagnetism use case <use-case-paleomagnetism>`. Here the directional
dispersion is markedly elliptical and modelled by a Kent distribution
:math:`\mathrm{Kent}(\kappa, \beta, \Gamma)` [kent1982]_ with density
:math:`f(x) \propto \exp(\kappa x^T \gamma_1
+ \beta ((x^T \gamma_2)^2 - (x^T \gamma_3)^2))` on :math:`\mathcal{S}^2`.

The dataset is a reduced stand-in (150 directions): the reference parameters
are :math:`\kappa = 12`, :math:`\beta = 3` with identity orientation.

Workflow
--------

- Fit :class:`~openturns.experimental.KentFactory` on the directions.
- Compare :math:`\kappa` and :math:`\beta` to the Directional package fit and
  draw the 95% confidence oval.

References
----------

- MagIC database, earthref.org (sites with marked ellipticity).
- Directional R/C++ package (Kent reference implementation).

Load the use case
-----------------

We can load this model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases.tectonic import Tectonic
    >>> m = Tectonic()
    >>> # Load the 150 site-mean directions (unit vectors)
    >>> data = m.directions

API documentation
-----------------

.. currentmodule:: openturns.usecases.tectonic

.. autoclass:: Tectonic
    :noindex:
