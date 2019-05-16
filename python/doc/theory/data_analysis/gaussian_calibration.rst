.. _code_calibration:

Gaussian calibration
--------------------

Introduction
~~~~~~~~~~~~

We consider a computer model :math:`\vect{h}` (*i.e.* a deterministic function)
to calibrate:

.. math::

   \begin{aligned}
       \vect{z} = \vect{h}(\vect{x}, \vect{\theta}),
   \end{aligned}

where

-  :math:`\vect{x} \in \Rset^{d_x}` is the input vector;

-  :math:`\vect{z} \in \Rset^{d_z}` is the output vector;

-  :math:`\vect{\theta} \in \Rset^{d_h}` are the unknown parameters of
   :math:`\vect{h}` to calibrate.

In the remaining of this section, the input :math:`\vect{x}` is not involved 
anymore in the expression. 
This is why we simplify the equation into:

.. math::

   \begin{aligned}
       \vect{z} = \vect{h}(\vect{\theta}).
   \end{aligned}

The standard hypothesis of the probabilistic calibration is:

.. math::

   \begin{aligned}
       \vect{Y}^i = \vect{z}^i + \vect{\varepsilon}^i,
   \end{aligned}

where :math:`\vect{\varepsilon}^i` is a random measurement error. 

The goal of gaussian calibration is to estimate :math:`\vect{\theta}`, based on 
observations of :math:`n` inputs :math:`(\vect{x}^1, \ldots, \vect{x}^n)` 
and the associated :math:`n` observations of the output  
:math:`(\vect{y}^1, \ldots, \vect{y}^n)`. 
In other words, the calibration process reduces the discrepancy between 
the observations :math:`(\vect{y}^1, \ldots, \vect{y}^n)` and the 
predictions :math:`\vect{h}(\vect{\theta})`. 
Given that :math:`(\vect{y}^1, \ldots, \vect{y}^n)` are realizations of a 
random variable, the estimate of :math:`\vect{\theta}`, denoted by 
:math:`\hat{\vect{\theta}}`, is also a random variable. 
Hence, the secondary goal of calibration is to estimate the distribution of 
:math:`\hat{\vect{\theta}}` representing the uncertainty of the calibration 
process. 

Bayesian calibration
~~~~~~~~~~~~~~~~~~~~

The bayesian calibration framework is based on two hypotheses.

The first hypothesis is that the output observations :math:`(\vect{y}^1, \ldots, \vect{y}^n)` 
are sampled from a known distribution denoted by :math:`p(\vect{y} | \vect{\theta}`. 

The second hypothesis is that the parameter :math:`\vect{\theta}` has 
a known distribution, called the *prior* distribution and denoted by :math:`p(\vect{\theta})`. 

For any :math:`\vect{y}\in\Rset^{d_z}` such that :math:`p(\vect{y})>0`, the Bayes theorem implies 
that the distribution of :math:`\vect{\theta}` given :math:`\vect{y}` is:

.. math::

    p(\vect{\theta} | \vect{y}) = \frac{p(\vect{y} | \vect{\theta}) p(\vect{\theta})}{p(\vect{y})}

for any :math:`\vect{\theta}\in\Rset^{d_h}`. 

The denominator of the Bayes fraction is independent of :math:`\vect{\theta}`, so that 
the posterior distribution is proportional to the numerator:

.. math::

    p(\vect{\theta} | \vect{y}) \propto  p(\vect{y} | \vect{\theta}) p(\vect{\theta}).

for any :math:`\vect{\theta}\in\Rset^{d_h}`. 

In the gaussian calibration, the two previous distributions are assumed to be gaussian. 

More precisely, we make the hypothesis that the output observations have the gaussian distribution:

.. math::

    \vect{y} | \vect{\theta} \sim \mathcal{N}(\vect{h}(\vect{\theta}), R),

where :math:`R\in\Rset^{d_z \times d_z}` is the covariance 
matrix of the output observations.

Secondly, we make the hypothesis that the parameter :math:`\vect{\theta}`  
has the gaussian distribution:

.. math::

    \vect{\theta} \sim \mathcal{N}(\vect{\mu}, B),

where :math:`\vect{\mu}\in\Rset^{d_h}` is the mean of the gaussian prior distribution, 
which is named the *background* and :math:`B\in\Rset^{d_h \times d_h}` is the covariance 
matrix of the parameter.

.. topic:: API:

    - See :class:`~openturns.GaussianLinearCalibration`
    - See :class:`~openturns.GaussianNonLinearCalibration`

.. topic:: References:

    - N. H. Bingham and John M. Fry (2010). *Regression, Linear Models in Statistics*, Springer Undergraduate Mathematics Series. Springer.
    - S. Huet, A. Bouvier, M.A. Poursat, and E. Jolivet (2004). *Statistical Tools for Nonlinear Regression*, Springer.
    - C. E. Rasmussen and C. K. I. Williams (2006), *Gaussian Processes for Machine Learning*, The MIT Press.

