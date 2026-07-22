%feature("docstring") OT::Normal
R"RAW(Normal distribution.

Its probability density function is defined as:

.. math::

    f_{\vect{X}}(\vect{x}) = \frac{1}
             {\displaystyle (2\pi)^{\frac{n}{2}}
                            (\mathrm{det}\mat{\Sigma})^{\frac{1}{2}}}
             \displaystyle exp \left(-\frac{1}{2}\Tr{(\vect{x} - \vect{\mu})}
                                     \mat{\Sigma}^{-1}(\vect{x} - \vect{\mu})\right)

with :math:`\mat{\Sigma} = \diag(\vect{\sigma}) \mat{R} \diag(\vect{\sigma})`.
Both :math:`\mat{R}` and :math:`\mat{\Sigma}` are symmetric positive definite.

The first moments of the distribution are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{\vect{X}} & = & \vect{\mu} \\
        \Var{\vect{X}} & = & \mat{\Sigma}
    \end{eqnarray*}

Available constructors:
    Normal(*d*)

    Normal(*mu, sigma*)

    Normal(*mu, sigma, R*)

    Normal(*mu, Sigma*)

Parameters
----------
d : int, :math:`d \geq 1`
    Dimension.
    
    Default value is 1.

mu : float :math:`\mu`, sequence of float, :math:`\vect{\mu} \in \Rset^d`
    Mean.

    Default value is 0.0.

sigma : float :math:`\sigma`, sequence of float, :math:`\vect{\sigma} \in \Rset^{*d}_+`
    Scale parameter.

    Default value is 1.0.

R : :class:`~openturns.CorrelationMatrix` :math:`\mat{R} \in  \cM_{d \times d}(\Rset)`
    Correlation matrix.

    Default value is the identity matrix.

Sigma : :class:`~openturns.CovarianceMatrix` :math:`\mat{\Sigma} \in  \cM_{d \times d}(\Rset)`
    Covariance matrix.

Notes
-----
In the first usage, a scalar centered and unit-variance normal distribution is built.

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> distribution = ot.Normal(0.0, 1.0)

Draw a sample:

>>> sample = distribution.getSample(5)

Create a multivariate normal distribution with constant :math:`\sigma` and identity correlation matrix:

>>> dimension = 5
>>> mu = ot.Point(dimension)
>>> sigma = ot.Point(dimension, 0.1)
>>> R = ot.CorrelationMatrix(dimension)
>>> distribution = ot.Normal(mu, sigma, R)
>>> print('distribution=', distribution)
distribution= Normal(mu = [0,0,0,0,0], sigma = [0.1,0.1,0.1,0.1,0.1], R = 5x5
[[ 1 0 0 0 0 ]
 [ 0 1 0 0 0 ]
 [ 0 0 1 0 0 ]
 [ 0 0 0 1 0 ]
 [ 0 0 0 0 1 ]])

Create a multivariate normal distribution from a covariance matrix:

>>> Sigma = ot.CovarianceMatrix(dimension)
>>> Sigma[1, 1] = 3.0
>>> Sigma[0, 2] = -0.5
>>> Sigma[3, 4] = -0.2
>>> distribution = ot.Normal(mu, Sigma)
>>> print('distribution=', distribution)
distribution= Normal(mu = [0,0,0,0,0], sigma = [1,1.73205,1,1,1], R = 5x5
[[  1    0   -0.5  0    0   ]
 [  0    1    0    0    0   ]
 [ -0.5  0    1    0    0   ]
 [  0    0    0    1   -0.2 ]
 [  0    0    0   -0.2  1   ]])
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Normal::computeCDF
R"RAW(Compute the cumulative distribution function.

See the class notes for the algorithm selection strategy.

Parameters
----------
x : sequence of float
    Point in :math:`\Rset^d`.

Returns
-------
F : float
    CDF value at *x*.

Notes
-----
The algorithm depends on the dimension:

- dim 1: direct evaluation via the complementary error function;
- any dim, independent copula: product of the marginal CDFs;
- dim 2: specialized 2D routine (:meth:`~openturns.DistFunc.pNormal2D`);
- dim 3: specialized 3D routine (:meth:`~openturns.DistFunc.pNormal3D`);
- dim 4-*Normal-SmallDimension* (default 6): tensor-product Gauss-Kronrod
  quadrature on :math:`N^k` nodes;
- dim > *Normal-SmallDimension*: quasi-Monte Carlo Genz algorithm with
  *Genz-DefaultSampleSize* Sobol' realizations.

)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Normal::computeProbability
R"RAW(Compute the probability content of an interval.

See the class notes for the algorithm selection strategy.

Parameters
----------
interval : :class:`~openturns.Interval`
    Interval over which the probability is computed.

Returns
-------
P : float
    Probability of the interval.

Notes
-----
The algorithm depends on the dimension:

- dim 1: generic 1D algorithm;
- any dim, independent copula: product of the marginal probabilities;
- dim 2: specialized 2D routine (:meth:`~openturns.DistFunc.pNormal2D`);
- dim 3: specialized 3D routine (:meth:`~openturns.DistFunc.pNormal3D`);
- dim >= 4: quasi-Monte Carlo Genz algorithm with *Genz-DefaultSampleSize*
  Sobol' realizations.

)RAW"
