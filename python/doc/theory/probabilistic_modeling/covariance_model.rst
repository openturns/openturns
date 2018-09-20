.. _covariance_model:

Covariance models
=================

We consider :math:`X: \Omega \times\cD \mapsto \Rset^d` a multivariate
stochastic process of dimension :math:`d`, where :math:`\omega \in \Omega`
is an event, :math:`\cD` is a domain of :math:`\Rset^n`,
:math:`\vect{t}\in \cD` is a multivariate index and
:math:`X(\omega, \vect{t}) \in \Rset^d`.

We note :math:`X_{\vect{t}}: \Omega \rightarrow \Rset^d` the random variable at
index :math:`\vect{t} \in \cD` defined by
:math:`X_{\vect{t}}(\omega)=X(\omega, \vect{t})` and
:math:`X(\omega): \cD  \mapsto \Rset^d` a realization of the process
:math:`X`, for a given :math:`\omega \in \Omega` defined by
:math:`X(\omega)(\vect{t})=X(\omega, \vect{t})`.

If the process is a second order process, we note:

- :math:`m : \cD \mapsto  \Rset^d` its *mean function*, defined by
  :math:`m(\vect{t})=\Expect{X_{\vect{t}}}`,
- :math:`C : \cD \times \cD \mapsto  \cS_d^+(\Rset)` its
  *covariance function*, defined by
  :math:`C(\vect{s}, \vect{t})=\Expect{(X_{\vect{s}}-m(\vect{s}))\Tr{(X_{\vect{t}}-m(\vect{t}))}}`,
- :math:`R : \cD \times \cD \mapsto  \cS_d^+(\Rset)` its
  *correlation function*, defined for all :math:`(\vect{s}, \vect{t})`,
  by :math:`R(\vect{s}, \vect{t})` such that for all :math:`(i,j)`,
  :math:`R_{ij}(\vect{s}, \vect{t})=C_{ij}(\vect{s}, \vect{t})/\sqrt{C_{ii}(\vect{s}, \vect{t})C_{jj}(\vect{s}, \vect{t})}`.


In a general way, the covariance models write:

.. math::

    C(\vect{s}, \vect{t}) = \mat{L}_{\rho}\left(\dfrac{\vect{s}}{\theta}, 
                            \dfrac{\vect{t}}{\theta}\right)\, 
                            \mbox{Diag}(\vect{\sigma}) \, \mat{R} \, 
                            \mbox{Diag}(\vect{\sigma}) \, 
                            \Tr{\mat{L}}_{\rho}\left(\dfrac{\vect{s}}{\theta}, 
                            \dfrac{\vect{t}}{\theta}\right), \quad 
                            \forall (\vect{s}, \vect{t}) \in \cD

where:

- :math:`\vect{\theta} \in \Rset^n` is the *scale* parameter
- :math:`\vect{\sigma} \in \Rset^d` id the *amplitude* parameter
- :math:`\mat{L}_{\rho}(\vect{s}, \vect{t})` is the Cholesky factor of 
  :math:`\mat{\rho}(\vect{s}, \vect{t})`: 

.. math::

    \mat{L}_{\rho}(\vect{s}, \vect{t})\,\Tr{\mat{L}_{\rho}(\vect{s}, \vect{t})}
    = \mat{\rho}(\vect{s}, \vect{t})

The correlation function :math:`\mat{\rho}` may depend on additional
specific parameters which are not made explicit here.

The global correlation is given by two separate correlations: 

    - the spatial correlation between the components of :math:`X_{\vect{t}}`
      which is given by the correlation matrix
      :math:`\mat{R} \in \cS_d^+(\Rset)` and the vector of marginal variances
      :math:`\vect{\sigma} \in \Rset^d`.
      The spatial correlation does not depend on :math:`\vect{t} \in \cD`.
      For each  :math:`\vect{t}`, it links together the components of
      :math:`X_{\vect{t}}`.
    - the correlation between :math:`X_{\vect{s}}` and  :math:`X_{\vect{t}}`
      which is given by :math:`\mat{\rho}(\vect{s}, \vect{t})`. 

        - In the general case, the correlation links each component
          :math:`X^i_{\vect{t}}` to all the components of :math:`X_{\vect{s}}`
          and :math:`\mat{\rho}(\vect{s}, \vect{t}) \in \cS_d^+(\Rset)`;

        - In some particular cases, the correlation is such that
          :math:`X^i_{\vect{t}}` depends only on the component
          :math:`X^i_{\vect{s}}` and that link does not depend on the component
          :math:`i`. In that case, :math:`\mat{\rho}(\vect{s}, \vect{t})` can be
          defined from the scalar function :math:`\rho(\vect{s}, \vect{t})` by
          :math:`\mat{\rho}(\vect{s}, \vect{t}) = \rho(\vect{s}, \vect{t})\, \mat{I}_d`.
          Then, the covariance model writes:

.. math::

    C(\vect{s}, \vect{t}) = \rho\left(\dfrac{\vect{s}}{\theta}, 
                                      \dfrac{\vect{t}}{\theta}\right)\,
                            \mbox{Diag}(\vect{\sigma}) \, \mat{R} \,
                            \mbox{Diag}(\vect{\sigma}), \quad
                            \forall (\vect{s}, \vect{t}) \in \cD

.. topic:: API:

    - See :class:`~openturns.AbsoluteExponential`
    - See :class:`~openturns.DiracCovarianceModel`
    - See :class:`~openturns.ExponentialModel`
    - See :class:`~openturns.ExponentiallyDampedCosineModel`
    - See :class:`~openturns.GeneralizedExponential`
    - See :class:`~openturns.MaternModel`
    - See :class:`~openturns.UserDefinedStationaryCovarianceModel`
    - See :class:`~openturns.SquaredExponential`

.. topic:: Examples:

    - See :doc:`/examples/probabilistic_modeling/create_stationary_covmodel`
    - See :doc:`/examples/probabilistic_modeling/user_stationary_covmodel`
    - See :doc:`/examples/probabilistic_modeling/userdefined_covariance_model`

