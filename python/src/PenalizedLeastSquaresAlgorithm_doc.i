%feature("docstring") OT::PenalizedLeastSquaresAlgorithm
R"RAW(Penalized least squares algorithm.

Refer to :any:`least_squares`.

Available constructors:
    PenalizedLeastSquaresAlgorithm(*x, y, psi, indices, penalizationFactor=0, useNormal=False*)

    PenalizedLeastSquaresAlgorithm(*x, y, weight, psi, indices, penalizationFactor=0, useNormal=False*)

    PenalizedLeastSquaresAlgorithm(*x, y, weight, psi, indices, penalizationFactor=0, penalizationMatrix, useNormal=False*)

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
psi : sequence of :class:`~openturns.Function`
    Basis
indices : sequence of int
    Indices allowed in the basis
penalizationFactor : float, optional
    Penalization factor
penalizationMatrix : 2-d sequence of float
    Penalization matrix
useNormal : bool, optional
    Solve the normal equation

See also
--------
ApproximationAlgorithm, LeastSquaresMetaModelSelection

Notes
-----
Refer to the :any:`least squares context <least_squares>` and  :any:`least_squares_resolution` to get details.

This class solves the least squares problem for each output marginal :math:`k \in \{1, ..., \outputDim\}` defined by equation :eq:`defA` in the :any:`least squares context <least_squares>`.

The finite discrete distribution based on the input sample and the weights defines the inner product that will be used to solve the least squares problem.)RAW"
