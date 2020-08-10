.. _use-case-cantilever-beam:

The cantilever beam model
=========================

We consider a cantilever beam defined by its Young’s modulus :math:`E`, its length :math:`L` and its section modulus :math:`I`. One end of the cantilever beam is built in a wall and we apply a concentrated bending load :math:`F` at the other end of the beam, resulting in a deviation :math:`Y`. 

.. figure:: _static/beam.png
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
 
We can load this classical model from the use cases module as follows :

.. code-block:: python

    >>> from use_cases import use_case_cantilever_beam
    >>> # Load the use case cantilever_beam
    >>> cb = use_case_cantilever_beam.cantilever_beam()

which is an instance of the following class :

.. literalinclude:: ./examples/py_use_cases/use_cases/use_case_cantilever_beam.py
   :language: python

Examples based on this use case :
---------------------------------

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_use_cases/images/thumb/sphx_glr_plot_central_tendency-v2_thumb.png
     :alt: Create a copula

     :ref:`sphx_glr_auto_py_use_cases_plot_central_tendency-v2.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_use_cases/plot_central_tendency-v2

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_use_cases/images/thumb/sphx_glr_plot_chaos_cantilever_beam_integration-v2_thumb.png
     :alt: Create a copula

     :ref:`sphx_glr_auto_py_use_cases_plot_chaos_cantilever_beam_integration-v2.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_use_cases/plot_chaos_cantilever_beam_integration-v2

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_use_cases/images/thumb/sphx_glr_plot_estimate_probability_directional_sampling-v2_thumb.png
     :alt: Create a copula

     :ref:`sphx_glr_auto_py_use_cases_plot_estimate_probability_directional_sampling-v2.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_use_cases/plot_estimate_probability_directional_sampling-v2

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_use_cases/images/thumb/sphx_glr_plot_estimate_probability_form-v2_thumb.png
     :alt: Create a copula

     :ref:`sphx_glr_auto_py_use_cases_plot_estimate_probability_form-v2.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_use_cases/plot_estimate_probability_form-v2

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_use_cases/images/thumb/sphx_glr_plot_estimate_probability_importance_sampling-v2_thumb.png
     :alt: Create a copula

     :ref:`sphx_glr_auto_py_use_cases_plot_estimate_probability_importance_sampling-v2.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_use_cases/plot_estimate_probability_importance_sampling-v2

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_use_cases/images/thumb/sphx_glr_plot_estimate_probability_randomized_qmc-v2_thumb.png
     :alt: Create a copula

     :ref:`sphx_glr_auto_py_use_cases_plot_estimate_probability_randomized_qmc-v2.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_use_cases/plot_estimate_probability_randomized_qmc-v2

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_use_cases/images/thumb/sphx_glr_plot_kriging_beam_arbitrary_trend-v2_thumb.png
     :alt: Create a copula

     :ref:`sphx_glr_auto_py_use_cases_plot_kriging_beam_arbitrary_trend-v2.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_use_cases/plot_kriging_beam_arbitrary_trend-v2

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_use_cases/images/thumb/sphx_glr_plot_kriging_beam_trend-v2_thumb.png
     :alt: Create a copula

     :ref:`sphx_glr_auto_py_use_cases_plot_kriging_beam_trend-v2.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_use_cases/plot_kriging_beam_trend-v2

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_use_cases/images/thumb/sphx_glr_plot_kriging_cantilever_beam_hmat-v2_thumb.png
     :alt: Create a copula

     :ref:`sphx_glr_auto_py_use_cases_plot_kriging_cantilever_beam_hmat-v2.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_use_cases/plot_kriging_cantilever_beam_hmat-v2

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_use_cases/images/thumb/sphx_glr_plot_kriging_cantilever_beam-v2_thumb.png
     :alt: Create a copula

     :ref:`sphx_glr_auto_py_use_cases_plot_kriging_cantilever_beam-v2.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_use_cases/plot_kriging_cantilever_beam-v2
