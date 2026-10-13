%feature("docstring") OT::SparseMethod
"Least squares solver using a sparse representation.

Available constructors:
    SparseMethod(*method*)

    SparseMethod(*method, basisSequenceFactory, fittingAlgorithm*)

Parameters
----------
method : :class:`~openturns.LinearLeastSquaresMethod`
    Least squares resolution method
basisSequenceFactory : :class:`~openturns.BasisSequenceFactory`
    Basis enumeration algorithm
fittingAlgorithm : :class:`~openturns.FittingAlgorithm`
    Validation algorithm

See also
--------
LinearLeastSquaresMethod

Examples
--------
>>> import openturns as ot
>>> basisSize = 3
>>> sampleSize = 5
>>> X = ot.Sample.BuildFromPoint(range(1, 1 + sampleSize))
>>> phis = [ot.SymbolicFunction(['x'], ['x^' + str(j + 1)]) for j in range(basisSize)]
>>> basis = ot.Basis(phis)
>>> proxy = ot.DesignProxy(X, phis)
>>> full = range(basisSize)
>>> design = ot.Matrix(proxy.computeDesign(full))
>>> method = ot.SparseMethod(ot.QRMethod(proxy, full))
>>> normal = ot.Normal([1.0] * sampleSize, [0.1] * sampleSize)
>>> y = normal.getRealization()
>>> yAt = design.transpose() * y
>>> x = method.solve(y)

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``SparseMethod-ErrorThreshold`` (``Scalar``, default: ``0.001``)
- ``SparseMethod-MaximumErrorFactor`` (``Scalar``, default: ``2``)"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseMethod::computeDesign
OT_LinearLeastSquaresMethod_computeDesign_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseMethod::solveNormalGram
OT_LinearLeastSquaresMethod_solveNormalGram_doc
