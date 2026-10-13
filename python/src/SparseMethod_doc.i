%feature("docstring") OT::SparseMethod
R"RAW(Least squares solver using a sparse representation.

This class enables to solve a least squares problem: refer to :any:`sparse_least_squares` to get all the
notations and to :any:`least_squares_resolution` to get details on the numerical method.
It provides a way to access rapidly to the coefficients of the linear combination of the basis functions.

Available constructors:
    SparseMethod(*method*)

    SparseMethod(*method, basisSequenceFactory, fittingAlgorithm*)

Parameters
----------
method : :class:`~openturns.LeastSquaresMethod`
    Least squares resolution method
basisSequenceFactory : :class:`~openturns.BasisSequenceFactory`
    Basis enumeration algorithm
fittingAlgorithm : :class:`~openturns.FittingAlgorithm`
    Validation algorithm

See also
--------
LeastSquaresMethod

Examples
--------
We get the projection onto the functional basis :math:`(x, x^2, x^3)` of
a model :math:`f: \Rset \rightarrow \Rset` from which we only have a data base of
size 5. The least squares problem is solved using the QR method.
We note that the solution is of dimension 2: the projection uses the
functions :math:`(x, x^2)`.

First, we do not specify the last two
arguments: no model selection strategy is used.

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

Now, we use a selection strategy with :class:`~openturns.LARS` and class:`~openturns.CorrectedLeaveOneOut`.
We note that the solution is of dimension 1: the projection uses the
function :math:`x`.

>>> fitAlgo = ot.CorrectedLeaveOneOut()
>>> basisFact = ot.LARS()
>>> method = ot.SparseMethod(ot.QRMethod(proxy, full), basisFact, fitAlgo)
>>> x = method.solve(y)

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``SparseMethod-ErrorThreshold`` (``Scalar``, default: ``0.001``)
- ``SparseMethod-MaximumErrorFactor`` (``Scalar``, default: ``2``)

)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::SparseMethod::computeDesign
OT_LeastSquaresMethod_computeDesign_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseMethod::solveNormalGram
OT_LeastSquaresMethod_solveNormalGram_doc
