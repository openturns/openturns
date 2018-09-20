.. _arma_process:

ARMA stochastic process
=======================

| Consider a stationary multivariate stochastic process
  :math:`X: \Omega \times[0,T] \rightarrow \Rset^d` of dimension
  :math:`d`, where :math:`X_t : \Omega \rightarrow \Rset^d` is the
  random variable at time :math:`t`. Under some general
  conditions,\ :math:`X` can be modeled by an :math:`ARMA(p,q)` model
  defined at the time stamp :math:`t` by:

  .. math::
     :label: dimn

       X_t + \mat{A}_{\, 1}   \, X_{t-1} + \hdots +  \mat{A}_{\, p} \,   X_{t-p} =
       \varepsilon_{t}+  \mat{B}_ {\, 1} \,  \varepsilon_{t-1}+   \hdots + \mat{B}_{\, q}  \, \varepsilon_{t-q}

where the coefficients of the recurrence are matrix in
:math:`\Rset^d` and :math:`(\varepsilon_t)_t`\ is white noise
discretized on the same time grid as the process :math:`X`.

The coefficients :math:`(\mat{A}_{\, 1} , \hdots, \mat{A}_{\, p} )`
form the Auto Regressive (AR) part of the model, while the
coefficients :math:`(\mat{B}_{\, 1} \, \hdots, \mat{B}_{\, q}  )` the
Moving Average (MA) part.

We introduce the homogeneous system associated to :eq:`dimn`:

  .. math::
     :label: dimnHom

       X_t + \mat{A}_{\, 1}   \,  X_{t-1} + \hdots +  \mat{A}_{\, p} \,  X_{t-p} = 0

To get stationary solutions of :eq:`dimn`, it is necessary to get its
characteristic polynomial defined in :eq:`PolPhi`:

.. math::
  :label: PolPhi

     \Phi(\vect{r}) = \vect{r}^p + \sum_{i=1}^p a_i\vect{r}^{p-i}

Thus the solutions of :eq:`dimnHom` are of the form
:math:`P(t)\vect{r}_i^t` where the :math:`\vect{r}_i` are the roots of
the polynomials :math:`\Phi(\vect{r})` defined in :eq:`PolPhi` and
:math:`P` is a polynomials of degree the order of the root
:math:`\vect{r}_i`:

The processes :math:`P(t)\vect{r}_i^t` decrease with time if and only if
the modulus of all the components of the roots :math:`\vect{r}_i` are
less than 1:

.. math::
   :label: Modul

     \forall i,j \in [1,d], \,  |r_{ij}| <1

Once given the coefficients of the model :math:`ARMA(p,q)`, we
evaluate the roots of the polynomials :math:`\Phi(\vect{r})` and checks
the previous condition :eq:`Modul`. The roots :math:`\vect{r}_i`, are the
eigenvalues of the matrix :math:`\mat{M}` which writes in dimension
:math:`d` as:

.. math::
   :label: MatrixMdimn

     \mat{M} = \left(
     \begin{array}{cccccc}
       \mat{0}_{\, d} & \mat{1}_{\, d} & \mat{0}_{\, d} & \hdots & \mat{0}_{\, d} & \mat{0}_{\, d} \\
       \mat{0}_{\, d} & \mat{0}_{\, d} & \mat{1}_{\, d} & \hdots & \mat{0}_{\, d} & \mat{0}_{\, d}\\
       \hdots \\
       \mat{0}_{\, d} & \mat{0}_{\, d} & \mat{0}_{\, d} & \hdots & \mat{0}_{\, d} & \mat{1}_{\, d}\\
       - \mat{A}_{\, 1} & -\mat{A}_{\, 2} & - \mat{A}_{\, 3}& \hdots  & -\mat{A}_{\, p-1}& -\mat{A}_{\, p}
     \end{array}
     \right)

and in dimension 1:

.. math::
   :label: MatrixMdim1

     \mat{M} = \left(
     \begin{array}{cccccc}
       0 & 1 & 0 & \hdots & 0 & 0\\
       0 & 0 & 1 & \hdots & 0 & 0\\
       \hdots \\
       0 & 0 & 0 & \hdots & 0 & 1\\
       -\alpha_1 & -\alpha_2 & -\alpha_3 & \hdots  & -\alpha_{p-1} & -\alpha_p
     \end{array}
     \right)

