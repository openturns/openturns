.. _stationary_covariance_model:

Parametric stationary covariance models
=======================================

| Let :math:`X: \Omega \times \cD \rightarrow \Rset^d` be a multivariate
  stationary normal process where :math:`\cD \in \Rset^n`. The process
  is supposed to be zero mean. It is entirely defined by its covariance
  function
  :math:`C^{stat}: \cD \rightarrow  \mathcal{M}_{d \times d}(\Rset)`,
  defined by
  :math:`C^{stat}(\vect{\tau})=\Expect{X_{\vect{s}}X_{\vect{s}+\vect{\tau}}^t}`
  for all :math:`\vect{s}\in \Rset^n`.
| If the process is continuous, then :math:`\cD=\Rset^n`. In the
  discrete case, :math:`\cD` is a lattice.
| This use case illustrates how the User can create a covariance
  function from parametric models. The library proposes the
  *multivariate Exponential model* as one of the possible parametric models for the
  covariance function :math:`C^{stat}`.

**The multivariate exponential model**

This model defines the covariance function :math:`C^{stat}` by:

.. math::
  :label: fullMultivariateExponential2

    \forall \vect{\tau} \in \cD,\quad C^{stat}( \vect{\tau} )=\left[\mat{A}\mat{\Delta}( \vect{\tau} ) \right] \,\mat{R}\, \left[ \mat{\Delta}( \vect{\tau} )\mat{A}\right]

where :math:`\mat{R} \in  \mathcal{M}_{d \times d}([-1, 1])` is a
correlation matrix,
:math:`\mat{\Delta}( \vect{\tau} ) \in \mathcal{M}_{d \times d}(\Rset)`
is defined by:

.. math::
  :label: fullMultivariateExponential3

  \mat{\Delta}( \vect{\tau} )= \mbox{Diag}(e^{-\lambda_1|\tau|/2}, \dots, e^{-\lambda_d|\tau|/2})

and :math:`\mat{A}\in \mathcal{M}_{d \times d}(\Rset)` is defined by:

.. math::
  :label: fullMultivariateExponential4

    \mat{A}= \mbox{Diag}(a_1, \dots, a_d)

with :math:`\lambda_i>0` and :math:`a_i>0` for any :math:`i`.

We call :math:`\vect{a}` the amplitude vector and
:math:`\vect{\lambda}` the scale vector.
The expression of :math:`C^{stat}` is the combination of:

-  the matrix :math:`\mat{R}` that models the spatial correlation
   between the components of the process :math:`X` at any vertex
   :math:`\vect{t}` (since the process is stationary):

   .. math::
     :label: fullMultivariateExponential1

      \forall \vect{t}\in \cD,\quad \mat{R} = \Cor{X_{\vect{t}}, X_{\vect{t}}}

-  the matrix :math:`\mat{\Delta}( \vect{\tau} )` that models the
   correlation between the marginal random variables
   :math:`X^i_{\vect{t}}` and :math:`X^i_{\vect{t}+\vect{\tau}}`:

   .. math::

      \begin{aligned}
          \Cor{X_{\vect{t}}^i,X^i_{\vect{t}+\vect{\tau}}} = e^{-\lambda_i|\tau|}
        \end{aligned}

-  the matrix :math:`\mat{A}` that models the variance of each marginal
   random variable:

   .. math::

      \begin{aligned}
          \Var{X_{\vect{t}}} = (a_1, \dots, a_d)
        \end{aligned}

This model is such that:

.. math::
  :label: diago

   \begin{aligned}
     C_{ij}^{stat}(\vect{\tau}) & = a_ie^{-\lambda_i|\tau|/2}R_{i,j}a_je^{-\lambda_j|\tau|/2},\quad i\neq j\\
     C_{ii}^{stat}(\vect{\tau}) & = a_ie^{-\lambda_i|\tau|/2}R_{i,i}a_ie^{-\lambda_j|\tau|/2}=a_i^2e^{-\lambda_i|\tau|}
   \end{aligned}

It is possible to define the exponential model from the spatial
covariance matrix :math:`\mat{C}^{spat}` rather than the correlation
matrix :math:`\mat{R}` :

.. math::
  :label: relRA

    \forall \vect{t} \in \cD,\quad \mat{C}^{spat} = \Expect{X_{\vect{t}}X^t_{\vect{t}}} = \mat{A}\,\mat{R}\, \mat{A}

.. topic:: API:

    - See :class:`~openturns.AbsoluteExponential`
    - See :class:`~openturns.DiracCovarianceModel`
    - See :class:`~openturns.ExponentialModel`
    - See :class:`~openturns.ExponentiallyDampedCosineModel`
    - See :class:`~openturns.GeneralizedExponential`
    - See :class:`~openturns.MaternModel`
    - See :class:`~openturns.SquaredExponential`

.. topic:: Examples:

    - See :doc:`/examples/probabilistic_modeling/create_stationary_covmodel`
    - See :doc:`/examples/probabilistic_modeling/user_stationary_covmodel`
