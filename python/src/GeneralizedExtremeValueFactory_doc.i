%feature("docstring") OT::GeneralizedExtremeValueFactory
"GeneralizedExtremeValue factory.

See also
--------
DistributionFactory, GeneralizedExtremeValue, FrechetFactory, GumbelFactory, WeibullMaxFactory

Notes
-----
Several estimators to build a GeneralizedExtremeValueFactory distribution from a scalar sample
are proposed. The details are given in the methods documentation.

The following :class:`~openturns.ResourceMap` entries can be used to tweak
the parameters of the optimization solver involved in the different estimators:

- `GeneralizedExtremeValueFactory-DefaultOptimizationAlgorithm`
- `GeneralizedExtremeValueFactory-MaximumCallsNumber`
- `GeneralizedExtremeValueFactory-MaximumAbsoluteError`
- `GeneralizedExtremeValueFactory-MaximumRelativeError`
- `GeneralizedExtremeValueFactory-MaximumObjectiveError`
- `GeneralizedExtremeValueFactory-MaximumConstraintError`
- `GeneralizedExtremeValueFactory-InitializationMethod`
- `GeneralizedExtremeValueFactory-NormalizationMethod`
"

// ---------------------------------------------------------------------

%feature("docstring") OT::GeneralizedExtremeValueFactory::build
R"RAW(Estimate the distribution via maximum likelihood.


**Available usages**:

    build(*sample*)

    build(*param*)

Parameters
----------
sample : 2-d sequence of float
    The block maxima sample of dimension 1 from which
    :math:`\vect{\theta} = (\mu, \sigma, \xi)` are estimated.
param : sequence of float
   The parameters of the :class:`~openturns.GeneralizedExtremeValue`.

Returns
-------
distribution : :class:`~openturns.GeneralizedExtremeValue`
    The estimated distribution.

Notes
-----
The estimation strategy described in :meth:`buildAsGeneralizedExtremeValue` is followed.)RAW"
// ----------------------------------------------------------------------------

%feature("docstring") OT::GeneralizedExtremeValueFactory::buildAsGeneralizedExtremeValue
R"RAW(Estimate the distribution as native distribution.


**Available usages**:

    buildAsGeneralizedExtremeValue()

    buildAsGeneralizedExtremeValue(*sample*)

    buildAsGeneralizedExtremeValue(*param*)

Parameters
----------
sample : 2-d sequence of float
    The block maxima sample of dimension 1 from which
    :math:`\vect{\theta} = (\mu, \sigma, \xi)` are estimated.
param : sequence of float
   The parameters of the :class:`~openturns.GeneralizedExtremeValue`.

Returns
-------
distribution : :class:`~openturns.GeneralizedExtremeValue`
    The estimated distribution as a GeneralizedExtremeValue.
    
    In the first usage, the default GeneralizedExtremeValue distribution is built.

Notes
-----
The estimate maximizes the log-likelihood of the model.)RAW"

// ----------------------------------------------------------------------------

%feature("docstring") OT::GeneralizedExtremeValueFactory::buildMethodOfXiProfileLikelihood
R"RAW(Estimate the distribution with the profile likelihood.

Parameters
----------
sample : 2-d sequence of float
    Block maxima grouped in a sample of size :math:`n` and dimension :math:`R`.
r : int, :math:`1 \leq r \leq R`,
    Number of largest order statistics taken into account among the :math:`R` stored ones.

    By default, :math:`r=0` which means that all the maxima are used.

Returns
-------
distribution : :class:`~openturns.GeneralizedExtremeValue`
    The estimated distribution.

Notes
-----
The method estimates a GEV distribution parameterized by :math:`\vect{\theta} = (\mu, \sigma, \xi)` from a given sample.

The estimator :math:`\hat{\vect{\theta}}` is defined using a nested numerical optimization of the log-likelihood:

.. math::

    \ell_p (\xi) = \max_{(\mu, \sigma)} \ell (\mu, \sigma, \xi)

where :math:`\ell (\mu, \sigma, \xi)` is detailed in equations :eq:`llR1` and :eq:`llR2` with :math:`r=1`.

If :math:`\xi = 0` then:

