%feature("docstring") OT::ProfileLikelihoodResult
R"RAW(Distribution factory result for profile likelihood estimation.

This  class provides all the results obtained after a profile likelihood estimation with respect to the
parameter :math:`\vect{\theta}^{(1)}`: refer to :meth:`~openturns.HypothesisTest.LikelihoodRatioTest`.

Parameters
----------
distribution : :class:`~openturns.Distribution`
    Parent distribution at optimal parameter :math:`(\hat{\vect{\theta}}^{(1)}, \hat{\vect{\theta}}^{(2)})`.
parameterDistribution : :class:`~openturns.Distribution`
    Asymptotic distribution of :math:`(\hat{\vect{\theta}}^{(1)}, \hat{\vect{\theta}}^{(2)})`.
logLikelihood : float
    Maximum profile log-likelihood.
profileLikelihood : :class:`~openturns.Function`
    Profile log-likelihood function with respect to the scalar parameter :math:`\theta^{(1)}`.
parameter : float
    Estimator of :math:`\vect{\theta}^{(1)}`.

See also
--------
GeneralizedExtremeValueFactory

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``ProfileLikelihoodResult-AbsolutePrecision`` (``Scalar``, default: ``1.0e-10``): absolute precision of the solver used to compute the confidence interval
- ``ProfileLikelihoodResult-ConfidenceIntervalMargin`` (``Scalar``, default: ``0.05``): margin used to enlarge the search interval of the confidence interval
- ``ProfileLikelihoodResult-RelativePrecision`` (``Scalar``, default: ``1.0e-10``): relative precision of the solver used to compute the confidence interval
- ``ProfileLikelihoodResult-StartingScaling`` (``Scalar``, default: ``1.0e-2``): initial value of the :math:`\rho` parameter used to run the profile likelihood computation

- ``ProfileLikelihoodResult-TextMargin`` (``Scalar``, default: ``0.03``): margin used to place the text on the graph of the profile likelihood

)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProfileLikelihoodResult::getParameterDistribution
R"RAW(Distribution of :math:`(\vect{\theta}^{(1)}, \vect{\theta}^{(2)})` accessor.

Returns
-------
parameterDistribution : :class:`~openturns.Distribution`
    Estimator distribution of :math:`(\vect{\theta}^{(1)}, \vect{\theta}^{(2)})`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProfileLikelihoodResult::setConfidenceLevel
R"RAW(Confidence level accessor.

Parameters
----------
level : float
    The confidence level :math:`(1-\alpha)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProfileLikelihoodResult::getConfidenceLevel
R"RAW(Confidence level accessor.

Returns
-------
level : float
    The confidence level :math:`(1-\alpha)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProfileLikelihoodResult::getParameter
R"RAW(Estimator of :math:`\vect{\theta}^{(1)}`.

Returns
-------
parameter : float
    Estimator of :math:`\vect{\theta}^{(1)}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProfileLikelihoodResult::getParameterConfidenceInterval
R"RAW(Confidence interval accessor.

Returns
-------
ci : :class:`~openturns.Interval`
    Confidence interval of :math:`\vect{\theta}^{(1)}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProfileLikelihoodResult::getProfileLikelihoodFunction
R"RAW(Profile log-likelihood function accessor.

Returns
-------
ll : :class:`~openturns.Function`
    Profile log-likelihood function with respect to :math:`\vect{\theta}^{(1)}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProfileLikelihoodResult::drawProfileLikelihoodFunction
R"RAW(Draw the profile likelihood graph with respect to the scalar parameter :math:`\theta^{(1)}`.

If the parameter :math:`\theta^{(1)}` is scalar, the graph of the profile log-likelihood: :math:`\theta^{(1)} \mapsto \ell_p(\theta^{(1)})` provides:

- the estimator :math:`\hat{\theta}^{(1)}` of  :math:`\theta^{(1)}` that maximizes the log-profile likelihood,
- the :math:`(1-\alpha)`-confidence interval of :math:`\theta^{(1)}` built from the profile deviance statistics :math:`\mathcal{D}_p (\theta^{(1)})`.

Returns
-------
graph : :class:`~openturns.Graph`
    Profile likelihood graph with respect to the scalar parameter :math:`\theta^{(1)}`.)RAW"
