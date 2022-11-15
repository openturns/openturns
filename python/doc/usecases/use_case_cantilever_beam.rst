.. _use-case-cantilever-beam:

The cantilever beam model
=========================

We are interested in the vertical deviation of a diving board created by a child diver.
We consider a child whose weight generates a force approximately equal to 300N (i.e. almost 30 kg).
Because of the uncertainties in the weight of the person, we consider that the force is a random variable.
The length of the diving board is between 2.5 m and 2.6 m.
The Young modulus is uncertain and between 65 and 75 GPa, which corresponds to
the fiberglass material, a material often used for diving boards.
Uncertainties in the production of the material are taken into account in the Young modulus and the section modulus of the board.

We consider a cantilever beam defined by its Young’s modulus :math:`E`, its length :math:`L` and its section modulus :math:`I`.
One end of the cantilever beam is built in a wall and we apply a concentrated bending load :math:`F` at the other end of the beam, resulting in a deviation :math:`Y`.

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


API documentation
-----------------

.. currentmodule:: openturns.usecases.cantilever_beam

.. autoclass:: CantileverBeam
    :noindex:

Examples based on this use case
-------------------------------

.. minigallery:: openturns.usecases.cantilever_beam.CantileverBeam