.. math::

    \hat{\xi}  & =  \argmax_{\xi} \ell_p(\xi)\\
    (\hat{\mu}, \hat{\sigma})  & =  \argmax_{(\mu, \sigma)} \ell(\mu, \sigma, \hat{\xi})

The starting point of the optimization is initialized from the probability weighted moments method, see [diebolt2008]_.
)RAW"

// ----------------------------------------------------------------------------

%feature("docstring") OT::GeneralizedExtremeValueFactory::buildMethodOfXiProfileLikelihoodEstimator
R"RAW(Estimate the distribution and the parameter distribution with the profile likelihood.

Parameters
----------
sample : 2-d sequence of float
    Block maxima grouped in a sample of size :math:`n` and dimension :math:`R`.
r : int, :math:`1 \leq r \leq R`,
    Number of largest order statistics taken into account among the :math:`R` stored ones.
    The block maxima sample of dimension 1 from which
    :math:`\vect{\theta} = (\mu, \sigma, \xi)` are estimated.

    By default, :math:`r=0` which means that all the maxima are used.

Returns
-------
result : :class:`~openturns.ProfileLikelihoodResult`
    The result class.

Notes
-----
The method estimates a GEV distribution parameterized by :math:`\vect{\theta} = (\mu, \sigma, \xi)` from a given sample.

The estimator :math:`\hat{\vect{\theta}}` is defined in :meth:`buildMethodOfXiProfileLikelihood`.

The result class produced by the method provides:

- the GEV distribution associated to :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`,
- the asymptotic distribution of :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`,
- the profile log-likelihood function :math:`\xi \mapsto \ell_p(\xi)`,
- the optimal profile log-likelihood value :math:`\ell_p(\hat{\xi})`,
- confidence intervals of level :math:`(1-\alpha)` of :math:`\xi`.)RAW"

// ----------------------------------------------------------------------------

%feature("docstring") OT::GeneralizedExtremeValueFactory::buildMethodOfLikelihoodMaximization
R"RAW(Estimate the distribution from the :math:`r` largest order statistics.

Parameters
----------
sample : 2-d sequence of float
    Block maxima grouped in a sample of size :math:`n` and dimension :math:`R`.
r : int, :math:`1 \leq r \leq R`,
    Number of largest order statistics taken into account among the :math:`R` stored ones.

    By default, :math:`r=0` which means that all the maxima are used.

Returns
-------
distribution : :class:`~openturns.GeneralizedExtremeValue`
    The estimated distribution.

Notes
-----
The method estimates a GEV distribution parameterized by :math:`\vect{\theta} = (\mu, \sigma, \xi)` from a given sample.

Let us suppose we have a series of independent and identically distributed variables and that data are grouped
into :math:`n` blocks. In each block, the largest :math:`R` observations are recorded.

We define the series
:math:`M_i^{(R)} = (z_i^{(1)}, \hdots, z_i^{(R)})` for :math:`1 \leq i \leq n` where the values are sorted
in decreasing order.

The estimator of :math:`(\mu, \sigma, \xi)` maximizes  the log-likelihood built from the :math:`r` largest order statistics,
with :math:`1 \leq r \leq R` defined as:

If :math:`\xi \neq 0`, then:

.. math::
    :label: llR1

    \ell(\mu, \sigma, \xi) = -nr \log \sigma - \sum_{i=1}^n   \biggl[ 1 + \xi \Bigl( \frac{z_i^{(r)} - \mu }{\sigma} \Bigr) \biggr]^{-1/\xi}  -\left(\dfrac{1}{\xi} +1 \right)  \sum_{i=1}^n \sum_{k=1}^r  \log \biggl[ 1 + \xi \Bigl( \frac{z_i^{(k)} - \mu }{\sigma} \Bigr) \biggr] 


defined on :math:`(\mu, \sigma, \xi)` such that :math:`1+\xi \left( \frac{z_i^{(k)} - \mu}{\sigma} \right) > 0`
for all :math:`1 \leq i \leq m` and :math:`1 \leq k \leq r`.

If :math:`\xi = 0`, then:

