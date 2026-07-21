%feature("docstring") OT::ApproximationAlgorithmImplementationFactory
"Approximation algorithm factory base class.

Notes
-----
It represents a generic class (virtual) for different factories like
:class:`~openturns.PenalizedLeastSquaresAlgorithmFactory` or
:class:`~openturns.LeastSquaresMetaModelSelectionFactory`

This class is not usable because it operates only within the class
:class:`~openturns.FunctionalChaosAlgorithm`.

See also
--------
PenalizedLeastSquaresAlgorithmFactory, LeastSquaresMetaModelSelectionFactory"

// ---------------------------------------------------------------------

%feature("docstring") OT::ApproximationAlgorithmImplementationFactory::involvesModelSelection
"Get the model selection flag.

A model selection method can be used to select the coefficients
of the decomposition which enable to best predict the output.
Model selection leads to a sparse functional chaos expansion.

Returns
-------
involvesModelSelection : bool
    True if the method involves a model selection method."

// ---------------------------------------------------------------------

%feature("docstring") OT::ApproximationAlgorithmImplementationFactory::build
R"RAW(Build the approximation.

Parameters
----------
x : 2-d sequence of float
    The input sample :math:`\left\{\vect{x}^{(1)}, ..., \vect{x}^{(\sampleSize)}\right\}`
    where :math:`\vect{y}\in \Rset^\inputDim`
y : 2-d sequence of float
    The output sample :math:`\left\{\vect{y}^{(1)}, ..., \vect{y}^{(\sampleSize)}\right\}`
    where :math:`\vect{y}\in \Rset^\outputDim`
weight : sequence of float
    Weights
    
    By default, all the weights are equal to :math:`\frac{1}{\sampleSize}`
psi : sequence of :class:`~openturns.Function`
    The functional basis.
indices : sequence of int
    Indices in the basis.

Returns
-------
algorithm: :class:`~openturns.ApproximationAlgorithm`
    The estimation algorithm.

Notes
-----
The finite discrete distribution based on the input sample and the weights defines the inner product that will be used to solve the least squares problem.
)RAW"

// ---------------------------------------------------------------------
