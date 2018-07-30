.. _bayesian_calibration:

Bayesian calibration
--------------------

We consider a computer model :math:`h` (*i.e.* a deterministic function)
to calibrate:

.. math::

   \begin{aligned}
       \vect{z} = h(\vect{x}, \vect{\theta}_h),
     \end{aligned}

where

-  :math:`\vect{x} \in \Rset^{d_x}` is the input vector;

-  :math:`\vect{z} \in \Rset^{d_z}` is the output vector;

-  :math:`\vect{\theta}_h \in \Rset^{d_h}` are the unknown parameters of
   :math:`h` to calibrate.

Our goal here is to estimate :math:`\vect{\theta}_h`, based on a certain
set of :math:`n` inputs :math:`(\vect{x}^1, \ldots, \vect{x}^n)` (an
experimental design) and some associated observations
:math:`(\vect{y}^1, \ldots, \vect{y}^n)` which are regarded as the
realizations of some random vectors
:math:`(\vect{Y}^1, \ldots, \vect{Y}^n)`, such that, for all :math:`i`,
the distribution of :math:`\vect{Y}^i` depends on
:math:`\vect{z}^i = h(\vect{x}^i, \vect{\theta}_h)`. Typically,
:math:`\vect{Y}^i = \vect{z}^i + \vect{\varepsilon}^i` where
:math:`\vect{\varepsilon}^i` is a random measurement error.

For the sake of clarity, lower case letters are used for both random
variables and realizations in the following (the notation does not
distinguish the two anymore), as usual in the bayesian literature.

In fact, the bayesian procedure which is implemented allows to infer
some unknown parameters :math:`\vect{\theta}\in\Rset^{d_\theta}` from
some data :math:`\mat{y} = (\vect{y}^1, \ldots, \vect{y}^n)` as soon as
the conditional distribution of each :math:`\vect{y}^i` given
:math:`\vect{\theta}` is specified. Therefore :math:`\vect{\theta}` can
be made up with some computer model parameters :math:`\vect{\theta}_h`
together with some others :math:`\vect{\theta}_\varepsilon`:
:math:`\vect{\theta} = \Tr{(\Tr{\vect{\theta}_h}, \Tr{\vect{\theta}_\varepsilon})}`.
For example, :math:`\vect{\theta}_\varepsilon` may represent the unknown
standard deviation :math:`\sigma` of an additive centered gaussian
measurement error affecting the data (see the example hereafter).
Besides the procedure can be used to estimate the parameters of a
distribution from direct observations (no computer model to calibrate:
:math:`\vect{\theta} = \vect{\theta}_\varepsilon`).

More formally, the likelihood :math:`L(\mat{y} | \vect{\theta})` is
defined by, firstly, a family
:math:`\{\cP_{\vect{w}}, \vect{w} \in \Rset^{d_w}\}` of probability
distributions parametrized by :math:`\vect{w}`, which is specified in
practice by a conditional distribution :math:`f(.|\vect{w})` given
:math:`\vect{w}` (:math:`f` is a PDF or a probability mass function),
and, secondly, a function
:math:`g:\Rset^{d_\theta} \longrightarrow \Rset^{n\,d_w}` such that
:math:`g(\theta) = \Tr{(\Tr{g^1(\vect{\theta})}, \ldots, \Tr{g^n(\vect{\theta})})}`
which enables to express the parameter :math:`\vect{w}^i` of the i-th
observation :math:`\vect{y}^i \sim f(.|\vect{w}^i)` in function of
:math:`\vect{\theta}`: :math:`g^i(\vect{\theta}) = \vect{w}^i` thus
:math:`\vect{y}^i \sim f(.|g^i(\vect{\theta}))` and

.. math::

   \begin{aligned}
       L(\mat{y} | \vect{\theta}) = \prod_{i=1}^n f(\vect{y}^i|g^i(\vect{\theta})).
     \end{aligned}