.. math::
    :label: llR2

    \ell(\mu, \sigma, \xi) = -nr \log \sigma - \sum_{i=1}^n   \exp \biggl[ - \Bigl( \frac{z_i^{(r)} - \mu }{\sigma} \Bigr) \biggr] - \sum_{i=1}^n \sum_{k=1}^r  \Bigl( \frac{z_i^{(k)} - \mu }{\sigma} \Bigr)
)RAW"

// ----------------------------------------------------------------------------

%feature("docstring") OT::GeneralizedExtremeValueFactory::buildMethodOfLikelihoodMaximizationEstimator
R"RAW(Estimate the distribution and the parameter distribution with the R-maxima method.

Parameters
----------
sampleM : 2-d sequence of float
    Block maxima  grouped in a sample of size :math:`n` and dimension :math:`R`.
r : int, :math:`1 \leq r \leq R`, optional
    Number of order statistics taken into account among the :math:`R` stored ones.
    
    By default, :math:`r=0` which means that all the maxima are used.

Returns
-------
result : :class:`~openturns.DistributionFactoryLikelihoodResult`
    The result class.

Notes
-----
The method estimates a GEV distribution parameterized by :math:`\vect{\theta} = (\mu, \sigma, \xi)` from a given sample.

The estimator :math:`\hat{\vect{\theta}}` is defined using the profile log-likelihood  as detailed in
:meth:`buildMethodOfLikelihoodMaximization`.

The result class produced by the method provides:

- the GEV distribution associated to :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`,
- the asymptotic distribution of :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`.)RAW"

// ----------------------------------------------------------------------------

%feature("docstring") OT::GeneralizedExtremeValueFactory::buildCovariates
R"RAW(Estimate a GEV from covariates.

Parameters
----------
sample : 2-d sequence of float
    The block maxima grouped in a sample of size :math:`m` and one dimension.
covariates : 2-d sequence of float
    Covariates sample.
    A constant column is automatically added if it is not provided.
muIndices : sequence of int, optional
    Indices of covariates considered for parameter :math:`\mu`.

    By default, an empty sequence.
    
    The index of the constant covariate is added
    if empty or if the covariates do not initially contain a constant column.
sigmaIndices : sequence of int, optional
    Indices of covariates considered for parameter :math:`\sigma`.

    By default, an empty sequence.
    
    The index of the constant covariate is added
    if empty or if the covariates do not initially contain a constant column.
xiIndices : sequence of int, optional
    Indices of covariates considered for parameter :math:`\xi`.

    By default, an empty sequence.
    
    The index of the constant covariate is added
    if empty or if the covariates do not initially contain a constant column.
muLink : :class:`~openturns.Function`, optional
    The :math:`h_{\mu}` function.

    By default, the identity function.
sigmaLink : :class:`~openturns.Function`, optional
    The :math:`h_{\sigma}` function.

    By default, the identity function.
xiLink : :class:`~openturns.Function`, optional
    The :math:`h_{\xi}` function.

    By default, the identity function.
initializationMethod : str, optional
    The initialization method for the optimization problem: *Gumbel* or *Static*.

    By default, the method *Gumbel* (see :class:`~openturns.ResourceMap`, key
    *GeneralizedExtremeValueFactory-InitializationMethod*).
normalizationMethod : str, optional
    The data normalization method: *CenterReduce*, *MinMax* or *None*.

    By default, the method *MinMax* (see :class:`~openturns.ResourceMap`, key
    *GeneralizedExtremeValueFactory-NormalizationMethod*).

Returns
-------
result : :class:`~openturns.CovariatesResult`
    The result class.

Notes
-----
Let :math:`Z_{\vect{y}}` be a GEV model whose parameters depend on :math:`d` covariates
denoted by :math:`\vect{y} = \Tr{(y_1, \dots, y_d)}`:

.. math::

    Z_{\vect{y}} \sim \mbox{GEV}(\mu(\vect{y}), \sigma(\vect{y}), \xi(\vect{y}))

We denote by :math:`(z_{\vect{y}_1}, \dots, z_{\vect{y}_n})`
the values of :math:`Z_{\vect{y}}` associated to the values of the
covariates :math:`(\vect{y}_1, \dots, \vect{y}_n)`.

For numerical reasons, it is recommended to normalize the covariates. Each covariate
:math:`y_k` has its own normalization:

.. math::

    \tilde{y}_k = \tau_k(y_k) = \dfrac{y_k-c_k}{d_k}

