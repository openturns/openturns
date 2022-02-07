.. _use-case-ackley:

The Ackley test case
====================

Introduction
------------

The Ackley test case is a real function defined in dimension :math:`d` where :math:`d` is an integer. 

The Ackley function is defined by the equation:

.. math::
   f(\mathbf{x}) = -a \exp\left(-b\sqrt{\frac{1}{d}\sum_{i=1}^d}x_i^2\right)-\exp\left(\frac{1}{d}\sum_{i=1}^d \cos(c x_i)\right)+a+\exp(1)
 

for any :math:`\mathbf{x} \in [-15,15]^d`. However, we consider the smaller interval :math:`[-4,4]^d` in this example, for visual purposes.

We use the dimension :math:`d=2` with the parameters :math:`a=20`, :math:`b=0.2`, :math:`c=2\pi`. 

The solution is 

.. math::
   \mathbf{x}^\star=(0,0,...,0)

where 

.. math::
   f_{min} = f(\mathbf{x}^\star) = 0.


References
----------

    * Adorio, E. P., & Diliman, U. P. MVF - Multivariate Test Functions Library in C for Unconstrained Global Optimization (2005). Retrieved June 2013, from http://http://www.geocities.ws/eadorio/mvf.pdf.

    * Molga, M., & Smutnicki, C. Test functions for optimization needs (2005). Retrieved June 2013, from http://www.zsd.ict.pwr.wroc.pl/files/docs/functions.pdf.

    * Back, T. (1996). Evolutionary algorithms in theory and practice: evolution strategies, evolutionary programming, genetic algorithms. Oxford University Press on Demand.

Load the use case
-----------------

We can load this model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases import ackley_function as ackley_function
    >>> # Load the Ackley model
    >>> am = ackley_function.AckleyModel()

API documentation
-----------------

See :class:`~openturns.usecases.ackley_function.AckleyModel`.


Examples based on this use case
-------------------------------

.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_numerical_methods/optimization/images/thumb/sphx_glr_plot_ego_thumb.png
     :alt: 

     :ref:`sphx_glr_auto_numerical_methods_optimization_plot_ego.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:
 
   /auto_numerical_methods/optimization/plot_ego
