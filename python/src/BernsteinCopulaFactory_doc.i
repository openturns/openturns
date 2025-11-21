%feature("docstring") OT::BernsteinCopulaFactory
"EmpiricalBernsteinCopula factory.

Notes
-----
This class builds an :class:`~openturns.EmpiricalBernsteinCopula` which is a non parametric fitting of
the copula of a multivariate distribution.

Using the class :class:`~openturns.BernsteinCopulaFactory` garantees to get a
:class:`~openturns.EmpiricalBernsteinCopula` which is a copula and not only a core,
i.e a multivariate distribution whose range is included in :math:`[0,1]^d` (see the documentation of the class to get more details).

The keys of :class:`~openturns.ResourceMap` related to the class are:

- the keys `BernsteinCopulaFactory-MinM` and `BernsteinCopulaFactory-MaxM` that define the range of :math:`m`
  in the optimization
  problems computing the optimal bin number according to a specified criterion,
- the key `BernsteinCopulaFactory-BinNumberSelection` that defines the  criterion to compute the optimal bin number
  when it is not specified. The possible choices are 'AMISE', 'LogLikelihood', 'PenalizedCsiszarDivergence';
- the key `BernsteinCopulaFactory-KFraction` that defines the fraction of the sample used for the validation in the
  method :meth:`ComputeLogLikelihoodBinNumber`,
- the key `BernsteinCopulaFactory-SamplingSize`  that defines the :math:`N` parameter used in the
  method :meth:`ComputePenalizedCsiszarDivergenceBinNumber`.

See also
--------
DistributionFactory, EmpiricalBernsteinCopula"

// ---------------------------------------------------------------------

%feature("docstring") OT::BernsteinCopulaFactory::build
R"RAW(Build the empirical Bernstein copula.

**Available usages**:

    build()

    build(*sample*)

    build(*sample, m*)

    build(*sample, method, f*)

Parameters
----------
sample : 2-d sequence of float, of dimension :math:`d`
    The sample of size :math:`\sampleSize>0` from which the copula is estimated.
method : str
    The name of the bin number selection method. Possible choices are *AMISE*, *LogLikelihood* and *PenalizedCsiszarDivergence*.
    
    Default is *LogLikelihood*.
f : :class:`~openturns.Function`
    The function defining the Csiszar divergence of interest used by
    the  *PenalizedCsiszarDivergence* method.
    
    Default is Function().
m : int,:math:`1 \leq m \leq \sampleSize`,
    The bin number, i.e. the number of sub-intervals in which all the edges of the unit cube
    :math:`[0, 1]^d` are regularly partitioned.
    
    Default value is the value computed from the default bin number selection method.

Returns
-------
copula : :class:`~openturns.Distribution`
    The empirical Bernstein copula as a generic distribution.

Notes
-----
If the bin number :math:`m` is specified and does not divide the sample size :math:`\sampleSize`, then a part of the sample is
removed for the result to be a copula. See :class:`~openturns.EmpiricalBernsteinCopula`.
)RAW"
// ---------------------------------------------------------------------

%feature("docstring") OT::BernsteinCopulaFactory::buildAsEmpiricalBernsteinCopula
R"RAW(Build the empirical Bernstein copula as a native distribution.

**Available usages**:

    buildAsEmpiricalBernsteinCopula()

    buildAsEmpiricalBernsteinCopula(*sample*)

    buildAsEmpiricalBernsteinCopula(*sample, m*)

    buildAsEmpiricalBernsteinCopula(*sample, method, f*)

Parameters
----------
sample : 2-d sequence of float, of dimension *d*
    The sample of size :math:`\sampleSize>0` from which the copula is estimated.
method : str
    The name of the bin number selection method. Possible choices are *AMISE*, *LogLikelihood* and
    *PenalizedCsiszarDivergence*.
    
    Default is *LogLikelihood*.
f : :class:`~openturns.Function`
    The function defining the Csiszar divergence of interest used by
    the  *PenalizedCsiszarDivergence* method.
    
    Default is Function().
m : int, :math:`1 \leq m \leq \sampleSize`,
    The bin number, i.e. the number of sub-intervals in which all the edges of the unit cube
    :math:`[0, 1]^d` are regularly partitioned.
    
    Default value is the value computed from the default bin number selection method.

Returns
-------
copula : :class:`~openturns.EmpiricalBernsteinCopula`
    The empirical Bernstein copula as a native distribution.

Notes
-----
If the bin number :math:`m` is specified and does not divide the sample size :math:`\sampleSize`, then a part of the sample is
removed for the result to be a copula a copula. See :class:`~openturns.EmpiricalBernsteinCopula`.
)RAW"
// ---------------------------------------------------------------------