and with three ways of defining :math:`(c_k,d_k)` of the covariate :math:`y_k`:

- the *CenterReduce* method where :math:`c_k = \dfrac{1}{n} \sum_{i=1}^n y_{k,i}` is the
  covariate mean and :math:`d_k = \sqrt{\dfrac{1}{n} \sum_{i=1}^n (y_{k,i}-c_k)^2}` is the standard deviation of the covariates;
- the *MinMax* method where :math:`c_k = \min_i y_{k,i}` is the min value of the covariate
  :math:`y_k` and :math:`d_k = \max_i y_{k,i}- \min_i y_{k,i}` its range. This is the default method;
- the *None* method where :math:`c_k = 0` and :math:`d_k = 1`: in that case, data are not
  normalized.

Let :math:`\vect{\theta} = (\mu, \sigma, \xi)` be the vector of parameters. Then, :math:`\vect{\theta}` depends on all the :math:`d` covariates
even if each component of :math:`\vect{\theta}` only depends on a subset of the covariates. We
denote by :math:`(y_1^q, \dots, y_{d_q}^q)` the :math:`d_q` covariates involved in the
modelling of the component :math:`\theta_q`.

Each component :math:`\theta_q` can be written as a function of the normalized covariates:

.. math::

    \theta_q(y_1^q, \dots, y_{d_q}^q)  & = h_q\left(\sum_{i=1}^{d_q} \tilde{\beta}
    _i^q\tilde{y}_i^q \right)

This relation can be written  as a function of the real covariates:

.. math::

    \theta_q(y_1^q, \dots, y_{d_q}^q)  & = h_q\left(\sum_{i=1}^{d_q} \beta_i^qy_i^q +
    \beta_{d_q+1}^q \right)

where:

- :math:`h_q: \Rset \mapsto \Rset` is usually referred to as the *inverse-link function* of
  the component :math:`\theta_q`,
- each :math:`\beta_i^{q} \in \Rset`.

To allow some parameters to remain constant, i.e. independent of the covariates
(this will generally be the case for the parameter :math:`\xi`), the library systematically
adds the constant covariate to the speciﬁed covariates.

The complete vector of parameters is defined by:

.. math::

    \Tr{\vect{b}} & = \Tr{ ( \Tr{\vect{b}_1}, \dots,  \Tr{\vect{b}_p} ) } \in  \Rset^{d_t}\\
    \Tr{\vect{b}_q} & =  (\beta_1^q, \dots, \beta_{d_q}^q) \in \Rset^{d_q}

where :math:`d_t = \sum_{q=1}^p d_q`.

The estimator of  :math:`\vect{\beta}` maximizes  the likelihood of the model which is defined
by:

.. math::

    L(\vect{\beta}) = \prod_{i=1}^{n} g(z_{\vect{y}_i};\vect{\theta}(\vect{y}_i)))

where :math:`g(z_{\vect{y}_i};\vect{\theta}(\vect{y}_i))` denotes the GEV density function
with parameters
:math:`\vect{\theta}(\vect{y}_i)` and evaluated at :math:`z_{\vect{y}_i}`.

Then, if none of the :math:`\xi(\vect{y}_i)` is zero, the log-likelihood is defined by:

.. math::

    \ell (\vect{\beta}) = -\sum_{i=1}^{n} \left\{ \log(\sigma(\vect{y}_i)) + (1 + 1 /
    \xi(\vect{y}
    _i) ) \log\left[ 1+\xi(\vect{y}_i) \left( \frac{z_{\vect{y}_i} - \mu(\vect{y}_i)}
    {\sigma(\vect{y}_i)}\right) \right] + \left[ 1 + \xi(\vect{y}_i) \left( \frac{
    z_{\vect{y}_i}-
    \mu(\vect{y}_i)}{\sigma(\vect{y}_i)} \right) \right]^{-1 / \xi(\vect{y}_i)} \right\}

defined on :math:`(\mu, \sigma, \xi)` such that :math:`1+\xi \left( \frac{z_{\vect{y}_i} -
\mu(\vect{y}_i)}{\sigma(\vect{y}_i)} \right) > 0`
for all :math:`\vect{y}_i`.

