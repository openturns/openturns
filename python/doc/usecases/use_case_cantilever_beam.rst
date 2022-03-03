.. _use-case-cantilever-beam:

The cantilever beam model
=========================

We are interested in the the vertical deviation of a diving board created by a child diver. We consider a child whose weight generates a force approximately equal to 300N (i.e. almost 30 kg). Because of the uncertainties in the weight of the person, we consider that the force is a random variable. The length of the diving board is between 2.5 m and 2.6 m. The Young modulus is uncertain and between 65 and 75 GPa, which corresponds to the fiberglass material, a material often used for diving boards. Uncertainties in the production of the material are taken into account in the Young modulus and the section modulus of the board. 

We consider a cantilever beam defined by its Young’s modulus :math:`E`, its length :math:`L` and its section modulus :math:`I`. One end of the cantilever beam is built in a wall and we apply a concentrated bending load :math:`F` at the other end of the beam, resulting in a deviation :math:`Y`. 

.. figure:: ../_static/beam.png
    :align: center
    :alt: beam geometry
    :width: 25%

    The beam geometry


**Inputs**

 * :math:`E` : Young modulus (Pa), Beta(:math:`\alpha = 0.9`, :math:`\beta = 3.5`, a = :math:`65.0 \times 10^9`, :math:`b = 75.0 \times 10^9`)
 * :math:`F` : Loading (N), Lognormal(:math:`\mu_F=300.0`, :math:`\sigma_F=30.0`, shift=0.0)
 * :math:`L` : Length of beam (m), Uniform(min=2.5, max= 2.6)
 * :math:`I` : Moment of inertia (:math:`m^4`), Beta(:math:`\alpha = 2.5`, :math:`\beta = 4.0`, :math:`a = 1.3 \times 10^{-7}`, :math:`b = 1.7 \times 10^{-7}`).

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
    Y \ge 0.30 (m) 

Load the use case
-----------------
 
We can load this classical model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases import cantilever_beam
    >>> # Load the cantilever beam example
    >>> cb = cantilever_beam.CantileverBeam()

API documentation
-----------------

See :class:`~openturns.usecases.cantilever_beam.CantileverBeam`.

Examples based on this use case
-------------------------------

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_reliability_sensitivity/central_dispersion/images/thumb/sphx_glr_plot_central_tendency_thumb.png
     :alt:

     :ref:`sphx_glr_auto_reliability_sensitivity_central_dispersion_plot_central_tendency.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_reliability_sensitivity/central_dispersion/plot_central_tendency


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_meta_modeling/polynomial_chaos_metamodel/images/thumb/sphx_glr_plot_chaos_cantilever_beam_integration_thumb.png
     :alt:

     :ref:`sphx_glr_auto_meta_modeling_polynomial_chaos_metamodel_plot_chaos_cantilever_beam_integration.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_meta_modeling/polynomial_chaos_metamodel/plot_chaos_cantilever_beam_integration



.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_reliability_sensitivity/reliability/images/thumb/sphx_glr_plot_estimate_probability_directional_sampling_thumb.png
     :alt:

     :ref:`sphx_glr_auto_reliability_sensitivity_reliability_plot_estimate_probability_directional_sampling.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_reliability_sensitivity/reliability/plot_estimate_probability_directional_sampling



.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_reliability_sensitivity/reliability/images/thumb/sphx_glr_plot_estimate_probability_form_thumb.png
     :alt:

     :ref:`sphx_glr_auto_reliability_sensitivity_reliability_plot_estimate_probability_form.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_reliability_sensitivity/reliability/plot_estimate_probability_form


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_reliability_sensitivity/reliability/images/thumb/sphx_glr_plot_estimate_probability_importance_sampling_thumb.png
     :alt:

     :ref:`sphx_glr_auto_reliability_sensitivity_reliability_plot_estimate_probability_importance_sampling.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_reliability_sensitivity/reliability/plot_estimate_probability_importance_sampling


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_reliability_sensitivity/reliability/images/thumb/sphx_glr_plot_estimate_probability_randomized_qmc_thumb.png
     :alt:

     :ref:`sphx_glr_auto_reliability_sensitivity_reliability_plot_estimate_probability_randomized_qmc.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_reliability_sensitivity/reliability/plot_estimate_probability_randomized_qmc


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_meta_modeling/kriging_metamodel/images/thumb/sphx_glr_plot_kriging_beam_arbitrary_trend_thumb.png
     :alt:

     :ref:`sphx_glr_auto_meta_modeling_kriging_metamodel_plot_kriging_beam_arbitrary_trend.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_meta_modeling/kriging_metamodel/plot_kriging_beam_arbitrary_trend


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_meta_modeling/kriging_metamodel/images/thumb/sphx_glr_plot_kriging_beam_trend_thumb.png
     :alt:

     :ref:`sphx_glr_auto_meta_modeling_kriging_metamodel_plot_kriging_beam_trend.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_meta_modeling/kriging_metamodel/plot_kriging_beam_trend


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_meta_modeling/kriging_metamodel/images/thumb/sphx_glr_plot_kriging_cantilever_beam_thumb.png
     :alt:

     :ref:`sphx_glr_auto_meta_modeling_kriging_metamodel_plot_kriging_cantilever_beam.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_meta_modeling/kriging_metamodel/plot_kriging_cantilever_beam
