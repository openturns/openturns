.. _gaussian_calibration:

Gaussian calibration
--------------------

Introduction
~~~~~~~~~~~~

We consider a computer model :math:`\vect{h}` (i.e. a deterministic function)
to calibrate:

.. math::

    \vect{z} = \vect{h}(\vect{x}, \vect{\theta}),

where

-  :math:`\vect{x} \in \Rset^{d_x}` is the input vector;

-  :math:`\vect{z} \in \Rset^{d_z}` is the output vector;

-  :math:`\vect{\theta} \in \Rset^{d_h}` are the unknown parameters of
   :math:`\vect{h}` to calibrate.

Let :math:`n \in \Nset` be the number of observations. 
The standard hypothesis of the probabilistic calibration is:

.. math::

    \vect{y}^i = \vect{z}^i + \vect{\varepsilon}^i,

for :math:`i=1,...,n` where :math:`\vect{\varepsilon}^i` is a random measurement error. 

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

In the remaining of this section, the input :math:`\vect{x}` is not involved 
anymore in the equations. 
This is why we simplify the equation into:

.. math::

    \vect{z} = \vect{h}(\vect{\theta}).

Bayesian calibration
~~~~~~~~~~~~~~~~~~~~

The bayesian calibration framework is based on two hypotheses.

The first hypothesis is that the parameter :math:`\vect{\theta}` has 
a known distribution, called the *prior* distribution, and denoted by :math:`p(\vect{\theta})`. 

The second hypothesis is that the output observations :math:`(\vect{y}^1, \ldots, \vect{y}^n)` 
are sampled from a known conditional distribution denoted by :math:`p(\vect{y} | \vect{\theta})`. 

For any :math:`\vect{y}\in\Rset^{d_z}` such that :math:`p(\vect{y})>0`, the Bayes theorem implies 
that the conditional distribution of :math:`\vect{\theta}` given :math:`\vect{y}` is:

.. math::

    p(\vect{\theta} | \vect{y}) = \frac{p(\vect{y} | \vect{\theta}) p(\vect{\theta})}{p(\vect{y})}

for any :math:`\vect{\theta}\in\Rset^{d_h}`. 

The denominator of the previous Bayes fraction is independent of :math:`\vect{\theta}`, so that 
the posterior distribution is proportional to the numerator:

.. math::

    p(\vect{\theta} | \vect{y}) \propto  p(\vect{y} | \vect{\theta}) p(\vect{\theta}).

for any :math:`\vect{\theta}\in\Rset^{d_h}`. 

In the gaussian calibration, the two previous distributions are assumed to be gaussian. 

More precisely, we make the hypothesis that the parameter :math:`\vect{\theta}`  
has the gaussian distribution:

.. math::

    \vect{\theta} \sim \mathcal{N}(\vect{\mu}, B),

where :math:`\vect{\mu}\in\Rset^{d_h}` is the mean of the gaussian prior distribution, 
which is named the *background* and :math:`B\in\Rset^{d_h \times d_h}` is the covariance 
matrix of the parameter.

Secondly, we make the hypothesis that the output observations have the conditional gaussian distribution:

.. math::

    \vect{y} | \vect{\theta} \sim \mathcal{N}(\vect{h}(\vect{\theta}), R),

where :math:`R\in\Rset^{d_z \times d_z}` is the covariance 
matrix of the output observations.

Posterior distribution
~~~~~~~~~~~~~~~~~~~~~~

Denote by :math:`\|\cdot\|_B` the Mahalanobis distance associated with the matrix 
:math:`B` :

.. math::

    \|\vect{\theta}-\vect{\mu} \|^2_B = (\vect{\theta}-\vect{\mu} )^T B^{-1} (\vect{\theta}-\vect{\mu} ),

for any :math:`\vect{\theta},\vect{\mu} \in \Rset^{d_h}`.
Denote by :math:`\|\cdot\|_R` the Mahalanobis distance associated with the matrix 
:math:`R` :

.. math::

    \|\vect{y}-H(\vect{\theta})\|^2_R = (\vect{y}-H(\vect{\theta}))^T R^{-1} (\vect{y}-H(\vect{\theta})).

for any :math:`\vect{\theta} \in \Rset^{d_h}` and any :math:`\vect{y} \in \Rset^{d_z}`. 
Therefore, the posterior distribution of :math:`\vect{\theta}` given the observations :math:`\vect{y}` is :

.. math::

    p(\vect{\theta}|\vect{y}) \propto \exp\left( -\frac{1}{2} \left( \|\vect{y}-H(\vect{\theta})\|^2_R 
    + \|\vect{\theta}-\vect{\mu} \|^2_B \right) \right)

for any :math:`\vect{\theta}\in\Rset^{d_h}`. 

MAP estimator
~~~~~~~~~~~~~

The maximum of the posterior distribution of :math:`\vect{\theta}` given the observations :math:`\vect{y}` is 
reached at :

.. math::

    \hat{\vect{\theta}} = arg min_{\vect{\theta}\in\Rset^{d_h}} \frac{1}{2} \left( \|\vect{y} - H(\vect{\theta})\|^2_R 
    + \|\vect{\theta}-\vect{\mu} \|^2_B \right).