And if any of the :math:`\xi(\vect{y}_i)` is equal to 0, the log-likelihood is defined as:

.. math::

    \ell (\vect{\beta}) = -\sum_{i=1}^{n} \left\{ \log(\sigma(\vect{y}_i)) + \frac{z_{\vect{y}_i}
    - \mu(\vect{y}_i)}{\sigma(\vect{y}_i)} + \exp \left\{ - \frac{z_{\vect{y}_i} -
    \mu(\vect{y}_i)}{\sigma(\vect{y}_i)} \right\} \right\}

The initialization of the optimization problem is crucial.
Two initial points :math:`(\mu_0, \sigma_0, \xi_0)` are proposed:

- the *Gumbel* initial point: in that case, we assume that the GEV is a stationary Gumbel
  distribution and we deduce
  :math:`(\mu_0, \sigma_0)` from the mean :math:`\hat{M}` and standard variation :math:`\hat{\sigma}` of the data:
  :math:`\sigma_0 = \dfrac{\sqrt{6}}{\pi} \hat{\sigma}` and :math:`\mu_0 = \hat{M} - \gamma
  \sigma_0` where
  :math:`\gamma` is Euler's constant; then we take the initial point :math:`(\mu_0, \sigma_0, \xi_0 = 0.1)`. This is the default initial point;
- the *Static* initial point: in that case, we assume that the GEV is stationary and
  :math:`(\mu_0, \sigma_0, \xi_0)`
  is the maximum likelihood estimate resulting from that assumption.

The result class provides:

- the estimator :math:`\hat{\vect{\beta}}`,
- the asymptotic distribution of :math:`\hat{\vect{\beta}}`,
- the parameter function :math:`(\vect{\beta}, \vect{y}) \mapsto \vect{\theta}(\vect{\beta},
  \vect{y})`,
- the graphs of the parameter functions :math:`y_k \mapsto \theta_q(\vect{y})`, where all the components of
  :math:`\vect{y}` are fixed to a reference value excepted for :math:`y_k`, for each :math:`k`,
- the graphs of the parameter functions :math:`(y_k, y_\ell) \mapsto\theta_q(\vect{y})`, where all the
  components of :math:`\vect{y}` are fixed to a reference value excepted for
  :math:`(y_k, y_\ell)`, for each :math:`(k,\ell)`,
- the normalizing function :math:`\vect{y} \mapsto (\tau_1(y_1), \dots, \tau_d(y_d))`,
- the optimal log-likelihood value :math:`\hat{\vect{\beta}}`,
- the GEV distribution at covariate :math:`\vect{y}`,
- the graphs of the quantile functions of order :math:`p`: :math:`y_k \mapsto q_p(Z_{\vect{y}})` where all the components
  of :math:`\vect{y}` are fixed to a reference value excepted for :math:`y_k`, for each :math:`k`,
- the graphs of the quantile functions of order :math:`p`: :math:`(y_k, y_\ell) \mapsto q_p(Z_{\vect{y}})`
  where all the components of :math:`\vect{y}` are fixed to a
  reference value excepted for :math:`(y_k, y_\ell)`, for each :math:`(k,\ell)`.)RAW"

// ----------------------------------------------------------------------------

%feature("docstring") OT::GeneralizedExtremeValueFactory::buildTimeVarying
R"RAW(Estimate a non stationary GEV from a time-dependent parametric model.

Parameters
----------
sample : 2-d sequence of float
    The block maxima grouped in a sample of size :math:`m` and one dimension.
timeStamps : 2-d sequence of float
    Values of :math:`t`.
basis : :class:`~openturns.Basis`
    Functional basis respectively for :math:`\mu(t)`, :math:`\sigma(t)` and :math:`\xi(t)`.
muIndices : sequence of int, optional
    Indices of basis terms considered for parameter :math:`\mu`
sigmaIndices : sequence of int, optional
    Indices of basis terms considered for parameter :math:`\sigma`
xiIndices : sequence of int, optional
    Indices of basis terms considered for parameter :math:`\xi`
muLink : :class:`~openturns.Function`, optional
    The :math:`h_{\mu}` function.

    By default, the identity function.
sigmaLink : :class:`~openturns.Function`, optional
    The :math:`h_{\sigma}` function.

    By default, the identity function.