The matrix :math:`\mat{M}` is known to be the companion matrix.




It is important to note that:

-  when asking for a *realization* of the stationary process modeled by
   :math:`ARMA(p,q)`, one has to obtain a realization that does not
   depend on the current state of the process;

-  whereas, when one asks for a *possible future* extending a particular
   current state of the process, the realization of the model must depend
   on that current sate.

How to proceed to respect these constraints?

If we note :math:`\vect{X}_1(\omega,t)` and
:math:`\vect{X}_2(\omega,t)` two distinct solutions of :eq:`dimn`
associated to two distinct initial states, then, the process
:math:`\vect{D}(\omega,t) = \vect{X}_2(\omega,t) - \vect{X}_1(\omega,t)`
is solution of the homogeneous equation associated to :eq:`dimn` and
then decreases with time under the condition :eq:`Modul`. Let us note
:math:`N_{ther}` the number such that:

.. math::
  :label: nTher

    \left( \max_{i,j} |r_{ij}| \right)^{N_{ther}} < \varepsilon ,\Longleftrightarrow N_{ther} > \displaystyle \frac{\ln  \varepsilon}{\ln \max_{i,j} |r_{ij}|}

where the :math:`r_i` are the roots of the polynomials :eq:`PolPhi` and
:math:`\varepsilon` is the precision of the computer (
:math:`\varepsilon =2^{-53} \simeq 10^{-16}`). Then, after
:math:`N_{ther}` instants, the process :math:`\vect{D}(\omega,t)` has
disappeared, which means that the processes
:math:`\vect{X}_1(\omega,t)` and :math:`\vect{X}_2(\omega,t)` do not
differ any more. As a conclusion, after :math:`N_{ther}` instants, the
realization of the ARMA process does not depend any more on the
initial state.

That is why, when making a realization of the ARMA model, we perform
a *thermalization step* that simply consists in realizing the
model upon :math:`N_{ther}` additional instants, erasing the
:math:`N_{ther}` first values and finally only retaining the other
ones. That step ensures that the realization of the process does not
depend on the initial state.

By default, the number :math:`N_{ther}` is evaluated according to
:eq:`nTher` by the method *computeNThermalization*. The User could get
access to it with the method *getNThermalization* and can change the
value with the method *setNThermalization*. (In order to give back to
:math:`N_{ther}` its default value, it is necessary to re-use the
method *computeNThermalization*).

On the contrary, in the context of getting a possible future from a
specified current state, the User should care that the number of
additional instants :math:`N_{it}` on which he wants to extend the
process, is such that :math:`N_{it} <  N_{ther}` because beyond
:math:`N_{ther}`, the future has no link with the present.
More precisely, after :math:`N_{it}^*` instants, such that:

.. math::
  :label: nitEt

     \left( \max_{i,j} |r_{ij}| \right)^{N_{it}^*} <  \max_{i} \sigma_i, \Longleftrightarrow N_{ther} > \displaystyle \frac{\max_{i} \sigma_i}{\ln \max_{i,j} |r_{ij}|}

where the :math:`\sigma_i` are the components of the covariance matrix
of the white noise :math:`\vect{\varepsilon}`, the influence of the
initial state is of same order than the influence of the white noise.

Let us note that when the ARMA model is created without specifying
the current state, we automatically proceed to a
thermalization step at the creation of the ARMA object.

Before asking for the generation of a possible future, the user has to
specify the current state of the ARMA model, thanks to the creation
method that takes into account the current state. In that case,
we do not proceed to the thermalization step.

As an ARMA model is a stochastic process, the object *ARMA* inherits
the methods of the *Process* object. Thus, it is possible to get its
marginal processes, its time grid, its dimension and to get several
realizations at a time of the process.


.. topic:: API:

    - See :class:`~openturns.ARMA`
    - See :class:`~openturns.ARMACoefficients`
    - See :class:`~openturns.ARMAState`
    - See :class:`~openturns.ARMALikelihoodFactory`

.. topic:: Examples:

    - See :doc:`/examples/probabilistic_modeling/arma_creation`
    - See :doc:`/examples/probabilistic_modeling/arma_manipulation`
    - See :doc:`/examples/data_analysis/estimate_arma`
