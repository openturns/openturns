%feature("docstring") OT::MarginalDistribution
"Marginal distribution.

Allows one to extract marginals of a distribution when it does not natively
support it.

Parameters
----------
distribution : :class:`~openturns.Distribution`
    The underlying distribution.
indices : sequence of int
    Marginal indices.

This class uses the following entries of :class:`~openturns.ResourceMap`:

- *MarginalDistribution-MaximumError* (`Scalar`, default: `1e-07`): admitted error of the numerical
  integration of the marginal distributions,
- *MarginalDistribution-MaximumSubIntervals* (`UnsignedInteger`, default: `128`): maximum number of
  sub-intervals used by the adaptive numerical integration,
- *MarginalDistribution-Rule* (`String`, default: `G15K31`): name of the Gauss-Kronrod integration rule
  used by the numerical integration, among `G1K3`, `G3K7`, `G7K15`, `G11K23`, `G15K31` and `G25K51`,
- *MarginalDistribution-UsePDF* (`Bool`, default: `true`): if true, the marginals are computed from the
  probability density function, otherwise they are computed from the cumulative distribution function.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::MarginalDistribution::setDistribution
"Accessor to the distribution.

Parameters
----------
distribution : :class:`~openturns.Distribution`
    The underlying distribution."

// ---------------------------------------------------------------------

%feature("docstring") OT::MarginalDistribution::getDistribution
"Accessor to the distribution.

Returns
-------
distribution : :class:`~openturns.Distribution`
    The underlying distribution."

// ---------------------------------------------------------------------

%feature("docstring") OT::MarginalDistribution::setIndices
"Accessor to the marginal indices.

Parameters
----------
indices : sequence of int
    Marginal indices."

// ---------------------------------------------------------------------

%feature("docstring") OT::MarginalDistribution::getIndices
"Accessor to the marginal indices.

Returns
-------
indices : :class:`~openturns.Indices`
    Marginal indices."

// ---------------------------------------------------------------------

%feature("docstring") OT::MarginalDistribution::setIntegrationAlgorithm
"Accessor to the integration algorithm used to compute the PDF.

Parameters
----------
algo : :class:`~openturns.IntegrationAlgorithm`
    The integration algorithm used to marginalize the unwanted
    components of the PDF."

// ---------------------------------------------------------------------

%feature("docstring") OT::MarginalDistribution::getIntegrationAlgorithm
"Accessor to the integration algorithm used to compute the PDF.

Returns
-------
algo : :class:`~openturns.IntegrationAlgorithm`
    The integration algorithm used to marginalize the unwanted
    components of the PDF."

// ---------------------------------------------------------------------

%feature("docstring") OT::MarginalDistribution::setUsePDF
"Accessor to the flag telling how the PDF is computed.

Parameters
----------
flag : bool
    Flag telling if the marginal PDF is computed using an integration
    of the PDF of the underlying distribution or if it is computed
    using finite differences of the underlying CDF."

// ---------------------------------------------------------------------

%feature("docstring") OT::MarginalDistribution::getUsePDF
"Accessor to the flag telling how the PDF is computed.

Returns
-------
flag : bool
    Flag telling if the marginal PDF is computed using an integration
    of the PDF of the underlying distribution or if it is computed
    using finite differences of the underlying CDF."