xiLink : :class:`~openturns.Function`, optional
    The :math:`h_{\xi}` function.

    By default, the identity function.
initializationMethod : str, optional
    The initialization method for the optimization problem: *Gumbel* or *Static*.

    By default, the method *Gumbel* (see :class:`~openturns.ResourceMap`, key
    *GeneralizedExtremeValueFactory-InitializationMethod*).
normalizationMethod : str, optional
    The data normalization method: *CenterReduce*, *MinMax* or *None*.

    By default, the method *MinMax* (see :class:`~openturns.ResourceMap`, key
    *GeneralizedExtremeValueFactory-NormalizationMethod*).

Returns
-------
result : :class:`~openturns.TimeVaryingResult`
    The result class.

Notes
-----
Let :math:`Z_t` be a non stationary GEV distribution:

.. math::

    Z_t \sim \mbox{GEV}(\mu(t), \sigma(t), \xi(t))

We denote by :math:`(z_{t_1}, \dots, z_{t_n})` the values of
:math:`Z_t` on the time stamps :math:`(t_1, \dots, t_n)`.

For numerical reasons, it is recommended to normalize the time stamps.
The following mapping is applied:

.. math::

    \tau(t) = \dfrac{t-c}{d}

and with three ways of defining :math:`(c,d)`:

- the *CenterReduce* method where :math:`c = \dfrac{1}{n} \sum_{i=1}^n t_i` is the mean time
  stamps
  and :math:`d = \sqrt{\dfrac{1}{n} \sum_{i=1}^n (t_i-c)^2}` is the standard deviation of the time stamps;
- the *MinMax* method where :math:`c = t_1` is the first time and :math:`d = t_n-t_1` the
  range of the time stamps. This is the default method;
- the *None* method where :math:`c = 0` and :math:`d = 1`: in that case, data are not
  normalized.

If we denote by :math:`\theta_q` is a component of :math:`\vect{\theta} = (\mu, \sigma, \xi)`,
then  :math:`\theta_q` can be written as a function of :math:`t`: 

.. math::

    \theta_q(t)  = h_q\left(\sum_{i=1}^{d_{\theta_q}} \beta_i^{\theta_q} \varphi_i^{\theta_q}
    (\tau(t))\right)

where:

- :math:`d_{\theta_q}` is the size of the functional basis involved in the modelling of
  :math:`\theta_q`,
- :math:`h_q: \Rset \mapsto \Rset` is usually referred to as the *inverse-link function* of
  the parameter :math:`\theta_q`,
- each :math:`\varphi_i^{\theta_q}` is a scalar function :math:`\Rset \mapsto \Rset`,
- each :math:`\beta_i^{j} \in \Rset`.

We denote by :math:`d_{\mu}`, :math:`d_{\sigma}` and :math:`d_{\xi}` the size of the functional basis of
:math:`\mu`, :math:`\sigma` and :math:`\xi` respectively. We denote by
:math:`\vect{\beta} = (\beta_1^{\mu}, \dots, \beta_{d_{\mu}}^{\mu}, \beta_1^{\sigma}, \dots, \beta_{d_{\sigma}}^{\sigma}, \beta_1^{\xi}, \dots, \beta_{d_{\xi}}^{\xi})`
the complete vector of parameters.

The estimator of  :math:`\vect{\beta}` maximizes  the likelihood of the non stationary model which is defined by:

.. math::

    L(\vect{\beta}) = \prod_{i=1}^{n} g(z_{t_i};\mu(t_i), \sigma(t_i), \xi(t_i))

where :math:`g(z_{t};\mu(t), \sigma(t), \xi(t))` denotes the GEV density function with parameters
:math:`(\mu(t), \sigma(t), \xi(t))` evaluated at :math:`z_t`.

Then, if none of the :math:`\xi(t_i)` is zero, the log-likelihood is defined by:

.. math::

    \ell (\vect{\beta}) = -\sum_{i=1}^{n} \left\{ \log(\sigma(t_i)) + (1 + 1 / \xi(t_i) ) \log\left[ 1+\xi(t_i) \left( \frac{z_{t_i} - \mu(t_i)}{\sigma(t_i)}\right) \right] + \left[ 1 + \xi(t_i) \left( \frac{z_{t_i}- \mu(t_i)}{\sigma(t_i)} \right) \right]^{-1 / \xi(t_i)} \right\}

