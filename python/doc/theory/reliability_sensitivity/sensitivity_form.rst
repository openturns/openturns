Sensitivity Factors from FORM method
------------------------------------

| Sensitivity Factors are evaluated under the following context:
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
    :label: PfX11

    P_f = \int_{g(\vect{X}\,,\,\vect{d}) \le 0}  \pdf\, d\vect{x}.

| In this context, the probability :math:`P_f` can often be
  efficiently estimated by FORM or SORM approximations.
| The FORM importance factors offer a way to analyse the sensitivity of
  the probability the realization of the event with respect to the
  parameters of the probability distribution of :math:`\vect{X}`.

| A sensitivity factor is defined as the derivative of the Hasofer-Lind
  reliability index with respect to the parameter :math:`\theta`. The
  parameter :math:`\theta` is a parameter in a distribution of the
  random vector :math:`\vect{X}`.
| If :math:`\vect{\theta}` represents the vector of all the parameters
  of the distribution of :math:`\vect{X}` which appear in the definition
  of the isoprobabilistic transformation :math:`T`, and
  :math:`U_{\vect{\theta}}^{*}` the design point associated to the event
  considered in the :math:`U`-space, and if the mapping of the limit
  state function by the :math:`T` is noted
  :math:`G(\vect{U}\,,\,\vect{\theta}) =  g[T^{-1}(\vect{U}\,,\,\vect{\theta}), \vect{d}]`,
  then the sensitivity factors vector is defined as:

  .. math::

      \nabla_{\vect{\theta}} \beta_{HL} =  \displaystyle +\frac{1}{||\nabla_{\vect{\theta}} G(U_{\vect{\theta}}^{*}, \vect{d})||} \nabla_{\vect{u}} G(U_{\vect{\theta}}^{*}, \vect{d}).

The sensitivity factors indicate the importance on the Hasofer-Lind
reliability index (refer to ) of the value of the parameters used to
define the distribution of the random vector :math:`\vect{X}`.

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
:math:`P_f` is the threshold exceedance probability, defined as:
:math:`P_f     =       P(Z \geq z_s) = \int_{g(\vect{X}\,,\,\vect{d}) \le 0}  \pdf\, d\vect{x}`.
Thus, the FORM sensitivity factors offer a way to rank the importance of
the parameters of the input components with respect to the threshold
exceedance by the quantity of interest :math:`Z`. They can be seen as a
specific sensitivity analysis technique dedicated to the quantity Z around
a particular threshold rather than to its variance.


.. topic:: API:

    - See :class:`~openturns.FORM`
    - See :class:`~openturns.SORM`


.. topic:: Examples:

    - See :ref:`examples/reliability_sensitivity/estimate_probability_form.ipynb`


.. topic:: References:

    - O. Ditlevsen, H.O. Madsen, 2004, "Structural reliability methods", Department of mechanical engineering technical university of Denmark - Maritime engineering, internet publication.

