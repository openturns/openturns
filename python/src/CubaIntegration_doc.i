%feature("docstring") OT::CubaIntegration
R"RAW(Multidimensional integration algorithms.

Parameters
----------
algorithmName : str
    The name of the integration routine to use. It must be 'cuhre', 'divonne',
    'suave', or 'vegas'.

Notes
-----
The Cuba algorithms enable one to approximate the definite integral:

.. math::

    \int_{I_n} f(x_1, ..., x_n) \di{x_1}...\di{x_n}


which takes values in :math:`\Rset^p`, with :math:`f: \Rset^n \mapsto \Rset^p`,
:math:`I_n=[a_1, b_1]\times\cdots\times[a_n, b_n]\subset\Rset^n`,
having :math:`a_i<b_i` for all :math:`i=1,...,n`.

The four available integration routines work on vector integrands. While Cuhre
is deterministic, the three other ones use Monte Carlo methods. Cuhre employs a
cubature rule for subregion estimation in an adaptative subdivision scheme.
Divonne works by stratified sampling. Suave combines techniques from importance
sampling and subregion sampling. Vegas uses importance sampling for variance
reduction. The result is returned as a :class:`~openturns.Point` of dimension
:math:`p`, which is the number of components of the return value of the
:class:`~openturns.Function` that one will integrate.

All the integration routines and the parameters are discussed in [hahn2005]_.

This class uses the following entries of :class:`~openturns.ResourceMap`:

- *CubaIntegration-DefaultMaximumAbsoluteError* (:raw:`Scalar`, default: `0.0001`): default absolute error.
- *CubaIntegration-DefaultMaximumRelativeError* (:raw:`Scalar`, default: `0.0001`): default relative error.
- *CubaIntegration-DefaultMaximumCallsNumber* (:raw:`UnsignedInteger`, default: `1000000`): default maximum number of integrand calls.
- *CubaIntegration-mineval* (:raw:`UnsignedInteger`, default: `0`): minimum number of integrand evaluations.
- *CubaIntegration-seed* (:raw:`UnsignedInteger`, default: `0`): random number generator seed.
- *CubaIntegration-cuhre-key* (:raw:`UnsignedInteger`, default: `0`): Cuhre integration key.
- *CubaIntegration-flags* (:raw:`UnsignedInteger`, default: `0`): global flags.
- *CubaIntegration-divonne-border* (:raw:`Scalar`, default: `0.0`): Divonne border parameter.
- *CubaIntegration-divonne-maxchisq* (:raw:`Scalar`, default: `10.0`): Divonne maximum chi-square.
- *CubaIntegration-divonne-mindeviation* (:raw:`Scalar`, default: `0.25`): Divonne minimum deviation.
- *CubaIntegration-divonne-key1* (:raw:`UnsignedInteger`, default: `47`): Divonne key1 parameter.
- *CubaIntegration-divonne-key2* (:raw:`UnsignedInteger`, default: `1`): Divonne key2 parameter.
- *CubaIntegration-divonne-key3* (:raw:`UnsignedInteger`, default: `1`): Divonne key3 parameter.
- *CubaIntegration-divonne-maxpass* (:raw:`UnsignedInteger`, default: `5`): Divonne maximum passes.
- *CubaIntegration-divonne-ngiven* (:raw:`UnsignedInteger`, default: `0`): Divonne number of given points.
- *CubaIntegration-divonne-nextra* (:raw:`UnsignedInteger`, default: `0`): Divonne number of extra points.
- *CubaIntegration-suave-flatness* (:raw:`Scalar`, default: `25.0`): Suave flatness parameter.
- *CubaIntegration-suave-nnew* (:raw:`UnsignedInteger`, default: `1000`): Suave number of new integrand evaluations.
- *CubaIntegration-suave-nmin* (:raw:`UnsignedInteger`, default: `2`): Suave minimum number of evaluations.
- *CubaIntegration-vegas-nstart* (:raw:`UnsignedInteger`, default: `1000`): Vegas initial number of integrand evaluations.
- *CubaIntegration-vegas-nincrease* (:raw:`UnsignedInteger`, default: `500`): Vegas increase in evaluations per iteration.
- *CubaIntegration-vegas-nbatch* (:raw:`UnsignedInteger`, default: `1000`): Vegas batch size.
- *CubaIntegration-vegas-gridno* (:raw:`UnsignedInteger`, default: `0`): Vegas grid number.