defined on :math:`(\mu, \sigma, \xi)` such that :math:`1+\xi(t) \left( \frac{z_t - \mu(t)}{\sigma(t)} \right) > 0`
for all :math:`t`.

And if any of the :math:`\xi(t)` is equal to 0, the log-likelihood is defined as:

.. math::

    \ell (\vect{\beta}) = -\sum_{t=1}^{n} \left\{ \log(\sigma(t)) + \frac{z_t - \mu(t)}{\sigma(t)} + \exp \left\{ - \frac{z_t - \mu(t)}{\sigma(t)} \right\} \right\}

The initialization of the optimization problem is crucial.
Two initial points :math:`(\mu_0, \sigma_0, \xi_0)` are proposed:

- the *Gumbel* initial point: in that case, we assume that the GEV is a stationary Gumbel
  distribution and we deduce
  :math:`(\mu_0, \sigma_0)` from the empirical mean :math:`\hat{M}` and the empirical standard variation
  :math:`\hat{\sigma}` of the data:
  :math:`\sigma_0 = \dfrac{\sqrt{6}}{\pi} \hat{\sigma}` and :math:`\mu_0 = \hat{M} - \gamma
  \sigma_0` where
  :math:`\gamma` is Euler's constant; then we take the initial point :math:`(\mu_0, \sigma_0,
  \xi_0 = 0.1)`. This is the default initial point;
- the *Static* initial point: in that case, we assume that the GEV is stationary and
  :math:`(\mu_0, \sigma_0, \xi_0)`
  is the maximum likelihood estimate resulting from that assumption.

The result class produced by the method provides:

- the estimator :math:`\hat{\vect{\beta}}`,
- the asymptotic distribution of :math:`\hat{\vect{\beta}}`,
- the parameter functions :math:`t \mapsto \vect{\theta}(t)`,
- the normalizing function :math:`t \mapsto \tau(t)`,
- the optimal log-likelihood value :math:`\hat{\vect{\beta}}`,
- the GEV distribution at time :math:`t`,
- the quantile functions of order :math:`p`: :math:`t \mapsto q_p(Z_t)`.)RAW"

// ----------------------------------------------------------------------------

%feature("docstring") OT::GeneralizedExtremeValueFactory::buildReturnLevelEstimator
R"RAW(Estimate a return level and its distribution from the GEV parameters.

Parameters
----------
result : :class:`~openturns.DistributionFactoryResult`
    Likelihood estimation result of a :class:`~openturns.GeneralizedExtremeValue`
m : float
    The return period expressed in terms of number of blocks.

Returns
-------
distribution : :class:`~openturns.Distribution`
    The asymptotic distribution of :math:`\hat{z}_m`.

Notes
-----
Let :math:`Z` be a random variable which follows a GEV distribution parameterized by
:math:`\vect{\theta} = (\mu, \sigma, \xi)`.

The :math:`m`-block return level :math:`z_m` is the level exceeded on average once every :math:`m`
blocks.
The :math:`m`-block return level can be translated into the annual-scale: if there
are :math:`n_y` blocks per year, then the :math:`N`-year return level
corresponds to the :math:`m`-bock return level where :math:`m = n_yN`.

The :math:`m`-block return level is defined as the quantile of order :math:`1-p=1-1/m` of the GEV
distribution:

If :math:`\xi \neq 0`:

.. math::
    :label: zm1

    z_m = \mu - \frac{\sigma}{\xi} \left[ 1- (-\log(1-p))^{-\xi}\right]

If :math:`\xi = 0`:

.. math::
    :label: zm2

    z_m = \mu - \sigma \log(-\log(1-p))

The estimator :math:`\hat{z}_m` of :math:`z_m` is deduced from the estimator
:math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})` of :math:`(\mu, \sigma, \xi)`. 

