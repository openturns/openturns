.. _form_approximation:

FORM
----

The First Order Reliability Method is used under the following
context: let :math:`\inputRV` be a random input vector with
joint density probability  :math:`\pdf`,
let  :math:`\model` be the
limit state function of the model and let :math:`\cD_f =
\{\vect{x} \in \Rset^{\inputDim} \,   / \, \model(\vect{x}) \le 0\}` be
an event whose probability :math:`P_f` is defined as:

.. math::
    :label: PfX5

      P_f = \Prob{\model(\inputRV)\leq 0}=   \int_{\cD_f}  \pdf\, d\vect{x}


The objective of FORM is to evaluate :math:`P_f`. The method proceeds in three steps:

**Step 1**: Map the probabilistic model in terms of :math:`\inputRV` thanks to an
isoprobabilistic transformation :math:`T` which is a diffeomorphism from
:math:`\supp{\inputRV}` into :math:`\Rset^{\inputDim}`, such that the distribution of the random vector
:math:`\RVU=T(\inputRV)` has
the following properties: :math:`\RVU` and :math:`\mat{R}\,\RVU` have the same
distribution for all rotations :math:`\mat{R}\in{\cS\cO}_d(\Rset)`.
The usual :ref:`isoprobabilistic_transformation` are the Generalized
Nataf transformation and the Rosenblatt one.
The mapping of the limit state function is
:math:`h(\vect{u}) =  \model(T^{-1}(\vect{u}))`.
Then, the event probability :math:`P_f` can be written as:

.. math::
    :label: PfU3

    P_f = \Prob{h(\RVU)\leq 0} = \int_{\Rset^n} \boldsymbol{1}_{h(\vect{u}\leq 0}
    \,f_{\RVU}(\vect{u})\,d\vect{u}


where :math:`f_{\RVU}` is the density function of the distribution in the standard space:
that distribution is spherical (invariant by rotation by definition). That property implies
that :math:`f_{\RVU}` is a function of :math:`||\RVU||^2` only.

Furthermore, we suppose that outside the sphere  tangent to the limit state surface in the standard space,
:math:`f_{\RVU}` is decreasing.

**Step 2**: Find the design point :math:`P^*` which is the point
verifying the event of maximum likelihood : the decreasing hypothesis of the standard
distribution :math:`f_{\RVU}` outside the sphere tangent to the limit state surface in
the standard space implies that the design point is the point on the limit state boundary that is closest
to the origin of the standard space. Thus, :math:`P^*` is the result of a constrained
optimization problem.

**Step 3**: In the standard space, approximate the limit state surface by a linear surface at the design
point :math:`P^*`. Then, the probability
:math:`P_f` :eq:`PfU3` where the limit state surface has been approximated by a linear surface (hyperplane)
can be obtained exactly, thanks to the rotation invariance of the standard distribution :math:`f_{\RVU}`:

.. math::
    :label: PfFORM

        P_{f,FORM} =
            \left|
            \begin{array}{ll}
              E(-\beta) & \mbox{if the origin fails}, \\
              E(+\beta) & \mbox{if the origin is safe}
            \end{array}
            \right.

where :math:`\beta` is the Hasofer-Lind reliability index, defined as the distance of the design point
:math:`P^*` to the origin of the standard space and :math:`E` the marginal cumulative distribution function
along any direction of
the spherical distribution in the standard space (refer to :ref:`nataf_transformation` and
:ref:`rosenblatt_transformation`).

Let us recall here that in the Rosenblatt standard space, random vectors follow the standard
normal
distribution (with zero mean, unit variance and unit correlation matrix), which implies that
:math:`E = \Phi` where :math:`\Phi` is the CDF of the normal distribution with zero mean and unit variance.
In the Generalized Nataf standard space, random vectors follow a spherical
distribution, with zero mean, unit variance, unit correlation matrix and whose type :math:`\psi` is the type of the
copula of the physical random vector :math:`\inputRV`: in that case, :math:`E` is the 1D cumulative  distribution
function with zero mean, unit variance and with type :math:`\psi`.

.. topic:: API:

    - See :class:`~openturns.FORM`
    - See :class:`~openturns.SystemFORM`

.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/reliability/plot_estimate_probability_form`

.. topic:: References:

    - O. Ditlevsen and H.O. Madsen, 2004, "Structural reliability methods,"
      Department of mechanical engineering technical university of Denmark -
      Maritime engineering, internet publication.
    - [lebrun2009b]_
    - [lebrun2009c]_
    - H.O. Madsen, Krenk, S., Lind, N. C., 1986, "Methods of Structural
      Safety," Prentice Hall.

