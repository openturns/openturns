.. _sensitivity_form:

Sensitivity Factors from FORM method
------------------------------------

Sensitivity Factors are evaluated under the following context:
:math:`\inputRV` denotes a random input vector, representing the
sources of uncertainties, :math:`\pdf` its joint density probability,
:math:`\model` the limit state function of the model. We denote by:

  .. math::
      :label: event

      \cD_f = \{\vect{x} \in \Rset^\inputDim \, / \, \model(\vect{x}) \le 0\}

the event considered here and :math:`{\model(\inputRV) = 0}`
its boundary (also called limit state surface).
The probability content of the event :math:`\cD_f` is :math:`P_f`:

  .. math::
    :label: PfX11

    P_f = \int_{\model(\inputRV) \le 0}  \pdf\, d\vect{x}.

In this context, the probability :math:`P_f` can often be
efficiently estimated by FORM or SORM approximations.
The FORM importance factors offer a way to analyze the sensitivity of
the probability the realization of the event with respect to the
parameters of the probability distribution of :math:`\inputRV`.

A sensitivity factor is defined as the derivative of the Hasofer-Lind
reliability index with respect to the parameter :math:`\theta`. The
parameter :math:`\theta` is a parameter in a distribution of the
random vector :math:`\inputRV`.
If :math:`\vect{\theta}` represents the vector of all the parameters
of the distribution of :math:`\inputRV` which appear in the definition
of the isoprobabilistic transformation :math:`T`, and
:math:`\vect{U}_{\vect{\theta}}^{*}` the design point associated to the event
considered in :eq:`event` in the :math:`U`-space, and if the mapping of the limit
state function by the :math:`T` is denoted by:

  .. math::

      h(\vect{U}\,,\,\vect{\theta}) =  \model \circ T^{-1}(\vect{U}\,,\,\vect{\theta}),

then the sensitivity factors vector is defined as:

  .. math::

      \nabla_{\vect{\theta}} \beta_{HL} =  \displaystyle +\frac{1}
      {||\nabla_{\vect{\theta}} h(U_{\vect{\theta}}^{*})||} \nabla_{\vect{u}} h(U_{\vect{\theta}}^{*}).

The sensitivity factors indicate the importance on the Hasofer-Lind
reliability index (refer to ) of the value of the parameters used to
define the distribution of the random vector :math:`\inputRV`.

In other words, the FORM sensitivity factors rank the importance of
the parameters of the input components in the realization of the event defined in :eq:`event`.


.. topic:: API:

    - See :class:`~openturns.FORM`
    - See :class:`~openturns.SORM`


.. topic:: Examples:

    - See :doc:`/auto_reliability/reliability_analysis/plot_estimate_probability_form`


.. topic:: References:

    - O. Ditlevsen, H.O. Madsen, 2004, "Structural reliability methods", Department of mechanical engineering technical university of Denmark - Maritime engineering, internet publication.