Examples
--------
Create a Cuba algorithm:

>>> import openturns as ot
>>> algo = ot.CubaIntegration('vegas')
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CubaIntegration::integrate
R"RAW(Evaluation of the integral of :math:`f w` on an interval.

Parameters
----------
f : :class:`~openturns.Function`, :math:`f: \Rset^n \mapsto \Rset^p`
    The integrand function.
interval : :class:`~openturns.Interval`, :math:`interval \subset \Rset^n` 
    The integration domain.

Returns
-------
value : :class:`~openturns.Point`
    Approximation of the integral. Its dimension is :math:`p`.

Examples
--------
>>> import openturns as ot
>>> f = ot.SymbolicFunction(['x', 'y', 'z'], ['sin(x) * cos(y) * exp(z)'])
>>> interval = ot.Interval([0.0] * 3, [1.0] * 3)
>>> algoC = ot.CubaIntegration('vegas')
>>> value = algoC.integrate(f, interval)  # doctest: +SKIP
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CubaIntegration::getMaximumRelativeError
"Accessor to the relative accuracy required in the integration.

Returns
-------
maximumRelativeError : float
    The relative accuracy that the *integrate* method attempts to achieve."

// ---------------------------------------------------------------------

%feature("docstring") OT::CubaIntegration::setMaximumRelativeError
"Accessor to the relative accuracy required in the integration.

Parameters
----------
maximumRelativeError : float
    The relative accuracy that the *integrate* method attempts to achieve."

// ---------------------------------------------------------------------

%feature("docstring") OT::CubaIntegration::getMaximumAbsoluteError
"Accessor to the absolute accuracy required in the integration.

Returns
-------
maximumAbsoluteError : float
    The absolute accuracy that the *integrate* method attempts to achieve."

// ---------------------------------------------------------------------

%feature("docstring") OT::CubaIntegration::setMaximumAbsoluteError
"Accessor to the absolute accuracy required in the integration.

Parameters
----------
maximumAbsoluteError : float
    The absolute accuracy that the *integrate* method attempts to achieve."

// ---------------------------------------------------------------------

%feature("docstring") OT::CubaIntegration::getAlgorithmName
"Accessor to the name of the integration routine to be used.

Returns
-------
algorithmName : str
    The name of the integration routine used by the algorithm."

// ---------------------------------------------------------------------

%feature("docstring") OT::CubaIntegration::GetAlgorithmNames
"Retrieves the names of the available integration algorithms.

Returns
-------
algoName : :class:`~openturns.Description`
    The names of the available integration algorithms."

// ---------------------------------------------------------------------

%feature("docstring") OT::CubaIntegration::setAlgorithmName
"Accessor to the name of the integration routine to be used.

Parameters
----------
algorithmName : str
    The name of the integration routine used by the algorithm. It must be 'cuhre', 'divonne', 'suave', or 'vegas'."

// ---------------------------------------------------------------------

%feature("docstring") OT::CubaIntegration::getMaximumCallsNumber
"Accessor to the maximal number of integrand calls.

Returns
-------
N : int
    The maximal number of times the algorithm will attempt to call the integrand."

// ---------------------------------------------------------------------

%feature("docstring") OT::CubaIntegration::setMaximumCallsNumber
"Accessor to the maximal number of integrand calls.

Parameters
----------
N : int, :math:`N>0`
    The maximal number of times the algorithm will attempt to call the integrand."

