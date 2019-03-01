.. _kriging_robust:

Robust estimation of kriging parameters
---------------------------------------

The robust estimation of :math:`\vect{\theta} \in \Rset^{n_\theta}` hyperparameters
according to [gu2016]_ apply in the following conditions:

- The output is of dimension :math:`d=1`
- The amplitude :math:`\sigma` is scalar
- The correlation function :math:`R` is the identity
- The active parameters are :math:`\sigma` and :math:`\vect{\theta}`

The model likelihood writes:

.. math::

    \cL(\beta, \sigma, \vect{\theta}, \epsilon;(\vect{x}_k, \vect{y}_k)_{1 \leq k \leq N}) = \left(\frac{1}{2\pi \sigma^2}\right)^{\frac{N}{2}} |\det (\Sigma_{\vect{\theta}} + \epsilon \textit{I})|^{-\frac{1}{2}} \exp\Big\{ -\dfrac{1}{2\sigma^2}\Tr{\left( \vect{y}-\textbf{F} \beta \right)} (\Sigma_{\vect{\theta}}+\epsilon \textit{I})^{-1}  \left( \vect{y}-\textbf{F} \beta \right) \Big\}


The maximum likelihood estimator of :math:`\beta` writes:

.. math::

    \hat{\beta} = (\Tr{F}(\Sigma_{\vect{\theta}}+\epsilon \textit{I})^{-1} F)^{-1} \Tr{F}(\Sigma_{\vect{\theta}}+\epsilon \textit{I})^{-1} \vect{y}


The maximum likelihood estimator of :math:`\sigma` writes:

.. math::

    \hat{\sigma} = \sqrt{\frac{\Tr{\vect{y}} \tilde{\Sigma}_{\vect{\theta},\epsilon} \vect{y}}{N-n_1}}


Then the reduced likelihood writes:

.. math::

    \cL_{res}(\vect{\theta}, \epsilon;(\vect{x}_k, \vect{y}_k)_{1 \leq k \leq N}) = \frac{(N-n_1)^{\frac{N}{2}}}{(2\pi)^{\frac{N}{2}}} (\Tr{\vect{y}} \tilde{\Sigma}_{\vect{\theta},\epsilon} \vect{y})^{-\frac{N}{2}} |\det (\Sigma_{\vect{\theta}} + \epsilon \textit{I})|^{-\frac{1}{2}} \exp\Big\{ -\frac{N-n_1}{2} \Big\}


We chose the integrated likelihood:

.. math::

    \cL_{int}(\vect{\theta}, \epsilon;(\vect{x}_k, \vect{y}_k)_{1 \leq k \leq N}) = |\det (\Sigma_{\vect{\theta}} + \epsilon \textit{I})|^{-\frac{1}{2}} |\det\Big\{ \Tr{F}(\Sigma_{\vect{\theta}}+\epsilon \textit{I})^{-1} F \Big\}| (\Tr{\vect{y}} \tilde{\Sigma}_{\vect{\theta},\epsilon} \vect{y})^{-\frac{N-n_1}{2}}


The Gu method consists in penalizing the integrated likelihood by a chosen prior :math:`f(\vect{\theta}, \epsilon)`:

.. math::

    \cL_{int}(\vect{\theta}, \epsilon;\vect{y}) f(\vect{\theta}, \epsilon)

We actually minimize the penalized integrated likelihood in log scale:

.. math::

    log \cL_{int}(\vect{\theta}, \epsilon;(\vect{x}_k, \vect{y}_k)_{1 \leq k \leq N}) + log f(\vect{\theta}, \epsilon) = -log |\det (\Sigma_{\vect{\theta}} + \epsilon \textit{I})| -log |\det\Big\{ \Tr{F}(\Sigma_{\vect{\theta}}+\epsilon \textit{I})^{-1} F \Big\}| - (N - n_1) log (\Tr{\vect{y}} \tilde{\Sigma}_{\vect{\theta},\epsilon} \vect{y}) + log f(\vect{\theta}, \epsilon)


Jointly Robust Prior
~~~~~~~~~~~~~~~~~~~~

The first prior proposed by Gu writes:

.. math::

    f_J(\theta, \epsilon) = c \left( \sum_{a=1}^n C_a \vect{\theta}_a + \epsilon \right)^{b_1} \exp \left(-b \sum{a=1}^n C_a \vect{\theta}^{-1} \right) \prod{a=1}^n \vect{\theta}_a^{-2}

with :math:`b_0 > 0`, :math:`b_1 \in (0,1)` and :math:`b = b_0 N^{-\frac{1}{n}} (n + b_1)`

We use the formulation from [gu2018]_ (page 12) as the maximum gap between design points instead of the mean gap from [gu2016]_:

.. math::

    C_a = |\max_{1 \leq k \leq N} x_{k,a} - \min_{1 \leq k \leq N} x_{k,a}| N^{-\frac{1}{n}}

Reference Prior
~~~~~~~~~~~~~~~

The second prior proposed by Gu writes:

.. math::

    \mathcal{I}(\vect{\theta}, \epsilon)_{a,b} = Tr \left[  \tilde{\Sigma}_{\vect{\theta},\epsilon} (\frac{\delta}{\delta \theta_a} \Sigma_{\vect{\theta}}) \tilde{\Sigma}_{\vect{\theta},\epsilon} (\frac{\delta}{\delta \theta_b} \Sigma_{\vect{\theta}}) \right]

.. math::

    f_R(\vect{\theta}, \epsilon) = \sqrt{\det \mathcal{I}(\vect{\theta}, \epsilon)}

Flat prior
~~~~~~~~~~

Allows to use the integrated likelihood with no penalization.

Scale reparametrization
~~~~~~~~~~~~~~~~~~~~~~~

During the penalized likelihood optimization the scale Gu proposes the following parametrizations of the scale:

- Standard parametrization: :math:`\vect{\theta} = (\theta_1, \dots, \theta_n)`
- Inverse parametrization: :math:`\vect{\mu} = (\theta_1^{-1}, \dots, \theta_n^{-1})`
- Log-inverse parametrization: :math:`\vect{\xi} = (-log(\theta_1), \dots, -log(\theta_n))`


.. topic:: API:

    - See :class:`~openturns.KrigingAlgorithm`
    - See :class:`~openturns.GeneralLinearModelAlgorithm`


.. topic:: Examples:

    - See :doc:`/examples/meta_modeling/kriging_robust`


.. topic:: References:

    - [gu2016]_
    - [gu2018]_

