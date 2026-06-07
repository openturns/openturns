%feature("docstring") OT::Student
R"RAW(Student distribution.

Its probability density function is defined as:

.. math::

    f_X(x) = \frac{\Gamma \left(\frac{\nu + d}{2}\right)}
                  {(\pi \nu) ^ {\frac{d}{2}} \Gamma \left(\frac{\nu}{2}\right)}
             \frac{\left| \mathrm{det}(\mat{R}) \right| ^ {-1/2}}
                  {\prod_{k = 1} ^ d \sigma_k}
             \left(1 + \frac{\vect{z} ^ t \mat{R} ^ {-1} \vect{z}}{\nu}
             \right) ^ {-\frac{\nu + d}{2}}, \quad x \in \Rset

with :math:`\nu > 0, d \geq 1, \mat{R} \in  \cM_{d \times d}(\Rset),
\sigma_k > 0, k=1, \dots, d` , and
where :math:`\vect{z}=\mat{\Delta}^{-1}\left(\vect{x}-\vect{\mu}\right)`, 
:math:`\mat{\Delta} = \mat{\mathrm{diag}}(\vect{\sigma})` and :math:`\Gamma`
denotes Euler's Gamma function :meth:`~openturns.SpecFunc.Gamma`.

with :math:`\mat{\Sigma} = \diag(\vect{\sigma}) \mat{R} \diag(\vect{\sigma})`.
Both :math:`\mat{R}` and :math:`\mat{\Sigma}` are symmetric positive definite.

The first moments of the distribution are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \vect{\Expect{X}} & = & \vect{\mu} \quad \text{ if }\nu>1\\
        \mat{\Cov{X}} & = & \displaystyle \frac{\nu}{\nu-2}
                            \Tr{\mat{\Delta}}\,\mat{R}\,\mat{\Delta}
                          \quad \text{ if }\nu>2
    \end{eqnarray*}

Available constructors:
    Student(*nu, d*)

    Student(*nu, mu, sigma*)

    Student(*nu, mu, sigma, R*)

    Student(*nu, mu, Sigma*)

Parameters
----------
d : int, :math:`d \geq 1`
    Dimension.

    Default value is 1.

nu : float, :math:`\nu > 0`
    Generalized number of degrees of freedom.

    Default value is 3.0.

mu : float :math:`\mu`, sequence of float, :math:`\vect{\mu} \in \Rset^d`
    Location parameter.

    Default value  is 0.0.

sigma : float :math:`\sigma`, sequence of float, :math:`\vect{\sigma} \in \Rset^{*d}_+`
    Scale parameter.

    Default value  is 1.0.

R : :class:`~openturns.CorrelationMatrix` :math:`\mat{R} \in  \cM_{d \times d}(\Rset)`
    Shape matrix.

    Default value is the identity matrix.

Sigma : :class:`~openturns.CovarianceMatrix` :math:`\mat{\Sigma} \in  \cM_{d \times d}(\Rset)`
    Covariance matrix.

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> distribution = ot.Student(3.0, -0.5, 2.0)

Draw a sample:

>>> sample = distribution.getSample(5)

Create a multivariate Student distribution with constant :math:`\sigma` and identity correlation matrix:

>>> dimension = 5
>>> nu = 3.0
>>> mu = [0.0] * dimension
>>> sigma = [0.1] * dimension
>>> R = ot.CorrelationMatrix(dimension)
>>> distribution = ot.Student(nu, mu, sigma, R)
>>> print('distribution=', distribution)
distribution= Student(nu = 3, mu = [0,0,0,0,0], sigma = [0.1,0.1,0.1,0.1,0.1], R = 5x5
[[ 1 0 0 0 0 ]
 [ 0 1 0 0 0 ]
 [ 0 0 1 0 0 ]
 [ 0 0 0 1 0 ]
 [ 0 0 0 0 1 ]])

Create a multivariate Student distribution with a different :math:`\sigma` parameter and a different correlation matrix:

>>> sigma = [1.5, 0.4, 1.0, 2.5, 1.0]
>>> R = ot.CorrelationMatrix(dimension)
>>> R[0, 2] = -0.5
>>> R[3, 4] = -0.2
>>> distribution = ot.Student(nu, mu, sigma, R)
>>> print('distribution=', distribution)
distribution= Student(nu = 3, mu = [0,0,0,0,0], sigma = [1.5,0.4,1,2.5,1], R = 5x5
[[  1    0   -0.5  0    0   ]
 [  0    1    0    0    0   ]
 [ -0.5  0    1    0    0   ]
 [  0    0    0    1   -0.2 ]
 [  0    0    0   -0.2  1   ]])
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Student::computeCDF
R"RAW(Compute the cumulative distribution function.

See the class notes for the algorithm selection strategy.

Parameters
----------
x : sequence of float
    Point in :math:`\Rset^d`.

Returns
-------
F : float
    CDF value at :math:`x`.

Notes
-----
The algorithm depends on the dimension:

- dim 1: direct evaluation via the regularized incomplete beta function;
- dim 2: specialized 2D routine (:meth:`~openturns.DistFunc.pStudent2D`);
- dim 3: specialized 3D routine (:meth:`~openturns.DistFunc.pStudent3D`);
- dim >= 4: quasi-Monte Carlo Genz algorithm with *Genz-DefaultSampleSize*
  Sobol' realizations, conditioning on the Chi-square factor of the
  Student representation.

)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Student::computeProbability
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
- dim 2: specialized 2D routine (:meth:`~openturns.DistFunc.pStudent2D`);
- dim 3: specialized 3D routine (:meth:`~openturns.DistFunc.pStudent3D`);
- dim >= 4: quasi-Monte Carlo Genz algorithm with *Genz-DefaultSampleSize*
  Sobol' realizations, conditioning on the Chi-square factor of the
  Student representation.

)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Student::getNu
R"RAW(Accessor to the generalized number of degrees of freedom.

Returns
-------
nu : float
    The :math:`\nu` parameter.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Student::setNu
R"RAW(Accessor to the generalized number of degrees of freedom.

Parameters
----------
nu : float, :math:`\nu > 0`
    The :math:`\nu` parameter.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Student::getMu
R"RAW(Accessor to the :math:`\mu` parameter.

Returns
-------
mu : float,
    The :math:`\mu` parameter.)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::Student::setMu
R"RAW(Accessor to the :math:`\mu` parameter.

Parameters
----------
mu : float,
    The :math:`\mu` parameter.)RAW"