It is called the *maximum a posteriori posterior* estimator or 
*MAP* estimator. 

Regularity of solutions of the Gaussian Calibration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The gaussian calibration is a tradeoff, so that the 
second expression acts as a *spring* which pulls the parameter 
:math:`\vect{\theta}` closer to the background :math:`\vect{\mu}` 
(depending on the "spring constant" :math:`B`)
meanwhile getting as close a possible to the observations. 
Depending on the matrix :math:`B`, the computation may have 
better regularity properties than the plain non linear least squares problem. 

Non Linear Gaussian Calibration : 3DVAR
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The cost function of the gaussian nonlinear calibration problem is :

.. math::

    C(\vect{\theta}) = \frac{1}{2}\|\vect{y}-H(\vect{\theta})\|^2_R 
    + \frac{1}{2}\|\vect{\theta}-\vect{\mu} \|^2_B

for any :math:`\vect{\theta}\in\Rset^{d_h}`. 

The goal of the non linear gaussian calibration is to find the 
value of :math:`\vect{\theta}` which minimizes the cost function :math:`C`. 
In general, this involves using a nonlinear unconstrained optimization solver. 

Let :math:`J \in \Rset^{n \times d_h}` be the Jacobian matrix made of the 
partial derivatives of :math:`\vect{h}` with respect to :math:`\vect{\theta}`:

.. math::

       J(\vect{\theta}) = \frac{\partial \vect{h}}{\partial \vect{\theta}}.

The Jacobian matrix of the cost function :math:`C` can be expressed 
depending on the matrices :math:`R` and :math:`B` and the Jacobian matrix 
of the function :math:`h`:

.. math::

    \frac{d }{d\vect{\theta}} C(\vect{\theta}) 
    = B^{-1} (\vect{\theta}-\vect{\mu}) + J(\vect{\theta})^T R^{-1} (H(\vect{\theta}) - \vect{y})

for any :math:`\vect{\theta}\in\Rset^{d_h}`. 

The Hessian matrix of the cost function is 

.. math::

    \frac{d^2 }{d\vect{\theta}^2} C(\vect{\theta}) 
    = B^{-1}  + J(\vect{\theta})^T R^{-1} J(\vect{\theta})

for any :math:`\vect{\theta}\in\Rset^{d_h}`. 

If the covariance matrix :math:`B` is positive definite, 
then the Hessian matrix of the cost function is positive definite. 
Under this hypothesis, the solution of the nonlinear gaussian calibration is unique. 

Solving the Non Linear Gaussian Calibration Problem
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The implementation of the resolution of the gaussian non linear calibration 
problem involves the Cholesky decomposition of the covariance matrices :math:`B` 
and :math:`R`. 
This allows to transform the sum of two Mahalanobis distances into a single 
euclidian norm. 
This leads to a classical non linear least squares problem. 

Linear Gaussian Calibration : bayesian BLUE
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We make the hypothesis that :math:`h` is linear with respect to :math:`\vect{\theta}`,
i.e., for any :math:`\vect{\theta}\in\Rset^{d_h}`, we have:

.. math::

    h(\vect{\theta}) = h(\vect{\mu}) + J(\vect{\theta}-\vect{\mu} ),

where :math:`J` is the constant Jacobian matrix of :math:`h`. 

Let :math:`A` be the matrix:

.. math::

    A^{-1} = B^{-1} + J^T R^{-1} J.

We denote by :math:`K` the Kalman matrix:

.. math::

    K = A J^T R^{-1}.

The maximum of the posterior distribution of :math:`\vect{\theta}` given the 
observations :math:`\vect{y}` is:

.. math::

    \hat{\vect{\theta}} = \vect{\mu} + K (\vect{y} - H(\vect{\mu})). 

It can be proved that:

.. math::

    p(\vect{\theta} | \vect{y}) \propto 
    \exp\left(\frac{1}{2} (\vect{\theta} - \hat{\vect{\theta}})^T A^{-1} (\vect{\theta} - \hat{\vect{\theta}}) \right)

for any :math:`\vect{\theta}\in\Rset^{d_h}`. 

This implies:

.. math::

    \hat{\vect{\theta}} \sim \mathcal{N}(\vect{\theta},A)

.. topic:: API:

    - See :class:`~openturns.GaussianLinearCalibration`
    - See :class:`~openturns.GaussianNonLinearCalibration`

.. topic:: Examples:

    - See :doc:`/examples/calibration/calibration_flooding`
    - See :doc:`/examples/calibration/calibration_chaboche`
    - See :doc:`/examples/calibration/calibration_deflection_tube`
    - See :doc:`/examples/calibration/calibration_logistic`

.. topic:: References:

    - N. H. Bingham and John M. Fry (2010). *Regression, Linear Models in Statistics*, Springer Undergraduate Mathematics Series. Springer.
    - S. Huet, A. Bouvier, M.A. Poursat, and E. Jolivet (2004). *Statistical Tools for Nonlinear Regression*, Springer.
    - C. E. Rasmussen and C. K. I. Williams (2006), *Gaussian Processes for Machine Learning*, The MIT Press.

