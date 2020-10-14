.. _use-case-deflection-tube:

Vertical deflection of a tube
=============================


Description
-----------

We consider the deflection of a tube under a vertical stress.

.. figure:: ../_static/simply_supported_beam.png
    :align: center
    :alt: simply supported beam
    :width: 50%

    A simply supported beam

The parameters of the model are:

* F : the strength,
* L : the length of the tube,
* a : position of the force,
* D : external diameter of the tube,
* d : internal diameter of the tube,
* E : Young modulus.

The following figure presents the internal and external diameter of the tube:

.. figure:: ../_static/tube-diameters.png
    :align: center
    :alt: tube diameters
    :width: 50%

    Tube diameters

The area moment of inertia of the cross section about the neutral axis of a round tube (i.e. perpendicular to the section) with external and internal diameters :math:`D` and :math:`d` are:

.. math::

   I = \frac{\pi (D^4-d^4)}{32}.


The vertical deflection at point :math:`x=a` is:

.. math::
   g_1(X) = - F \frac{a^2 (L-a)^2}{3 E L I},


where :math:`X=(F,L,a,D,d,E)`. 
The angle of the tube at the left end is:

.. math::
   g_2(X) = - F \frac{b (L^2-b^2)}{6 E L I},


and the angle of the tube at the right end is:

.. math::
   g_3(X) = F \frac{a (L^2-a^2)}{6 E L I}.

The following table presents the distributions of the random variables. These variables are assumed to be independent.

======== ==========================
Variable Distribution
======== ==========================
F        Normal(1,0.1)
L        Normal(1.5,0.01)
a        Uniform(0.7,1.2)
D        Triangular(0.75,0.8,0.85)
d        Triangular(0.09,0.1,0.11)
E        Normal(200000,2000)
======== ==========================


References
----------

    * Deflection of beams by Russ Elliott. http://www.clag.org.uk/beam.html
    * https://upload.wikimedia.org/wikipedia/commons/f/ff/Simple_beam_with_offset_load.svg
    * https://en.wikipedia.org/wiki/Deflection_(engineering)
    * https://mechanicalc.com/reference/beam-deflection-tables
    * https://en.wikipedia.org/wiki/Second_moment_of_area
    * Shigley's Mechanical Engineering Design (9th Edition), Richard G. Budynas, J. Keith Nisbettn, McGraw Hill (2011)
    * Mechanics of Materials (7th Edition), James M. Gere, Barry J. Goodno, Cengage Learning (2009)
    * Statics and Mechanics of Materials (5th Edition), Ferdinand Beer, E. Russell Johnston, Jr., John DeWolf, David Mazurek. Mc Graw Hill (2011) Chapter 15: deflection of beams.

Load the use case
-----------------

We can load this classical model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases import deflection_tube as deflection_tube 
    >>> # Load the tube deflection model
    >>> dt = deflection_tube.DeflectionTube()

Examples based on this use case :
---------------------------------


.. raw:: html                                                             

    <div class="sphx-glr-thumbcontainer" tooltip="">

.. only:: html

 .. figure:: /auto_py_calibration/least_squares_and_gaussian_calibration/images/thumb/sphx_glr_plot_calibration_deflection_tube_thumb.png
     :alt:

     :ref:`sphx_glr_auto_py_calibration_least_squares_and_gaussian_calibration_plot_calibration_deflection_tube.py`

.. raw:: html

    </div>

.. toctree::
   :hidden:

   /auto_py_calibration/least_squares_and_gaussian_calibration/plot_calibration_deflection_tube

