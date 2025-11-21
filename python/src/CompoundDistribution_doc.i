%feature("docstring") OT::CompoundDistribution
R"RAW(Compound distribution.

Let :math:`\inputRV` be a random vector of dimension :math:`\inputDim` such that
:math:`\vect{X}|\vect{\Theta}` follows the distribution :math:`\mathcal{L}_{\vect{X}|\vect{\Theta}}`,
with :math:`\vect{\Theta}=g(\vect{Y})`, :math:`\vect{Y}` following the distribution
:math:`\cL_{\vect{Y}}` and :math:`g` being a function which input
dimension is the dimension of :math:`\cL_{\vect{Y}}` and which output dimension is the
dimension of :math:`\vect{\Theta}`.

Then this class implements the distribution of :math:`\inputRV`.

This class requires the following features:

- the :math:`\inputRV` may be continuous, discrete or neither: e.g., it can be a
  :class:`~openturns.Mixture` of discrete and continuous distributions. In that case, its parameters set is the union of the
  parameters set of each of its atoms (the weights of the mixture are not considered as parameters).
- each component :math:`Y_i` is continuous or discrete: e.g., it can not be a :class:`~openturns.Mixture` of
  discrete and continuous distributions, (the random vector :math:`\vect{Y}` may have some discrete components and some continuous components), 
- the copula of :math:`\vect{Y}` is continuous: e.g., it can not be the :class:`~openturns.MinCopula`,
- if :math:`\vect{Y}` has both discrete components and continuous components, its copula must be the independent copula. The general case has
  not been implemented yet.

We define: 

.. math::

    p_{\vect{Y}}(\vect{y}) = \left( \prod_{i=1}^\inputDim p_i(y_i) \right) c(F_1(x_1), \dots, F_d(x_d))

where:

- :math:`c` is the probability density copula of :math:`\vect{Y}`,
- if :math:`Y_i` is a continuous component, :math:`p_i` is its probability density function,
- if :math:`Y_i` is a discrete component, :math:`p_i = \sum_{y^i_k \in \cS^i} \Prob{Y_i = y^i_k} \delta_{y^i_k}` where
  :math:`\cS^i = \{ y^i_k \}` is its support and :math:`\delta_{y^i_k}` the Dirac distribution centered on
  :math:`y^i_k`.


Then, the pdf of :math:`\inputRV` is defined by:

.. math::

    p_{\vect{X}}(\vect{x}) = \int p_{\vect{X}|\vect{\Theta}=g(\vect{y})}(\vect{x}|g(\vect{y})) p_{\vect{Y}}(\vect{y})\di{\vect{y}}

with the same convention as for :math:`\vect{Y}`.

Parameters
----------

conditionedDist : :class:`~openturns.Distribution`
    The distribution of :math:`\vect{X}|\vect{\Theta}`,
    whose parameters will be overwritten by :math:`g(\vect{Y})`.

    In the default constructor, :math:`\vect{X} | \vect{\Theta} \sim` :class:`~openturns.Uniform` :math:`(\vect{\Theta})`.

conditioningDist : :class:`~openturns.Distribution`
    The distribution of :math:`\vect{Y}`.

    In the default constructor, :math:`\vect{Y} \sim` :class:`~openturns.Uniform`:math:`(-1,1)`.

linkFunction : :class:`~openturns.Function`, optional
    The link unction :math:`g: \Rset^p \rightarrow \Rset^q`, where :math:`p` is the dimension of
    *conditioningDist* and :math:`q` is the dimension of :math:`\vect{\Theta}`.

    When not provided :math:`g = Id` and :math:`p=q`.
    
    In the default constructor, :math:`g: \Rset \rightarrow \Rset^2` with :math:`g(y)=(y,y+1)`.


Notes
-----
With the default constructor, the resulting random variable :math:`X` follows a
trapezoidal distribution parameterized by :math:`(-1, 0, 1, 2)`.

