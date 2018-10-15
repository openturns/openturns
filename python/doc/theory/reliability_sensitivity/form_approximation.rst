.. _form_approximation:

FORM
----

| The First Order Reliability Method is used under the following
  context: :math:`\vect{X}` is the input random vector and :math:`\pdf`
  its joint density probability function.
| Let us denote by :math:`\vect{d}` a deterministic vector,
  :math:`g(\vect{X}\,,\,\vect{d})` the limit state function of the
  model,
  :math:`\cD_f = \{\vect{X} \in \Rset^n \, / \, g(\vect{X}\,,\,\vect{d}) \le 0\}`
  the event considered here and g(,) = 0 its boundary.
| The objective of FORM is to evaluate the probability content of the
  event :math:`\cD_f`:

  .. math::
    :label: PfX5

      P_f = \Prob{g(\vect{X}\,,\,\vect{d})\leq 0}=   \int_{\cD_f}  \pdf\, d\vect{x}

The method proceeds in three steps:

#. | Map the probabilistic model in terms of :math:`\vect{X}` thanks to
     an isoprobabilistic transformation :math:`T` which is a
     diffeomorphism from :math:`\supp{\vect{X}}` into :math:`\Rset^n`,
     such that the distribution of the random vector
     :math:`\vect{U}=T(\vect{X})` has the following properties :
     :math:`\vect{U}` and :math:`\mat{R}\,\vect{U}` have the same
     distribution for all rotations :math:`\mat{R}\in{\cS\cO}_n(\Rset)`.
   | The usual isoprobabilistic transformations are the Generalized
     Nataf transformation and the Rosenblatt one.
   | The mapping of the limit state function is
     :math:`G(\vect{U}\,,\,\vect{d}) =  g(T^{-1}(\vect{U})\,,\,\vect{d})`.
     Then, the event probability :math:`P_f` rewrites :

     .. math::
       :label: PfU3

          P_f = \Prob{G(\vect{U}\,,\,\vect{d})\leq 0} = \int_{\Rset^n} \boldsymbol{1}_{G(\vect{u}\,,\,\vect{d}) \leq 0}\,f_{\vect{U}}(\vect{u})\,d\vect{u}

     where :math:`f_{\vect{U}}` is the density function of the
     distribution in the standard space : that distribution is spherical
     (invariant by rotation by definition). That property implies that
     :math:`f_{\vect{U}}` is a function of :math:`||\vect{U}||^2` only.
     Furthermore, we suppose that outside the sphere which tangents the
     limit state surface in the standard space, :math:`f_{\vect{U}}` is
     decreasing.

#. | Find the design point :math:`\vect{u}^*` which is the point verifying
     the event of maximum likelihood : the decreasing hypothesis of the
     standard distribution :math:`f_{\vect{U}}` outside the sphere which
     tangents the limit state surface in the standard space implies that
     the design point is the point on the limit state boundary the nearest
     to the origin of the standard space. Thus, :math:`\vect{u}^*` is the
     result of a constrained optimization problem.

#. | In the standard space, approximate the limit state surface in the
     standard space by a linear surface at the design point
     :math:`\vect{u}^*`. Then, the probability :math:`P_f` (:eq:`PfU3`) where
     the limit state surface has been approximated by a linear surface
     (hyperplane) can be obtained exactly, thanks to the rotation
     invariance of the standard distribution :math:`f_{\vect{U}}` :

   .. math::
     :label: PfFORM

        P_{f,FORM} =
            \left|
            \begin{array}{ll}
              E(-\beta_{HL}) & \mbox{if the origin of the $\vect{u}$-lies in the domain $\cD_f$} \\
              E(+\beta_{HL}) & \mbox{otherwise}
            \end{array}
            \right.

   | where :math:`\beta_{HL}` is the Hasofer-Lind reliability index,
     defined as the distance of the design point
     :math:`\vect{u}^*` to the origin of the standard space and
     :math:`E` the marginal cumulative density function of the spherical
     distributions in the standard space.
   | Let us recall here that in the Rosenblatt standard
     space, random vectors follow the standard normal distribution (with
     zero mean, unit variance and unit correlation matrix), which
     implies that :math:`E = \Phi`. In the Generalized Nataf standard
     space, random vectors follow some spherical distributions, with
     zero mean, unit variance, unit correlation matrix and which type
     :math:`\psi` is the one of the copula of the physical random vector
     :math:`\vect{X}` : in that case, :math:`E` is the 1D cumulative
     distribution function with zero mean, unit variance and which type
     is :math:`\psi`.

Here, the event considered is explicited directly from the limit state
function :math:`g(\vect{X}\,,\,\vect{d})` : this is the classical
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
:math:`P_f = P(Z \geq z_s) = \int_{g(\vect{X}\,,\,\vect{d}) \le 0}  \pdf\, d\vect{x}`.


.. topic:: API:

    - See :class:`~openturns.FORM`


.. topic:: Examples:

    - See :doc:`/examples/reliability_sensitivity/estimate_probability_form`


.. topic:: References:

    - O. Ditlevsen and H.O. Madsen, 2004, "Structural reliability methods", Department of mechanical engineering technical university of Denmark - Maritime engineering, internet publication.
    - [lebrun2009b]_
    - [lebrun2009c]_
    - H. O. Madsen, Krenk, S., Lind, N. C., 1986, "Methods of Structural Safety", Prentice Hall.
