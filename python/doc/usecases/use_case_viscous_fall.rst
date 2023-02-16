.. _use-case-viscous-fall:

A viscous free fall example
===========================


Introduction
-------------

We consider an object inside a vertical cylinder which contains a viscous fluid. The fluid generates a drag force which limits the speed of the solid and we assume that the force depends linearily on the object speed:

.. math::
   m \frac{dv}{dt} = - m g - c v


for any :math:`t \in [0, t_{max}]` where:

    - :math:`v` is the speed :math:`[m/s]`,
    - :math:`t` is the time :math:`[s]`,
    - :math:`t_{max}` is the maximum time :math:`[s]`,
    - :math:`g = 9.81` is the gravitational acceleration :math:`[m.s^{-2}]`,
    - :math:`m` is the mass :math:`[kg]`,
    - :math:`c` is the linear drag coefficient :math:`[kg.s^{-1}]`.

The previous differential equation has the exact solution:

.. math::
   z(t) = z_0 + v_{inf} t + \tau (v_0 - v_{inf})\left(1 - e^{-\frac{t}{\tau}}\right)


for any :math:`t \in [0, t_{max}]`

where:

- :math:`z` is the altitude above the surface :math:`[m]`,
- :math:`z_0` is the initial altitude :math:`[m]`,
- :math:`v_0` is the initial speed (upward) :math:`[m.s^{-1}]`,
- :math:`v_{inf}` is the limit speed :math:`[m.s^{-1}]`:

.. math::
   v_{inf}=-\frac{m g}{c}

- :math:`\tau` is time caracteristic :math:`[s]`:

.. math::
   \tau=\frac{m}{c}.


The stationnary speed limit at infinite time is equal to :math:`v_{inf}`:

.. math::
   \lim_{t\rightarrow+\infty} v(t)= v_{inf}.


When there is no drag, i.e. when :math:`c=0`, the trajectory depends quadratically on :math:`t`:

.. math::
   z(t) = z_0 + v_0 t -g t^2


for any :math:`t \in [0, t_{max}]`.

Furthermore when the solid touches the ground, we ensure that the altitude remains nonnegative i.e. the final altitude is:

.. math::
   y(t) = \max(z(t),0)


for any :math:`t \in [0, t_{max}]`.

Probabilistic model
-------------------

The parameters :math:`z_0`, :math:`v_0`, :math:`m` and :math:`c` are probabilistic:

    - :math:`z_0 \sim \mathcal{U}(100, 150)`,
    - :math:`v_0 \sim \mathcal{N}(55, 10)`,
    - :math:`m \sim \mathcal{N}(80, 8)`,
    - :math:`c \sim \mathcal{U}(0, 30)`.


References
----------

* Steven C. Chapra. Applied numerical methods with Matlab for engineers and scientists, Third edition. 2012. Chapter 7, "Optimization", p.182.

API documentation
-----------------

.. currentmodule:: openturns.usecases.viscous_free_fall

.. autoclass:: ViscousFreeFall
    :noindex:

Examples based on this use case
-------------------------------

.. minigallery:: openturns.usecases.viscous_free_fall.ViscousFreeFall