Note that a :class:`~openturns.CompoundDistribution` can be obtained as the
first marginal distribution of a :class:`~openturns.JointByConditioningDistribution`.

Several :class:`~openturns.ResourceMap` entries allow for a more fine-grained control over the distribution:

- The string entry *CompoundDistribution-ContinuousDiscretizationMethod* specifies the default
  :class:`~openturns.WeightedExperiment` which is used to discretize the continuous part of the conditioning distribution
  in order to perform the integration. It can be *GaussProduct*, *QMC* or *MC*: The default is *GaussProduct*, well adapted if the conditioned model is *regular*, meaning that its range does not depend on the parameters (e.g. a :class:`~openturns.Normal` distribution). Otherwise, one of the other discretization methods is to be 
  preferred (e.g. for a :class:`~openturns.Uniform` distribution).
- The integer entry *CompoundDistribution-MarginalIntegrationNodesNumber* specifies the number of nodes used to the
  integration in each direction (only active when a quadrature method is applied),
- The integer entry *CompoundDistribution-MarginalIntegrationNodesNumber* specifies the maximum number of nodes used to 
  the multivariate integration (only active when a quadrature method is applied).

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> conditioningDist = ot.Normal(0.0, 1.0)
>>> g = ot.SymbolicFunction(['y'], ['y', '0.1+y^2'])
>>> conditionedDist = ot.Normal()
>>> finalDist = ot.CompoundDistribution(conditionedDist, conditioningDist, g)  # doctest: +SKIP

Draw a sample:

>>> sample = finalDist.getSample(5)  # doctest: +SKIP
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CompoundDistribution::getConditionedDistribution
R"RAW(Accessor to the distribution's conditioned distribution parameter *conditionedDistribution*.

Returns
-------
conditionedDistribution : :class:`~openturns.Distribution`
    the distribution of :math:`\vect{X}|\vect{\Theta}`,
    whose parameters will be overwritten by :math:`g(\vect{Y})`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CompoundDistribution::getConditioningDistribution
R"RAW(Accessor to the distribution's conditioned distribution parameter *conditioningDistribution*.

Returns
-------
conditioningDistribution : :class:`~openturns.Distribution`
    the distribution of :math:`\vect{Y}`, specified with its parameters.
)RAW"
// ---------------------------------------------------------------------

%feature("docstring") OT::CompoundDistribution::getLinkFunction
R"RAW(Accessor to the distribution's conditioned distribution parameter *linkFunction*.

Returns
-------
linkFunction : :class:`~openturns.Function`
    the function :math:`g: \Rset^p \rightarrow \Rset^q`, where :math:`p` is the dimension of
    *conditioningDist* and :math:`q` is the dimension of :math:`\vect{\Theta}`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CompoundDistribution::setConditionedDistribution
R"RAW(Accessor to the distribution's conditioned distribution parameter *conditionedDistribution*.

Parameters
----------
conditionedDistribution : :class:`~openturns.Distribution`
    the distribution of :math:`\vect{X}|\vect{\Theta}`,
    whose parameters will be overwritten by :math:`g(\vect{Y})`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CompoundDistribution::setConditioningDistribution
R"RAW(Accessor to the distribution's conditioned distribution parameter *conditioningDistribution*.

Parameters
----------
conditioningDistribution : :class:`~openturns.Distribution`
    the distribution of :math:`\vect{Y}`, specified with its parameters.
)RAW"
// ---------------------------------------------------------------------

%feature("docstring") OT::CompoundDistribution::setLinkFunction
R"RAW(Accessor to the distribution's conditioned distribution parameter *linkFunction*.

Parameters
----------
linkFunction : :class:`~openturns.Function`
    the function :math:`g: \Rset^p \rightarrow \Rset^q`, where :math:`p` is the dimension of
    *conditioningDist* and :math:`q` is the dimension of :math:`\vect{\Theta}`.
)RAW"
