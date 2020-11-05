.. _use-case-ishigami:

The Ishigami function
=====================

The Ishigami function of Ishigami & Homma (1990) is  recurrent test case for sensitivity analysis methods and uncertainty.

Let :math:`a=7` and :math:`b=0.1` (see Crestaux et al. (2007) and Marrel et al. (2009)). We consider the function 

.. math::
   g(X_1,X_2,X_3) = \sin(X_1)+a \sin (X_2)^2 + b X_3^4 \sin(X_1)


for any :math:`X_1,X_2,X_3\in[-\pi,\pi]` 

We assume that the random variables :math:`X_1,X_2,X_3` are independent and have the uniform marginal distribution in the interval from :math:`-\pi` to :math:`\pi`:

.. math::
   X_1,X_2,X_3\sim \mathcal{U}(-\pi,\pi).


Analysis 
--------

The expectation and the variance of :math:`Y` are 

.. math::
    E(Y)  = \frac{a}{2}
 

and 

.. math::
    V(Y) = \frac{1}{2} +  \frac{a^2}{8} +  \frac{b^2 \pi^8}{18} +  \frac{b\pi^4}{5}.
 

The Sobol' decomposition variances are

.. math::
    V_1     = \frac{1}{2} \left(1 + b\frac{\pi^4}{5} \right)^2, \qquad 
    V_2     = \frac{a^2}{8}, \qquad 
    V_{1,3} = b^2 \pi^8 \frac{8}{225}
 

and :math:`V_3=V_{1,2} = V_{2,3}=V_{1,2,3} = 0`.

This leads to the following first order Sobol' indices:

.. math::
    S_1 = \frac{V_1}{V(Y)}, \qquad S_2 = \frac{V_2}{V(Y)}, \qquad S_3 = 0,
 

and the following total order indices:

.. math::
    ST_1 = \frac{V_1+V_{1,3}}{V(Y)}, \qquad ST_2 = S_2, \qquad S_3 = \frac{V_{1,3}}{V(Y)}.
 

The third variable :math:`X_3` has no effect at first order (because :math:`X_3^4` it is multiplied by :math:`\sin(X_1)`), but has a total effet because of the interactions with :math:`X_1`. On the other hand, the second variable :math:`X_2` has no interactions which implies that the first order indice is equal to the total order indice for this input variable.

References
----------

    * Ishigami, T., & Homma, T. (1990, December). An importance quantification technique in uncertainty analysis for computer models. In Uncertainty Modeling and Analysis, 1990. Proceedings., First International Symposium on (pp. 398-403). IEEE.

    * Sobol', I. M., & Levitan, Y. L. (1999). On the use of variance reducing multipliers in Monte Carlo computations of a global sensitivity index. Computer Physics Communications, 117(1), 52-61.

    * Saltelli, A., Chan, K., & Scott, E. M. (Eds.). (2000). Sensitivity analysis (Vol. 134). New York: Wiley.

    * Crestaux, T., Martinez, J.-M., Le Maitre, O., & Lafitte, O. (2007). Polynomial chaos expansion for uncertainties quantification and sensitivity analysis. SAMO 2007, http://samo2007.chem.elte.hu/lectures/Crestaux.pdf.

Load the use case
-----------------

We can load this classical model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases import ishigami_function as ishigami_function
    >>> # Load the Ishigami use case
    >>> im = ishigami_function.IshigamiModel()

API documentation
-----------------

See :class:`~openturns.usecases.ishigami_function.IshigamiModel`.

Examples based on this use case
-------------------------------


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_meta_modeling/polynomial_chaos_metamodel/images/thumb/sphx_glr_plot_chaos_ishigami_thumb.png
     :alt:

     :ref:`sphx_glr_auto_meta_modeling_polynomial_chaos_metamodel_plot_chaos_ishigami.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:
 
   /auto_meta_modeling/polynomial_chaos_metamodel/plot_chaos_ishigami

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_reliability_sensitivity/sensitivity_analysis/images/thumb/sphx_glr_plot_sensitivity_fast_thumb.png
     :alt:

     :ref:`sphx_glr_auto_reliability_sensitivity_sensitivity_analysis_plot_sensitivity_fast.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:
 
   /auto_reliability_sensitivity/sensitivity_analysis/plot_sensitivity_fast

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_data_analysis/manage_data_and_samples/images/thumb/sphx_glr_plot_sample_correlation_thumb.png
     :alt:

     :ref:`sphx_glr_auto_data_analysis_manage_data_and_samples_plot_sample_correlation.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:
 
   /auto_data_analysis/manage_data_and_samples/plot_sample_correlation

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_meta_modeling/polynomial_chaos_metamodel/images/thumb/sphx_glr_plot_chaos_draw_validation_thumb.png
     :alt:

     :ref:`sphx_glr_auto_meta_modeling_polynomial_chaos_metamodel_plot_chaos_draw_validation.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:
 
   /auto_meta_modeling/polynomial_chaos_metamodel/plot_chaos_draw_validation

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_meta_modeling/polynomial_chaos_metamodel/images/thumb/sphx_glr_plot_chaos_ishigami_grouped_indices_thumb.png
     :alt:

     :ref:`sphx_glr_auto_meta_modeling_polynomial_chaos_metamodel_plot_chaos_ishigami_grouped_indices.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:
 
   /auto_meta_modeling/polynomial_chaos_metamodel/plot_chaos_ishigami_grouped_indices

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_reliability_sensitivity/sensitivity_analysis/images/thumb/sphx_glr_plot_sensitivity_sobol_thumb.png
     :alt:

     :ref:`sphx_glr_auto_reliability_sensitivity_sensitivity_analysis_plot_sensitivity_sobol.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:
 
   /auto_reliability_sensitivity/sensitivity_analysis/plot_sensitivity_sobol
