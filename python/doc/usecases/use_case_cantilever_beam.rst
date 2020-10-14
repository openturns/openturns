.. _use-case-cantilever-beam:

The cantilever beam model
=========================

We consider a cantilever beam defined by its Young’s modulus :math:`E`, its length :math:`L` and its section modulus :math:`I`. One end of the cantilever beam is built in a wall and we apply a concentrated bending load :math:`F` at the other end of the beam, resulting in a deviation :math:`Y`. 

.. figure:: ../_static/beam.png
    :align: center 
    :alt: beam geometry
    :width: 100%

    caption: The beam geometry


**Inputs**

 * :math:`E` : Young modulus (Pa), Beta(:math:`\alpha = 0.9`, :math:`\beta = 2.27`, a = :math:`2.5\times 10^7`, :math:`b = 5\times 10^7`)
 * :math:`F` : Loading (N), Lognormal(:math:`\mu_F=30 \times 10^3`, :math:`\sigma_F=9\times 10^3`, shift=:math:`15\times 10^3`)
 * :math:`L` : Length of beam (cm), Uniform(min=250.0, max= 260.0)
 * :math:`I` : Moment of inertia (cm^4), Beta(:math:`\alpha = 2.5`, :math:`\beta = 1.5`, a = 310, b = 450).

In the previous table :math:`\mu_F=E(F)` and :math:`\sigma_F=\sqrt{V(F)}` are the mean and the standard deviation of :math:`F`.

We assume that the random variables E, F, L and I are dependent and associated with a gaussian copula which correlation matrix is :
 .. math::

    R = \begin{pmatrix}
        1 & 0 & 0 & 0 \\
        0 & 1 & 0 & 0 \\
        0 & 0 & 1 & -0.2 \\
        0 & 0 & -0.2 & 1
        \end{pmatrix}
 

In other words, we consider that the variables L and I are negatively correlated : when the length L increases, the moment of intertia I decreases.

**Output**

The vertical displacement at free end of the cantilever beam is:
 .. math::
    Y  = \dfrac{F\, L^3}{3 \, E \, I}

A typical event of interest is when the beam deviation is too large which is a failure :
 .. math::
    Y \ge 30 (cm) 

Load the use case
-----------------
 
We can load this classical model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases import cantilever_beam
    >>> # Load the cantilever beam example
    >>> cb = cantilever_beam.CantileverBeam()


Examples based on this use case :
---------------------------------

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_reliability_sensitivity/central_dispersion/images/thumb/sphx_glr_plot_central_tendency_thumb.png
     :alt:

     :ref:`sphx_glr_auto_py_reliability_sensitivity_central_dispersion_plot_central_tendency.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_reliability_sensitivity/central_dispersion/plot_central_tendency


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_meta_modeling/polynomial_chaos_metamodel/images/thumb/sphx_glr_plot_chaos_cantilever_beam_integration_thumb.png
     :alt:

     :ref:`sphx_glr_auto_py_meta_modeling_polynomial_chaos_metamodel_plot_chaos_cantilever_beam_integration.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_meta_modeling/polynomial_chaos_metamodel/plot_chaos_cantilever_beam_integration



.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_reliability_sensitivity/reliability/images/thumb/sphx_glr_plot_estimate_probability_directional_sampling_thumb.png
     :alt:

     :ref:`sphx_glr_auto_py_reliability_sensitivity_reliability_plot_estimate_probability_directional_sampling.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_reliability_sensitivity/reliability/plot_estimate_probability_directional_sampling



.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_reliability_sensitivity/reliability/images/thumb/sphx_glr_plot_estimate_probability_form_thumb.png
     :alt:

     :ref:`sphx_glr_auto_py_reliability_sensitivity_reliability_plot_estimate_probability_form.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_reliability_sensitivity/reliability/plot_estimate_probability_form


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_reliability_sensitivity/reliability/images/thumb/sphx_glr_plot_estimate_probability_importance_sampling_thumb.png
     :alt:

     :ref:`sphx_glr_auto_py_reliability_sensitivity_reliability_plot_estimate_probability_importance_sampling.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_reliability_sensitivity/reliability/plot_estimate_probability_importance_sampling


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_reliability_sensitivity/reliability/images/thumb/sphx_glr_plot_estimate_probability_randomized_qmc_thumb.png
     :alt:

     :ref:`sphx_glr_auto_py_reliability_sensitivity_reliability_plot_estimate_probability_randomized_qmc.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_reliability_sensitivity/reliability/plot_estimate_probability_randomized_qmc


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_meta_modeling/kriging_metamodel/images/thumb/sphx_glr_plot_kriging_beam_arbitrary_trend_thumb.png
     :alt:

     :ref:`sphx_glr_auto_py_meta_modeling_kriging_metamodel_plot_kriging_beam_arbitrary_trend.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_meta_modeling/kriging_metamodel/plot_kriging_beam_arbitrary_trend


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_meta_modeling/kriging_metamodel/images/thumb/sphx_glr_plot_kriging_beam_trend_thumb.png
     :alt:

     :ref:`sphx_glr_auto_py_meta_modeling_kriging_metamodel_plot_kriging_beam_trend.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_meta_modeling/kriging_metamodel/plot_kriging_beam_trend


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_meta_modeling/kriging_metamodel/images/thumb/sphx_glr_plot_kriging_cantilever_beam_hmat_thumb.png
     :alt: Create a copula

     :ref:`sphx_glr_auto_py_meta_modeling_kriging_metamodel_plot_kriging_cantilever_beam_hmat.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_meta_modeling/kriging_metamodel/plot_kriging_cantilever_beam_hmat


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_meta_modeling/kriging_metamodel/images/thumb/sphx_glr_plot_kriging_cantilever_beam_thumb.png
     :alt:

     :ref:`sphx_glr_auto_py_meta_modeling_kriging_metamodel_plot_kriging_cantilever_beam.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_meta_modeling/kriging_metamodel/plot_kriging_cantilever_beam
