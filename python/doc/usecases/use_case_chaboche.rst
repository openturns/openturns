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

Parameters to calibrate
-----------------------

The vector of parameters to calibrate is:

.. math::
   \theta = (R,C,\gamma).


We set:

- :math:`R = 750\times 10^6`,
- :math:`C = 2750\times 10^6`,
- :math:`\gamma = 10`.

Observations
------------

In order to create a calibration problem, we make the hypothesis that the strain has the following distribution:

.. math::
   \epsilon \sim Uniform(0,0.07).


Moreover, we consider a gaussian noise on the observed constraint:

.. math::
   \epsilon_\sigma \sim \mathcal{N} \left(0,10\times 10^6\right)


and we make the hypothesis that the observation errors are independent.
We set the number of observations to:

.. math::
   n = 100.


We generate a Monte-Carlo samplg with size :math:`n`:

.. math::
   \sigma_i = G(\epsilon_i,R,C,\gamma) + (\epsilon_\sigma)_i,


for :math:`i = 1,..., n`.
The observations are the pairs :math:`\{(\epsilon_i,\sigma_i)\}_{i=1,...,n}`, i.e. each observation is a couple made of the strain and the corresponding stress.

Variables
---------

In the particular situation where we want to calibrate this model, the following list presents which variables are observed input variables, input calibrated variables and observed output variables.

- :math:`\epsilon`: Input. Observed.
- :math:`R`, :math:`C`, :math:`\gamma` : Inputs. Calibrated.
- :math:`\sigma`: Output. Observed.

Thanks to
---------

- Antoine Dumas, Phimeca


References
----------

- J. Lemaitre and J. L. Chaboche (2002) "Mechanics of solid materials" Cambridge University Press.

Load the use case
-----------------

We can load this model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases import chaboche_model as chaboche_model
    >>> # Load the Chaboche use case
    >>> cm = chaboche_model.ChabocheModel()

API documentation
-----------------

See :class:`~openturns.usecases.chaboche_model.ChabocheModel`.

Examples based on this use case
-------------------------------


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_calibration/least_squares_and_gaussian_calibration/images/thumb/sphx_glr_plot_calibration_chaboche_thumb.png
     :alt:

     :ref:`sphx_glr_auto_calibration_least_squares_and_gaussian_calibration_plot_calibration_chaboche.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_calibration/least_squares_and_gaussian_calibration/plot_calibration_chaboche





