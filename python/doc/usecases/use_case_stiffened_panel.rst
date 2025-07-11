.. _use-case-stiffened-panel:

Stiffened panel buckling
========================


Introduction
------------

The following figure presents a stiffed panel subject to buckling on a military aircraft.

This use-case implements a simplified model of buckling for a stiffened panel (see [ko1994]_).

.. figure:: ../_static/stiffened_panel_illustration.jpg
    :align: center
    :alt: buckling illustration
    :width: 100%

    **Figure 1.** Buckling of a stiffened panel.


.. figure:: ../_static/stiffened_panel_simulation.png
    :align: center
    :alt: buckling simulation
    :width: 100%

    **Figure 2.** 3D simulation of buckling.


.. figure:: ../_static/stiffened_panel_description.png
    :align: center
    :alt: stiffened panel geometry
    :width: 100%

    **Figure 3.** Parameterization of the stiffened panel.


This test case is composed of ten random variables:

- :math:`E\sim\mathcal{TN}(\num{110.0e9}, \num{55.0e9}, \num{99.0e9}, \num{121.0e9})` : Young modulus (:math:`\unit{\Pa}`)

- :math:`nu\sim\mathcal{U}(0.3675, 0.3825)` : Poisson coefficient (-)

- :math:`h_c\sim\mathcal{U}(0.0285, 0.0315)` : Distance between the mean surface of the hat and the foot of the Stiffener (:math:`\unit{m}`)

- :math:`\ell\sim\mathcal{U}(0.04655, 0.05145)` : Length of the stiffener side (:math:`\unit{m}`)

- :math:`f_1\sim\mathcal{U}(0.0266, 0.0294)` : Width of the stiffener foot (:math:`\unit{m}`)

- :math:`f_2\sim\mathcal{U}(0.00627, 0.00693)` : Width of the stiffener hat (:math:`\unit{m}`)

- :math:`t\sim\mathcal{U}(\num{8.02e-5}, \num{8.181e-5})` : Thickness of the panel and the stiffener (:math:`\unit{m}`)

- :math:`a\sim\mathcal{U}(0.6039, 0.6161)` : Width of the panel (:math:`\unit{m}`)

- :math:`b_0\sim\mathcal{U}(0.04455, 0.04545)` : Distance between two stiffeners (:math:`\unit{m}`)

- :math:`p\sim\mathcal{U}(0.03762, 0.03838)` : Half-width of the stiffener (:math:`\unit{m}`)

The output of interest is:

- :math:`(N_{xy})_{cr}`: the critical shear force (:math:`\unit{N}`)

We assume that the input variables are independent except the :math:`f_1` and
:math:`f_2` for which we measure a Spearman correlation of :math:`\rho^S_{12}=-0.8`,
modelled using a :class:`~openturns.NormalCopula`.

The critical load :math:`(\tau_{xy})_{cr}` of a stiffened panel subject to shear load is:

.. math::
  (\tau_{xy})_{cr}=k_{xy}\frac{\pi^2D}{b_0^2t_s}

where:

- :math:`a` is the width of the panel;

- :math:`b_0` is the width between too consecutive stiffener feet;

- :math:`t_s` is the thickness of the panel main surface;

- :math:`E_s` is the Young modulus of the panel main surface;

- :math:`\nu_s` is the Poisson coefficient of the panel main surface;

- :math:`D` is the bending coefficient of the
  panel main surface:

.. math::
    D = \frac{E_st_s^3}{12(1-\nu_s^2)};

- :math:`k_{xy}` is the load factor associated to shear buckling. It is given as
  a function of :math:`\frac{b_0}{a}` through the empirical equation:

.. math::
    k_{xy} = 5.35 + 4 \left(\frac{b_0}{a}\right)^2.

It is more convenient to use the shear force :math:`N_{xy}` instead of the shear
stress component :math:`\tau_{xy}`. It leads to the equation:

.. math::
   N_{xy}=q_1+q_c

where :math:`q_1` abd :math:`q_c` are the shear fluxes in the panel main surface
and its stiffener. They are given by:

.. math::
   q_1=\tau_{xy}t_s=2G_sh_0t_s\frac{\partial^2w}{\partial x\partial y}

and:

.. math::
  q_c=\frac{G_ct_cp}{\ell} \left(h - 2h_0 + \frac{h_c}{2p}(f_1-f_2)\right) \frac{\partial^2w}{\partial x\partial y}

where:

- :math:`G_s` is the shear modulus of the panel main
  surface:

.. math::
    G_s = \frac{E_s}{2(1 + \nu_s)};

- :math:`\frac{\partial^2w}{\partial x\partial y}` is the torsion strain of the
  panel main surface;

- :math:`G_c` is the shear coefficient of the stiffener:

.. math::
    G_c = \frac{E_c}{2(1 + \nu_c)};

- :math:`t_c` is the thickness of the stiffener;

- :math:`h_c` is the distance between the mean surfaces of the stiffener hat and
  foot;

- :math:`h` is the distance between the mean surfaces of
  the stiffener hat and the panel main surface:

.. math::
    h = h_c+\frac{t_c + t_s}{2};

- :math:`f_1` is the width of the foot of the stiffener;

- :math:`f_2` is the width of the hat of the stiffener;

- :math:`p` is the half-widht of the stiffener;

- :math:`R` is the radius of the circular part of the stiffener;

- :math:`\theta` is the angle of the circular part of the stiffener;

- :math:`\ell` is the length of the stiffener flank;

- :math:`d=\frac{\ell-f_2}{2}-R\theta` is the half-lenght of the straight
  part of the side of the stiffener;

- :math:`A=\ell t_c` is the area of the section of an half-ondulation;

- :math:`\bar{A}` is the area of the section of
  the full panel (main surface and stiffener) bounded by :math:`p`:

.. math::
    \bar{A} = A + pt_s + \frac{1}{2}(f_1 - f_2)t_c

- :math:`h_0` is the distance between the mean surface of the panel main surface and the
  global geometric center of the panel:

.. math::
    h_0 = \frac{1}{2\bar{A}} \left(A(h_c+t_c+t_s)+\frac{1}{2}t_c(f_1-f_2)(t_c+t_s)\right).

It leads to:

.. math::
  N_{xy}=q_1(1+q_c/q_1) = \tau_{xy}t_s \left(1 + \frac{1}{4}\frac{G_ct_c}{G_st_s} \frac{2p(h-2h_0) - h_c(f_1-f_2)}{h_0\ell}\right)

and finally, :math:`(N_{xy})_{cr}` is given by:

.. math::
  (N_{xy})_{cr}=\left(5.35 + 4\left(\frac{b_0}{a}\right)^2\right)\left(\frac{\pi^2}{b_0^2}\frac{E_st_s^3}{12(1-\nu_s^2)}\right)\left(1+\frac{1}{4}\frac{G_ct_c}{G_st_s}\frac{2p(h-2h_0)-h_c(f_1-f_2)}{h_0\ell}\right)

For industrial constraints, the stiffener and the main surface are cut in the
same metal sheet, so :math:`E_c=E_s=E`, :math:`\nu_c=\nu_s=\nu`, :math:`t_c=t_s=t`.
The final expression of the critical shear force is then:

.. math::
  (N_{xy})_{cr}=\left(5.35 + 4\left(\frac{b_0}{a}\right)^2\right)\left(\frac{\pi^2}{b_0^2}\frac{Et^3}{12(1-\nu^2)}\right)\left(1+\frac{1}{4}\frac{2p(h-2h_0)-h_c(f_1-f_2)}{h_0\ell}\right)

with:

- :math:`A=\ell t`;

- :math:`\bar{A}=A+t\left(p+\frac{f_1-f_2}{2}\right)`;

- :math:`h_0=\frac{A(h_c+2t)+t^2(f_1-f_2)}{2\bar{A}}`;

- :math:`h=h_c+t`.


References
----------

* [ko1994]_

Load the use case
-----------------

We can load this model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases import stiffened_panel
    >>> sp = stiffened_panel.StiffenedPanel()
    >>> # Load the stiffened panel use case
    >>> model = sp.model()

API documentation
-----------------

.. currentmodule:: openturns.usecases.stiffened_panel

.. autoclass:: StiffenedPanel
    :noindex:

Examples based on this use case
-------------------------------

.. minigallery:: openturns.usecases.stiffened_panel.StiffenedPanel
