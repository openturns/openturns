.. _use-case-chaboche:

The Chaboche mechanical model
=============================

Deterministic model
-------------------

The Chaboche mechanical law predicts the stress depending on the strain:

.. math::
   \sigma = G(\epsilon,R,C,\gamma) = R + \frac{C}{\gamma} (1-\exp(-\gamma\epsilon))


where:

- :math:`\epsilon` is the strain,
- :math:`\sigma` is the stress (Pa),
- :math:`R`, :math:`C`, :math:`\gamma` are the parameters.

The variables have the following distributions and are supposed to be independent.

 ================  ===========================================================
 Random var.       Distribution
 ================  ===========================================================
 :math:`R`         Lognormal (:math:`\mu = 750` MPa, :math:`\sigma = 11` MPa)
 :math:`C`         Normal (:math:`\mu = 2750` MPa, :math:`\sigma = 250` MPa)
 :math:`\gamma`    Normal (:math:`\mu = 10`, :math:`\sigma = 2`)
 :math:`\epsilon`  Uniform(a=0, b=0.07).
 ================  ===========================================================

Thanks to
---------

- Antoine Dumas, Phimeca


References
----------

- J. Lemaitre and J. L. Chaboche (2002) "Mechanics of solid materials" Cambridge University Press.


API documentation
-----------------

.. currentmodule:: openturns.usecases.chaboche_model

.. autoclass:: ChabocheModel
    :noindex:

Examples based on this use case
-------------------------------

.. minigallery:: openturns.usecases.chaboche_model.ChabocheModel

