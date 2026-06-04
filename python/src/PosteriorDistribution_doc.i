%feature("docstring") OT::PosteriorDistribution
R"RAW(Distribution conditioned by observations.

Let :math:`\inputRV` be a random vector such that :math:`\inputRV|\vect{\Theta}` follows the distribution
:math:`\mathcal{L}_{\inputRV|\vect{\Theta}}`,
with :math:`\vect{\Theta}=g(\vect{Y})` and :math:`\vect{Y}` following the prior distribution :math:`\mathcal{L}_{\vect{Y}}`. The function
:math:`g` is the link function which input dimension
is the dimension of :math:`\mathcal{L}_{\vect{Y}}` and which output dimension the dimension of :math:`\vect{\Theta}`.

This class implements the posterior distribution of :math:`\vect{Y}` given that we have a sample :math:`(\vect{s}_1, \dots, \vect{s}_\sampleSize)` of
:math:`\vect{X}`.

This class requires the following features:

- the :math:`\inputRV` may be continuous, discrete or neither: e.g., it can be a
  :class:`~openturns.Mixture` of discrete and continuous distributions. In that case, its parameters set is the union of the
  parameters set of each of its atoms (the weights of the mixture are not considered as parameters).
- each component :math:`Y_i` is continuous or discrete: e.g., it can not be a :class:`~openturns.Mixture` of
  discrete and continuous distributions, (the random vector :math:`\vect{Y}` may have some discrete components and some continuous components), 
- the copula of :math:`\vect{Y}` is continuous: e.g., it can not be the :class:`~openturns.MinCopula`.

If  :math:`\vect{Y}` and :math:`\vect{X}` are continuous random vector, then the posterior PDF of :math:`\vect{Y}` is defined by:

.. math::
    :label: postPDF

    f_{\vect{Y}|\inputRV_1 = \vect{x}_1, \dots, \inputRV_\sampleSize =  \vect{x}_\sampleSize}(\vect{y}) = \frac{f_{\vect{Y}}(\vect{y})
    \prod_{i=1}^\sampleSize f_{\inputRV|\vect{\theta} = g(\vect{y})}(\vect{x}_i)}{\int f_{\vect{Y}}(\vect{y})\prod_{i=1}^\sampleSize
    f_{\inputRV|\vect{\theta} = g(\vect{y})}(\vect{x}_i) d \vect{y}}

with :math:`f_{\inputRV|\vect{\theta} = g(\vect{y})}` the PDF of the distribution of :math:`\inputRV|\vect{\Theta}`
where :math:`\vect{\Theta}` has been replaced by :math:`g(\vect{y})` and :math:`f_{\vect{Y}}` the PDF of the prior distribution
of  :math:`\vect{Y}`.

Note that the denominator  of :eq:`postPDF` is the PDF of the compound distribution of :math:`\inputRV|\vect{\Theta}=g(\vect{Y})`
with respect to the prior distribution of :math:`\vect{Y}`.

In the other cases, the PDF is the probability distribution function for the discrete components and the :math:`\int` are replaced by some
:math:`\sum`.

Parameters
----------
compound : :class:`~openturns.CompoundDistribution`
    The distribution of density :math:`f_{\vect{X}}(\vect{x}) = \int f_{\vect{X}|\vect{\Theta}=\vect{y}}(\vect{x}|\vect{y}) f_{\vect{Y}}(\vect{y})\di{\vect{y}}`.
observations : 2-d sequence of float
    Observations :math:`\vect{x}_i`.

See Also
--------
CompoundDistribution, MetropolisHastings

Examples
--------
>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> conditionedDistribution = ot.Normal()
>>> # Compound distribution
>>> atoms = [ot.Uniform(0.0, 1.0), ot.Uniform(1.0, 2.0)]
>>> conditioning = ot.JointDistribution(atoms)
>>> compound = ot.CompoundDistribution(conditionedDistribution, conditioning)
>>> # Observations
>>> observationsSize = 5
>>> observationsDistribution = ot.Distribution(conditionedDistribution)
>>> observationsDistribution.setParameter(conditioning.getMean())
>>> observations = observationsDistribution.getSample(observationsSize)
>>> # Posterior distribution
>>> distribution = ot.PosteriorDistribution(compound, observations)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PosteriorDistribution::setCompoundDistribution
"Compound distribution accessor.

