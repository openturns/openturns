%define OT_ProjectionStrategy_doc
R"RAW(Base class for the evaluation strategies of the approximation coefficients.

Available constructors:
    ProjectionStrategy(*projectionStrategy*)

Parameters
----------
projectionStrategy : :class:`~openturns.ProjectionStrategy`
    A projection strategy which is a :class:`~openturns.LeastSquaresStrategy` or
    an :class:`~openturns.IntegrationStrategy`.

See also
--------
FunctionalChaosAlgorithm, LeastSquaresStrategy, IntegrationStrategy

Notes
-----
This class is used in the :ref:`functional chaos expansion context <functional_chaos>` implemented
in the class :class:`~openturns.FunctionalChaosAlgorithm`. It is not usable outside this context. 

The model is approximated by the meta model defined in :ref:`functional_chaos` by equation :eq:`metaModelPn` and the coefficients :math:`(a_k)_{k \in I_n}` are computed by:

- solving the least squares problem defined in :ref:`functional_chaos` by equation
  :eq:`metaModeleF` : use the class
  :class:`~openturns.LeastSquaresStrategy`,
- computing the inner product defined in :ref:`functional_chaos` by equation
  :eq:`scalProd`: use the class
  :class:`~openturns.IntegrationStrategy`. In that case, the basis of the approximation space
  must be orthonormal to the input distribution.

)RAW"
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation
OT_ProjectionStrategy_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_getCoefficients_doc
R"RAW(Accessor to the coefficients.

Returns
-------
coef : :class:`~openturns.Point`
    Coefficients :math:`(\alpha_k)_{k \in I_n}`.)RAW"
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::getCoefficients
OT_ProjectionStrategy_getCoefficients_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_getExperiment_doc
"Accessor to the experiments.

Returns
-------
exp : :class:`~openturns.WeightedExperiment`
    Weighted experiment used to evaluate the coefficients."
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::getExperiment
OT_ProjectionStrategy_getExperiment_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_setInputSample_doc
"Accessor to the input sample.

Parameters
----------
X : :class:`~openturns.Sample`
    Input Sample."
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::setInputSample
OT_ProjectionStrategy_setInputSample_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_getInputSample_doc
"Accessor to the input sample.

Returns
-------
X : :class:`~openturns.Sample`
    Input Sample."
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::getInputSample
OT_ProjectionStrategy_getInputSample_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_getMeasure_doc
R"RAW(Accessor to the measure.

Returns
-------
mu : Distribution
    Measure :math:`\mu_{\inputRV}` defining the inner product.)RAW"
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::getMeasure
OT_ProjectionStrategy_getMeasure_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_setOutputSample_doc
"Accessor to the output sample.

Parameters
----------
Y : :class:`~openturns.Sample`
    Output Sample."
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::setOutputSample
OT_ProjectionStrategy_setOutputSample_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_getOutputSample_doc
"Accessor to the output sample.

Returns
-------
Y : :class:`~openturns.Sample`
    Output Sample."
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::getOutputSample
OT_ProjectionStrategy_getOutputSample_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_getRelativeError_doc
"Accessor to the relative error.

Returns
-------
e : float
    Relative error."
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::getRelativeError
OT_ProjectionStrategy_getRelativeError_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_getResidual_doc
"Accessor to the residual.

Returns
-------
er : float
    Residual error."
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::getResidual
OT_ProjectionStrategy_getResidual_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_setWeights_doc
"Accessor to the weights.

Parameters
----------
w : :class:`~openturns.Point`
    Weights of the design of experiments."
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::setWeights
OT_ProjectionStrategy_setWeights_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_getWeights_doc
"Accessor to the weights.

Returns
-------
w : :class:`~openturns.Point`
    Weights of the design of experiments."
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::getWeights
OT_ProjectionStrategy_getWeights_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_setExperiment_doc
"Accessor to the design of experiment.

Parameters
----------
exp : :class:`~openturns.WeightedExperiment`
    Weighted design of experiment."
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::setExperiment
OT_ProjectionStrategy_setExperiment_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_setMeasure_doc
R"RAW(Accessor to the measure.

Parameters
----------
m : Distribution
    Measure :math:`\mu_{\inputRV}` defining the scalar product.)RAW"
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::setMeasure
OT_ProjectionStrategy_setMeasure_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_getDesignProxy_doc
"Accessor to the design proxy.

Parameters
----------
designProxy : :class:`~openturns.DesignProxy`
    The design matrix."
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::getDesignProxy
OT_ProjectionStrategy_getDesignProxy_doc

%define OT_ProjectionStrategy_isLeastSquares_doc
"Get the least squares flag.

There are two methods to compute the coefficients: integration
or least squares.

Returns
-------
isLeastSquares : bool
    True if the coefficients are estimated from least squares."
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::isLeastSquares
OT_ProjectionStrategy_isLeastSquares_doc

// ---------------------------------------------------------------------

%define OT_ProjectionStrategy_involvesModelSelection_doc
"Get the model selection flag.

A model selection method can be used to select the coefficients
of the decomposition which enable to best predict the output.
Model selection can lead to a sparse functional chaos expansion.

Returns
-------
involvesModelSelection : bool
    True if the method involves a model selection method."
%enddef
%feature("docstring") OT::ProjectionStrategyImplementation::involvesModelSelection
OT_ProjectionStrategy_involvesModelSelection_doc