%feature("docstring") OT::BernsteinCopulaFactory::ComputeAMISEBinNumber
R"RAW(Compute the optimal AMISE number of bins.

Parameters
----------
sample : 2-d sequence of float, of dimension 1
    The sample from which the optimal AMISE bin number is computed.

Notes
-----
The bin number :math:`m` is computed by minimizing the asymptotic mean integrated squared error (AMISE),
leading to:

.. math::

    m = 1+\left\lfloor \sampleSize^{\frac{2}{4+d}} \right\rfloor

where :math:`\lfloor x \rfloor` is the largest integer less than or equal to :math:`x`, :math:`\sampleSize` the sample size and :math:`d` the sample dimension.

Note that this optimal :math:`m` does not necessarily divide the sample size :math:`\sampleSize`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BernsteinCopulaFactory::ComputeLogLikelihoodBinNumber
R"RAW(Compute the optimal log-likelihood number of bins by cross-validation.

Parameters
----------
sample : 2-d sequence of float, of dimension 1
    The sample of size :math:`\sampleSize` from which the optimal log-likelihood bin number is computed.
kFraction : int, :math:`0<kFraction<\sampleSize`
    The fraction of the sample used for the validation.
    
    Default value 2.

Notes
-----
Let :math:`\cE= (\inputReal_1, \dots, \inputReal_\sampleSize)` be the given sample. If :math:`kFraction=1`, the bin number :math:`m` is given by:

.. math::

    m = \argmin_{M\in\{1,\dots,\sampleSize\}}\dfrac{1}{\sampleSize}\sum_{\vect{x}_i\in\cE}-\log c^{\cE}_{M}(\vect{x}_i)

where :math:`c_M^{\cE}` is the density function of the :class:`~openturns.EmpiricalBernsteinCopula` associated to the sample :math:`\cE` and the bin number :math:`M`.

If :math:`kFraction>1`, the bin number :math:`m` is given by:

.. math::

    m = \argmin_{M\in\{1,\dots,\sampleSize\}}\dfrac{1}{kFraction}\sum_{k=0}^{kFraction-1}\dfrac{1}{\sampleSize}\sum_{\vect{x}_i\in\cE^V_k}-\log c^{\cE^L_k}_{M}(\vect{x}_i)

where :math:`\cE^V_k=\left\{\vect{x}_i\in\cE\,|\,i\equiv k \mod kFraction\right\}`
and :math:`\cE^L_k=\cE \backslash \cE^V_k`.

Note that this optimal :math:`m` does not necessarily divide the sample size :math:`\sampleSize`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BernsteinCopulaFactory::ComputePenalizedCsiszarDivergenceBinNumber
R"RAW(Compute the optimal penalized Csiszar divergence number of bins.

Parameters
----------
sample : 2-d sequence of float, of dimension 1
    The sample of size :math:`\sampleSize` from which the optimal AMISE bin number is computed.
f : :class:`~openturns.Function`
    The function defining the Csiszar divergence of interest.
alpha : float, :math:`\alpha\geq 0`
    The penalization factor.

Notes
-----
Let :math:`\cE=(\inputReal_1, \dots, \inputReal_\sampleSize)` be the given sample. The bin number :math:`m` is given by:

.. math::

    m = \argmin_{M\in\{1,\dots,\sampleSize\}}\left[\hat{D}_f(c^{\cE}_{M})-\dfrac{1}{\sampleSize}\sum_{\vect{x}_i\in\cE}f\left(\dfrac{1}{c^{\cE}_{M}(\vect{x}_i)}\right)\right]^2-[\rho_S(c^{\cE}_{M})-\rho_S({\cE}_{M})]^2

where :math:`c_M^{\cE}` is the density function of the :class:`~openturns.EmpiricalBernsteinCopula` associated to the sample
:math:`\cE` and the bin number :math:`M`, :math:`\hat{D}_f(c^{\cE}_{M})=\dfrac{1}{N}\sum_{j=1}^Nf\left(\dfrac{1}{\vect{u}_j}\right)` a
Monte Carlo estimate of the Csiszar :math:`f` divergence, :math:`\rho_S(c^{\cE}_{M})` the exact Spearman correlation of the empirical
Bernstein copula :math:`c^{\cE}_{M}` and :math:`\rho_S({\cE}_{M})` the empirical Spearman correlation of the sample :math:`{\cE}_{M}`.

The parameter :math:`N` is controlled by the *BernsteinCopulaFactory-SamplingSize* key in :class:`~openturns.ResourceMap`.

Note that this optimal :math:`m` does not necessarily divide the sample size :math:`\sampleSize`.
)RAW"

