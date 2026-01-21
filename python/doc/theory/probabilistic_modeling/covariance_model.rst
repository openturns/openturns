.. _covariance_model:

Covariance models
=================

We consider :math:`X: \Omega \times\cD \mapsto \Rset^{\inputDim}` a multivariate
stochastic process of dimension :math:`d`, where :math:`\omega \in \Omega`
is an event, :math:`\cD` is a domain of :math:`\Rset^{\sampleSize}`,
:math:`\vect{t}\in \cD` is a multivariate index and
:math:`X(\omega, \vect{t}) \in \Rset^{\inputDim}`.

We note :math:`X_{\vect{t}}: \Omega \rightarrow \Rset^{\inputDim}` the random variable at
index :math:`\vect{t} \in \cD` defined by
:math:`X_{\vect{t}}(\omega)=X(\omega, \vect{t})` and
:math:`X(\omega): \cD  \mapsto \Rset^{\inputDim}` a realization of the process
:math:`X`, for a given :math:`\omega \in \Omega` defined by
:math:`X(\omega)(\vect{t})=X(\omega, \vect{t})`.

If the process is a second order process, we denote by:

- :math:`m : \cD \mapsto  \Rset^{\inputDim}` its *mean function*, defined by
  :math:`m(\vect{t})=\Expect{X_{\vect{t}}}`,
- :math:`\mat{C} : \cD \times \cD \mapsto  \cS_{\inputDim}^+(\Rset)` its
  *covariance function*, defined by
  :math:`\mat{C}(\vect{s}, \vect{t})=\Expect{(X_{\vect{s}}-m(\vect{s}))\Tr{(X_{\vect{t}}-m(\vect{t}))}}`,
- :math:`\mat{\rho} : \cD \times \cD \mapsto  \cS_{\inputDim}^+(\Rset)` its
  *correlation function*, defined for all :math:`(\vect{s}, \vect{t})`,
  by :math:`\mat{\rho}(\vect{s}, \vect{t})` such that for all :math:`(i,j)`,
  :math:`(\rho_{ij}(\vect{s}, \vect{t}) = C_{ij}(\vect{s}, \vect{t})/\sqrt{C_{ii}(\vect{s}, \vect{t})C_{jj}(\vect{s}, \vect{t})}`.


The library assumes that:

- the *spatial* correlation :math:`\mat{R} \in \cS_{\inputDim}^+(\Rset)` between the components of :math:`X_{\vect{t}}`
  and the vector of marginal standard deviations :math:`\vect{\sigma} \in \Rset^{\inputDim}`
  do not depend on :math:`\vect{t} \in \cD`,
- the correlation between :math:`X_{\vect{s}}` and  :math:`X_{\vect{t}}`
  which is given by :math:`\mat{\rho}(\vect{s}, \vect{t})` is such that :math:`X^i_{\vect{t}}`
  depends only on :math:`X^i_{\vect{s}}` and that this link does not depend on the component
  :math:`i`. In that case, :math:`\mat{\rho}(\vect{s}, \vect{t})` can be
  defined from the scalar function :math:`\rho(\vect{s}, \vect{t}) \in \Rset` by
  :math:`\mat{\rho}(\vect{s}, \vect{t}) = \rho(\vect{s}, \vect{t})\, \mat{I}_{\inputDim}`.
  We have :math:`\rho(\vect{s}, \vect{s}) = 1`.


Then, the covariance model is written as:

.. math::
    :label: def_cov

    \mat{C}(\vect{s}, \vect{t}) = \rho\left(\dfrac{\vect{s}}{\theta},
                                      \dfrac{\vect{t}}{\theta}\right)\,
                            \diag{\vect{\sigma}} \, \mat{R} \,
                            \diag{\vect{\sigma}}, \quad
                            \forall (\vect{s}, \vect{t}) \in \cD

or:

.. math::
    :label: def_cov_2

    \mat{C}(\vect{s}, \vect{t}) = \rho\left(\dfrac{\vect{s}}{\theta},
                                      \dfrac{\vect{t}}{\theta}
                                \right)\, \mat{C}^{spatial}(\vect{s}, \vect{t}) \quad
                            \forall (\vect{s}, \vect{t}) \in \cD

where:

- :math:`\vect{\theta} \in \Rset^{\sampleSize}` is the *scale* parameter,
- :math:`\vect{\sigma} \in \Rset^{\inputDim}` is the *amplitude* parameter,
- :math:`\mat{R} \in \cS_{\inputDim}^+(\Rset)` is the *spatial* correlation matrix,
- :math:`\mat{C}^{spatial}(\vect{s}, \vect{t}) = \diag{\vect{\sigma}} \, \mat{R} \, \diag{\vect{\sigma}}`
  is the *spatial* covariance matrix.

The library allows one to model a **nugget effect**. The nugget effect represents a noise observed in the output
values of a process. This noise may be, for example, a measurement noise coming from a sensor with finite precision.

The nugget effect is taken into account by modifying the scalar correlation function :math:`\rho` at
any point :math:`(\vect{s}, \vect{s})` by adding a term denoted :math:`\varepsilon_{nugget}`:

.. math::
    :label: cor_with_noise

      \rho_{nugget}(\vect{s}, \vect{t}) &  =
          \left |
          \begin{array}{l}
            \rho(\vect{s}, \vect{t}) \quad \mbox{if}  \quad \vect{s} \neq \vect{t}  \\
            1 + \varepsilon_{nugget} \quad \mbox{else}
          \end{array}
          \right.

Then, the nugget effect transforms the covariance function :math:`\mat{C}` into the covariance function :math:`\mat{C}_{nugget}` as follows:

.. math::
      :label: cov_with_noise

      \mat{C}_{nugget}(\vect{s}, \vect{t}) = \mat{C}(\vect{s}, \vect{t}) + \varepsilon_{nugget}  \mat{C}^{spatial} 1_{\vect{s} = \vect{t}}

Then, we have:

.. math::
      :label: Css

      \mat{C}_{nugget}(\vect{s}, \vect{s}) = (1+\varepsilon_{nugget})\mat{C}^{spatial}

which shows how the nugget factor :math:`\varepsilon_{nugget}` acts on the covariance function.

.. topic:: API:

    - Refer to :ref:`stochastic_process`.

.. topic:: Examples:

    - See :doc:`/auto_stochastic_processes/plot_create_stationary_covmodel`
    - See :doc:`/auto_stochastic_processes/plot_userdefined_covariance_model`
    - See :doc:`/auto_stochastic_processes/plot_covariance_model_nugget`