Considering the issue of the calibration of some computer model
parameters :math:`\vect{\theta}_h`, the full statistical model can be
seen as a two-level hierarchical model, with a single level of latent
variables :math:`\vect{z}`. A classical example is given by the
nonlinear Gaussian regression model:

.. math::

   \begin{aligned}
       y_i &=& h(\vect{x}_i|\vect{\theta}_h) + \varepsilon_i,
       \mbox{ where } \varepsilon_i \stackrel{i.i.d.}{\sim} \cN(0, \sigma^2),
       \quad i = 1,\ldots, n.
     \end{aligned}

It can be implemented with :math:`f(.|\Tr{(\mu, \sigma)})` the PDF of
the Gaussian distribution :math:`\cN(\mu, \sigma^2)`, with
:math:`g^i(\vect{\theta}) = \Tr{(h(\vect{x}^i, \vect{\theta}_h), \:\sigma)}`,
and with :math:`\vect{\theta} = \vect{\theta}_h`, respectively
:math:`\vect{\theta} = \Tr{(\Tr{\vect{\theta}_h}, \sigma)}`, if
:math:`\sigma` is considered known, respectively unknown.

Given a distribution modelling the uncertainty on :math:`\vect{\theta}`
prior to the data, Bayesian inference is used to perform the inference
of :math:`\vect{\theta}`, hence the name Bayesian calibration.

Contrary to the maximum likelihood approach described in :ref:`maximum_likelihood`, which
provides a single ‘best estimate’ value :math:`\hat{\vect{\theta}}`,
together with confidence bounds accounting for the uncertainty remaining
on the true value :math:`\vect{\theta}`, the Bayesian approach derives a
full distribution of possible values for :math:`\vect{\theta}` given the
available data :math:`\mat{y}`. Known as the *posterior distribution* of
:math:`\vect{\theta}` given the data :math:`\mat{y}`, its density can be
expressed according to Bayes’ theorem:

.. math::
  :label: postdistribution

   \begin{aligned}
       \pi(\vect{\theta} | \mat{y}) &=& \frac{L(\mat{y} | \vect{\theta}) \times \pi(\vect{\theta})}{m(\mat{y})},
     \end{aligned}

where

-  :math:`L(\mat{y} | \vect{\theta})` is the (data) likelihood;

-  :math:`\pi(\vect{\theta})` is the so-called *prior distribution* of
   :math:`\vect{\theta}` (with support :math:`\Theta`), which encodes
   all possible :math:`\vect{\theta}` values weighted by their prior
   probabilities, before consideration of any experimental data (this
   allows for instance to incorporate expert information or known
   physical constraints on the calibration parameter)

-  :math:`m(\mat{y})` is the marginal likelihood:

   .. math::

      \begin{aligned}
            m(\mat{y}) &=& \displaystyle\int_{\vect{\theta}\in\Theta} L(\mat{y} | \vect{\theta}) \pi(\vect{\theta}) d\vect{\theta},
          \end{aligned}

which is the necessary normalizing constant ensuring that the
posterior density integrates to :math:`1`.

Except in very simple cases, :eq:`postdistribution` has, in general,
no closed form. Thus, it must be approximated, either using numerical
integration when the parameter space dimension :math:`d_\theta` is low,
or more generally through stochastic sampling techniques known as
Monte-Carlo Markov-Chain (MCMC) methods. See :ref:`metropolis_hastings`.

.. topic:: API:

    - See :class:`~openturns.RandomWalkMetropolisHastings`


.. topic:: Examples:

    - See :doc:`/examples/data_analysis/bayesian_calibration`

.. topic:: References:

    - Berger, J.O. (1985). *Statistical Decision Theory and Bayesian Analysis*, Springer.
    - Marin J.M. \& Robert C.P. (2007) *Bayesian Core: A Practical Approach to Computational Bayesian Statistics*, Springer.
