.. _importance_form:

Importance factors from FORM method
-----------------------------------

| Importance Factors are evaluated in the following context :
  :math:`\vect{X}` denotes a random input vector, representing the
  sources of uncertainties, :math:`\pdf` its joint density probability,
  :math:`\vect{d}` a deterministic vector, representing the fixed
  variables :math:`g(\vect{X}\,,\,\vect{d})` the limit state function of
  the model,
  :math:`\cD_f = \{\vect{X} \in \Rset^n \, / \, g(\vect{X}\,,\,\vect{d}) \le 0\}`
  the event considered here and :math:`{g(\vect{X}\,,\,\vect{d}) = 0}`
  its boundary (also called limit state surface).
| The probability content of the event :math:`\cD_f` is :math:`P_f`:

  .. math::
    :label: PfX10

     \begin{aligned}
         P_f =       \int_{g(\vect{X}\,,\,\vect{d}) \le 0}  \pdf\, d\vect{x}.
       \end{aligned}

| In this context, the probability :math:`P_f` can often be
  efficiently estimated by FORM or SORM approximations.
| The FORM importance factors offer a way to rank the importance of the
  input components with respect the realization of the event. They are
  often interpreted also as indicators of the impact of modeling the
  input components as random variables rather than fixed values. The
  FORM importance factors are defined as follows.

| The isoprobabilistic transformation :math:`T` used in the FORM and
  SORM approximation is a diffeomorphism from
  :math:`\supp{\vect{X}}` into :math:`\Rset^n`, such that the
  distribution of the random vector :math:`\vect{U}=T(\vect{X})` has the
  following properties: :math:`\vect{U}` and :math:`\mat{R}\,\vect{U}`
  have the same distribution for all rotations
  :math:`\mat{R}\in{\cS\cO}_n(\Rset)`.
| In the standard space, the design point :math:`\vect{u}^*` is the
  point on the limit state boundary the nearest to the origin of the
  standard space. The design point is :math:`\vect{x}^*` in the physical
  space, where :math:`\vect{x}^* = T^{-1}(\vect{u}^*)`. We note
  :math:`\beta_{HL}` the Hasofer-Lind reliability index:
  :math:`\beta_{HL} = ||\vect{u}^{*}||`.
| When the :math:`\bdU`-space is normal, the litterature proposes to
  calculate the importance factor :math:`\alpha_i^2` of the variable
  :math:`X_i` as the square of the co-factors of the design point in the
  :math:`\bdU`-space :

  .. math::
    :label: def1

        \alpha_i^2 = \displaystyle \frac{(u_i^{*})^2}{\beta_{HL}^2}

| This definition guarantees the relation :
  :math:`\Sigma_i \alpha_i^2 = 1`.
| Let’s note that this definition arises the following difficulties :

-  Which signification for :math:`\alpha_i` when the variables
   :math:`X_i` are correlated? In that case, the isoprobabilistic
   transformation does not associate :math:`U_i` to :math:`X_i` but
   :math:`U_i` to a set of :math:`X_i`.

-  In the case of dependence of the variables :math:`X_i`, the shape of
   the limit state function in the :math:`\bdU`-space depends on the
   isoprobabilistic transformation and in particular on the order of the
   variables :math:`X_i` within the random vector :math:`\vect{X}`.
   Thus, changing this order has an impact on the localisation of the
   design point in the :math:`\bdU`-space and, consequently, on the
   importance factors ... (see [lebrun2009c]_ to compare
   the different isoprobabilistic transformations).

| It is possible to give another definition to the importance factors
  which may be defined in the elliptical space of the iso-probabilistic
  transformation, where the marginal distributions are all elliptical,
  with cumulative distribution function noted :math:`E`, and not yet
  decorrelated.

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
:math:`\Sigma_i \alpha_i^2 = 1`.

Here, the event considered is explicited directly from the limit state
function :math:`g(\vect{X}\,,\,\vect{d})`: this is the classical
structural reliability formulation.
However, if the event is a threshold exceedance, it is useful to
explicit the variable of interest
:math:`Z=\tilde{g}(\vect{X}\,,\,\vect{d})`, evaluated from the model
:math:`\tilde{g}(.)`. In that case, the event considered, associated to
the threshold :math:`z_s` has the formulation:
:math:`\cD_f = \{ \vect{X} \in \Rset^n \, / \, Z=\tilde{g}(\vect{X}\,,\,\vect{d}) > z_s \}`
and the limit state function is :
:math:`g(\vect{X}\,,\,\vect{d}) = z_s - Z = z_s - \tilde{g}(\vect{X}\,,\,\vect{d})`.
:math:`P_f` is the threshold exceedance probability, defined as :
:math:`P_f     =       P(Z \geq z_s) = \int_{g(\vect{X}\,,\,\vect{d}) \le 0}  \pdf\, d\vect{x}`.
Thus, the FORM importance factors offer a way to rank the importance of
the input components with respect to the threshold exceedance by the
quantity of interest :math:`Z`. They can be seen as a specific sensitivity
analysis technique dedicated to the quantity Z around a particular
threshold rather than to its variance.


.. topic:: API:

    - See :class:`~openturns.FORM`
    - See :class:`~openturns.SORM`


.. topic:: Examples:

    - See :doc:`/examples/reliability_sensitivity/estimate_probability_form`


.. topic:: References:

    - H.O. Madsen, "Omission Sensitivity Factors," 1988, Structural Safety, 5, 35-45.
    - [lebrun2009c]_