Parameters
----------
compound : :class:`~openturns.CompoundDistribution`
    The compound distribution."

// ---------------------------------------------------------------------

%feature("docstring") OT::PosteriorDistribution::getCompoundDistribution
"Compound distribution accessor.

Returns
-------
compound : :class:`~openturns.CompoundDistribution`
    The compound distribution."

// ---------------------------------------------------------------------

%feature("docstring") OT::PosteriorDistribution::setObservations
"Accessor to the observations.

Parameters
----------
observations : 2-d sequence of float
    Observations."

// ---------------------------------------------------------------------

%feature("docstring") OT::PosteriorDistribution::getObservations
"Accessor to the observations.

Returns
-------
observations : :class:`~openturns.Sample`
    Observations."

// ---------------------------------------------------------------------

%feature("docstring") OT::PosteriorDistribution::getLogNormalizationFactor
"Normalization factor accessor.

Returns
-------
factor : float
    Log-scale normalization factor."

// ---------------------------------------------------------------------

%feature("docstring") OT::PosteriorDistribution::getConditionedDistribution
R"RAW(Accessor to the distribution's conditioned distribution parameter *conditionedDistribution*.

Returns
-------
conditionedDistribution : :class:`~openturns.Distribution`
    the distribution of :math:`\vect{X}|\vect{\Theta}`,
    whose parameters will be overwritten by :math:`g(\vect{Y})`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PosteriorDistribution::getConditioningDistribution
R"RAW(Accessor to the distribution's conditioned distribution parameter *conditioningDistribution*.

Returns
-------
conditioningDistribution : :class:`~openturns.Distribution`
    the distribution of :math:`\vect{Y}`, specified with its parameters.
)RAW"
// ---------------------------------------------------------------------

%feature("docstring") OT::PosteriorDistribution::getLinkFunction
R"RAW(Accessor to the distribution's conditioned distribution parameter *linkFunction*.

Returns
-------
linkFunction : :class:`~openturns.Function`
    the function :math:`g: \Rset^p \rightarrow \Rset^q`, where :math:`p` is the dimension of
    *conditioningDist* and :math:`q` is the dimension of :math:`\vect{\Theta}`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PosteriorDistribution::setConditionedDistribution
R"RAW(Accessor to the distribution's conditioned distribution parameter *conditionedDistribution*.

Parameters
----------
conditionedDistribution : :class:`~openturns.Distribution`
    the distribution of :math:`\vect{X}|\vect{\Theta}`,
    whose parameters will be overwritten by :math:`g(\vect{Y})`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PosteriorDistribution::setConditioningDistribution
R"RAW(Accessor to the distribution's conditioned distribution parameter *conditioningDistribution*.

Parameters
----------
conditioningDistribution : :class:`~openturns.Distribution`
    the distribution of :math:`\vect{Y}`, specified with its parameters.
)RAW"
// ---------------------------------------------------------------------

%feature("docstring") OT::PosteriorDistribution::setLinkFunction
R"RAW(Accessor to the distribution's conditioned distribution parameter *linkFunction*.

Parameters
----------
linkFunction : :class:`~openturns.Function`
    the function :math:`g: \Rset^p \rightarrow \Rset^q`, where :math:`p` is the dimension of
    *conditioningDist* and :math:`q` is the dimension of :math:`\vect{\Theta}`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PosteriorDistribution::computeLogNormalizedLikelihood
R"RAW(Compute the log-likelihood including the normalization constant.

Parameters
----------
x : sequence of float, 2-d sequence of float
    Point in :math:`\Rset^\inputDim`.

Returns
-------
value : float
    The value of the log-likelihood at *x*.

Notes
-----
The normalized log-likelihood is the log-PDF of the posterior distribution defined in :eq:`postPDF`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PosteriorDistribution::computeNormalizedLikelihood
R"RAW(Compute the likelihood including the normalization constant.

Parameters
----------
x : sequence of float, 2-d sequence of float
    Point in :math:`\Rset^\inputDim`.

Returns
-------
value : float
    The value of the likelihood at *x*.

Notes
-----
The normalized likelihood is the PDF of the posterior distribution defined in :eq:`postPDF`.)RAW"
