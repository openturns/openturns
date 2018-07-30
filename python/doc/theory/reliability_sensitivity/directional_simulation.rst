.. _directional_simulation:

Directional Simulation
----------------------

Using the probability distribution of a random vector :math:`\vect{X}`,
we seek to evaluate the following probability:

.. math::

    P_f = \Prob{g\left( \vect{X},\vect{d} \right) < 0}

Here, :math:`\vect{X}` is a random vector, :math:`\vect{d}` a
deterministic vector, :math:`g(\vect{X},\vect{d})` the function known as
“limit state function” which enables the definition of the event
:math:`\cD_f = \{\vect{X} \in \Rset^n \, / \, g(\vect{X},\vect{d}) \le 0\}`.

The directional simulation method is an accelerated sampling method. It
implies a preliminary iso-probabilistic transformation, as for FORM and SORM methods;
however, it remains based on sampling and is thus not an approximation method.
In the transformed space, the (transformed) uncertain variables :math:`\vect{U}` are
independent standard Gaussian variables (mean equal to zero and standard
deviation equal to 1).

Roughly speaking, each simulation of the directional simulation
algorithm is made of three steps. For the :math:`i^\textrm{th}`
iteration, these steps are the following:

-  Let :math:`\cS = \big\{ \vect{u} \big| ||\vect{u}|| = 1 \big\}`. A
   point :math:`P_i` is drawn randomly on :math:`\cS` according to an
   uniform distribution.

-  In the direction starting from the origin and passing through
   :math:`P_i`, solutions of the equation
   :math:`g(\vect{X},\vect{d}) = 0` (i.e. limits of :math:`\cD_f`) are
   searched. The set of values of :math:`\underline{u}` that belong to
   :math:`\cD_f` is deduced for these solutions: it is a subset
   :math:`I_i \subset \Rset`.

-  Then, one calculates the probability
   :math:`q_i = \Prob{ ||\vect{U}|| \in I_i }`. By property of
   independent standard variable, :math:`||\vect{U}||^2` is a random
   variable distributed according to a chi-square distribution, which
   makes the computation effortless.

Finally, the estimate of the probability :math:`P_f` after :math:`N`
simulations is the following:

.. math::

    \widehat{P}_{f,DS} = \frac{1}{N} \sum_{i=1}^N q_i

The following figure illustrates the principle of an iteration in
dimension 2.

The Central Limit Theorem enables the difference between the estimated
value and the sought value to be controlled by means of a confidence
interval (if N is sufficiently large, typically :math:`N` > a few dozens
even if there is now way to say for sure if the asymptotic behaviour is
reached). For a probability :math:`\alpha` strictly between 0 and 1
chosen by the user, one can, for example, be sure with a confidence
:math:`\alpha`, that the true value of :math:`P_f` is between
:math:`\widehat{P}_{f,\inf}` and :math:`\widehat{P}_{f,\sup}` calculated
analytically from simple formulae. To illustrate, for :math:`\alpha = 0.95`:

.. math::

    \widehat{P}_{f,\inf} = \widehat{P}_{f,DS} - 1.96 \frac{\sigma_q}{\sqrt{N}},\ \widehat{P}_{f,\sup} = \widehat{P}_{f,DS} + 1.96 \frac{\sigma_q}{\sqrt{N}}

.. math::

    \textrm{that is to say}\ \Prob{ \widehat{P}_{f,\inf} \leq P_f \leq \widehat{P}_{f,\sup}} = 0.95

where :math:`\sigma_q` denotes the empirical standard deviation of the
sample :math:`\left\{ q_1,\ldots,q_N \right\}`.

In practice the Directional Sampling simulation requires
the choice of:

-  a Root Strategy :

   -  | RiskyAndFast: for each direction, we check whether there is a
        sign change of the standard limit state function between the
        maximum distant point (at distance *MaximumDistance* from the
        center of the standard space) and the center of the standard
        space.
      | In case of sign change, we research one root in the segment
        [origin, maximum distant point] with the selected non linear
        solver.
      | As soon as founded, the segment [root, infinity point] is
        considered within the failure space.

   -  | MediumSafe: for each direction, we go along the direction by
        step of length *stepSize* from the origin to the maximum distant
        point (at distance *MaximumDistance* from the center of the
        standard space) and we check whether there is a sign change
        on each segment so formed.
      | At the first sign change, we research one root in the
        concerned segment with the selected non linear solver. Then, the
        segment [root, maximum distant point] is considered within the
        failure space.
      | If *stepSize* is small enough, this strategy guarantees us to
        find the root which is the nearest from the origin.

   -  | SafeAndSlow : for each direction, we go along the direction by
        step of length *stepSize* from the origin to the maximum distant
        point(at distance *MaximumDistance* from the center of the
        standard space) and we check whether there is a sign change
        on each segment so formed.
      | We go until the maximum distant point. Then, for all the
        segments where we detected the presence of a root, we research
        the root with the selected non linear solver. We evaluate the
        contribution to the failure probability of each segment.
      | If *stepSize* is small enough, this strategy guarantees us to
        find all the roots in the direction and the contribution of this
        direction to the failure probability is precisely evaluated.

-  a Non Linear Solver:

   -  Bisection: bisection algorithm,

   -  Secant: based on the evaluation of a segment between the two last
      iterated points,

   -  Brent: mix of Bisection, Secant and inverse quadratic
      interpolation.

-  and a Sampling Strategy:

   -  RandomDirection: we generate some points on the sphere unity
      according to the uniform distribution and we consider both
      opposite directions so formed.

   -  | OrthogonalDirection: this strategy is parameterized by
        :math:`k\in \{1,\dots,n\}`, where :math:`n` is the dimension of
        the input random vector :math:`\vect{X}`. We generate one direct
        ortho-normalized basis :math:`(e_1, \dots, e_n)` uniformly
        distributed in the set of direct orthonormal bases. We consider
        all the normalized linear combinations of :math:`k` vectors
        chosen within the :math:`n` vectors of the basis, where the
        coefficients of the linear combinations are in
        :math:`\{+1, -1\}`. This generates :math:`C_n^k 2^k` new vectors
        :math:`v_i`. We sample according to all the directions defined
        by the vectors :math:`v_i`.
      | If :math:`k=1`, we consider all the axes of the standard space.


.. topic:: API:

    - See :class:`~openturns.DirectionalSampling`


.. topic:: Examples:

    - See :doc:`/examples/reliability_sensitivity/estimate_probability_directional_sampling`


.. topic:: References:

    - Robert C.P., Casella G. (2004). Monte-Carlo Statistical Methods, Springer, ISBN 0-387-21239-6, 2nd ed.
    - Rubinstein R.Y. (1981). Simulation and The Monte-Carlo methods, John Wiley \& Sons
    - Bjerager, P. (1988). "Probability integration by Directional Simulation". Journal of Engineering Mechanics, vol. 114, no. 8

