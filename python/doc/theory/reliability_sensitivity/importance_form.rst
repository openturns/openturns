.. _importance_form:

Importance factors from FORM method
-----------------------------------

Importance Factors are evaluated in the following context: let :math:`\vect{X}` be a probabilistic input vector with
joint density probability  :math:`\pdf`, let :math:`\vect{d}` be a
deterministic vector, let  :math:`g(\vect{X}\,,\,\vect{d})` be the
limit state function of the model and let :math:`\cD_f =
\{\vect{X} \in \Rset^n \,   / \, g(\vect{X}\,,\,\vect{d}) \le 0\}` be
an event whose probability :math:`P_f` is defined as:

.. math::
    :label: PfX10

         P_f =       \int_{g(\vect{X}\,,\,\vect{d}) \le 0}  \pdf\, d\vect{x}

In this context, the probability :math:`P_f` can often be
efficiently estimated by :ref:`form_approximation` or :ref:`sorm_approximation` approximations.

FORM importance factors rank the importance of the
input components with respect to the realization of the event. They are often seen as indicators
of the impact of modeling the
input components as random variables rather than fixed values. FORM importance factors are defined as follows.

The :ref:`isoprobabilistic transformation <isoprobabilistic_transformation>` :math:`T` used in the FORM and
SORM approximation is a diffeomorphism from
:math:`\supp{\vect{X}}` into :math:`\Rset^n`, such that the
distribution of the random vector :math:`\vect{U}=T(\vect{X})` has the
following properties: :math:`\vect{U}` and :math:`\mat{R}\,\vect{U}`
have the same distribution for all rotations
:math:`\mat{R}\in{\cS\cO}_n(\Rset)`.

In the standard space, the design point :math:`\vect{u}^*` is the
point on the limit state boundary the that is colosest to the origin of the
standard space. The design point is :math:`\vect{x}^*` in the physical
space, where :math:`\vect{x}^* = T^{-1}(\vect{u}^*)`. We note
:math:`\beta` the Hasofer-Lind reliability index:
:math:`\beta = ||\vect{u}^{*}||`.
When the :math:`\bdU`-space is normal, the literature proposes to
calculate the importance factor :math:`\alpha_i^2` of the variable
:math:`X_i` as the square of the co-factors of the design point in the
:math:`\bdU`-space :

.. math::
    :label: def1

        \alpha_i^2 = \displaystyle \frac{(u_i^{*})^2}{\beta_{HL}^2}

This definition implies that the sum of importance factors is equal to 1, i.e.:

.. math::

      \sum_{i=1}^n \alpha_i^2 = 1

This definition raises the following difficulties:

-  How can the :math:`\alpha_i` coefficients be interpreted
   when the :math:`X_i` variables are correlated?
   In that case, the isoprobabilistic
   transformation does not associate :math:`U_i` to :math:`X_i` but
   :math:`U_i` to a set of :math:`X_i`.

-  In the case of dependence of the variables :math:`X_i`, the shape of
   the limit state function in the :math:`\bdU`-space depends on the
   isoprobabilistic transformation and in particular on the order of the
   variables :math:`X_i` within the random vector :math:`\vect{X}`.
   Thus, changing this order has an impact on the localization of the
   design point in the :math:`\bdU`-space and, consequently, on the
   importance factors ... (see [lebrun2009c]_ to compare
   the different isoprobabilistic transformations).

It is possible to give another definition to the importance factors
which may be defined in the elliptical space of the iso-probabilistic
transformation (refer to :ref:`isoprobabilistic_transformation`), where the marginal distributions are all elliptical,
with cumulative distribution function noted :math:`E` , and not yet
uncorrelated. Let :math:`F_i` be the cumulative distribution of the component :math:`i`.

.. math::

     \begin{aligned}
         \boldsymbol{Y}^* =  \left(
         \begin{array}{c}
           E^{-1}\circ F_1(X_1^*) \\
           E^{-1}\circ F_2(X_2^*) \\
           \vdots \\
           E^{-1}\circ F_n(X_n^*)
         \end{array}
         \right).\label{varY10}
       \end{aligned}

The importance factor :math:`\alpha_i^2` writes:

.. math::
  :label: def2

    \alpha_i^2 = \displaystyle \frac{(y_i^{*})^2}{||\vect{y}^{*}||^2}

This definition still guarantees the relation:

.. math::

    \sum_{i=1}^n \alpha_i^2 = 1


.. topic:: API:

    - See :class:`~openturns.FORM`
    - See :class:`~openturns.SORM`


.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/reliability/plot_estimate_probability_form`


.. topic:: References:

    - H.O. Madsen, "Omission Sensitivity Factors," 1988, Structural Safety, 5, 35-45.
    - [lebrun2009c]_