The asymptotic distribution of :math:`\hat{z_m}` is obtained by the Delta method from the asymptotic distribution of
:math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`. It is a normal distribution with mean :math:`\hat{z}_m` and variance:

.. math::

    \Var{z_m} = (\nabla z_m)^T \mat{V}_n \nabla z_m

where :math:`\nabla z_m = (\frac{\partial z_m}{\partial \mu}, \frac{\partial z_m}{\partial \sigma}, \frac{\partial z_m}{\partial \xi})`
and :math:`\mat{V}_n` is the asymptotic covariance of :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`.)RAW"

// ----------------------------------------------------------------------------

%feature("docstring") OT::GeneralizedExtremeValueFactory::buildReturnLevelProfileLikelihoodEstimator
R"RAW(Estimate :math:`(z_m, \sigma, \xi)` and its distribution with the profile likelihood.

Parameters
----------
sample : 2-d sequence of float
    The block maxima sample of dimension 1.
m : float
    The return period expressed in terms of number of blocks.
    
Returns
-------
result : :class:`~openturns.ProfileLikelihoodResult`
    The result class.

Notes
-----
Let :math:`Z` be a random variable which follows a GEV distribution parameterized by
:math:`\vect{\theta} = (\mu, \sigma, \xi)`.

The  :math:`m`-block return level :math:`z_m` is defined in :meth:`buildReturnLevelEstimator`.
The profile log-likelihood :math:`\ell_p(z_m)` is defined in :meth:`buildReturnLevelProfileLikelihood`.

The estimator of  :math:`(\hat{z}_m, \hat{\sigma}, \hat{\xi})` is defined by:

.. math::

    \hat{z}_m & = \argmax_{z_m} \ell_p(z_m)\\
    (\hat{\sigma}, \hat{\xi}) & = \argmax_{(\sigma, \xi)} \ell(\hat{z}_m, \sigma, \xi)

The result class produced by the method provides:

- the GEV distribution associated to :math:`(\hat{z}_m, \hat{\sigma}, \hat{\xi})`,
- the asymptotic distribution of :math:`(\hat{z}_m, \hat{\sigma}, \hat{\xi})`,
- the profile log-likelihood function :math:`z_m \mapsto \ell_p(z_m)`,
- the optimal profile log-likelihood value :math:`\ell_p(\hat{z}_m)`,
- confidence intervals of level :math:`(1-\alpha)` of :math:`\hat{z}_m`.)RAW"

// ----------------------------------------------------------------------------

%feature("docstring") OT::GeneralizedExtremeValueFactory::buildReturnLevelProfileLikelihood
R"RAW(Estimate a return level and its distribution with the profile likelihood.


Parameters
----------
sample : 2-d sequence of float
    The block maxima sample of dimension 1.

Returns
-------
distribution : :class:`~openturns.Normal`
    The asymptotic distribution of :math:`\hat{z}_m`.

Notes
-----
Let :math:`Z` be a random variable which follows a GEV distribution parameterized by
:math:`\vect{\theta} = (\mu, \sigma, \xi)`.

The :math:`m`-return level :math:`z_m` is defined in :meth:`buildReturnLevelEstimator`.

The estimator is defined using a nested numerical optimization of the log-likelihood:

.. math::

    \ell_p (z_m) = \max_{(\mu, \sigma)} \ell (z_m, \sigma, \xi)

where :math:`\ell (z_m, \sigma, \xi)` is the log-likelihood detailed in :eq:`llR1` and :eq:`llR2` with :math:`r=1` and where we substitued
:math:`\mu` for :math:`z_m` using equations :eq:`zm1` or :eq:`zm2`.

The estimator :math:`\hat{z}_m` of :math:`z_m` is defined by:

.. math::
    
    \hat{z}_m = \argmax_{z_m} \ell_p(z_m)
    
The asymptotic distribution of :math:`\hat{z}_m` is normal.

The starting point of the optimization is initialized from the regular maximum likelihood method.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GeneralizedExtremeValueFactory::setOptimizationAlgorithm
"Accessor to the solver.

Parameters
----------
solver : :class:`~openturns.OptimizationAlgorithm`
    The solver used for numerical optimization of the moments."

// ---------------------------------------------------------------------

%feature("docstring") OT::GeneralizedExtremeValueFactory::getOptimizationAlgorithm
"Accessor to the solver.

Returns
-------
solver : :class:`~openturns.OptimizationAlgorithm`
    The solver used for numerical optimization of the moments."
