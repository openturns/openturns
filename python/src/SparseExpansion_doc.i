%feature("docstring") OT::SparseExpansion
R"RAW(L2 approximation on an orthonormal basis using least-squares and sparse model selection.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class builds a sparse polynomial chaos expansion by iteratively
selecting basis functions using either Orthogonal Matching Pursuit (OMP)
or Least Angle Regression (LARS), controlled by the `selectionMethod`
parameter (default: ``"OMP"``).

**OMP algorithm:**

At each iteration, the basis function having the largest absolute correlation
with the current residual is added to the active set. The coefficients are
then updated by solving the least-squares problem restricted to the active
set.

For each output marginal :math:`j`, the algorithm starts with a constant
approximation:

.. math::
    \widehat{y}_j = \beta_0

and the initial residual:

.. math::
    r = y_j - \beta_0

At each iteration :math:`i`:

1. Find :math:`k_i = \arg\max_k |\langle \varphi_k, r \rangle|`
2. Add :math:`\varphi_{k_i}` to the active set
3. Solve :math:`\beta = \arg\min \| y_j - \Phi_{\mathcal{A}} \beta \|^2`
4. Update the residual: :math:`r = y_j - \Phi_{\mathcal{A}} \beta`
5. Compute the cross-validation error using the :class:`~openturns.FittingAlgorithm`

**LARS algorithm:**

At each iteration, the algorithm computes an equiangular direction in the
span of the active basis functions and steps along it until another
inactive function's correlation catches up. The coefficients of active
functions change continuously along the path, unlike OMP which recomputes
them from scratch at each step.

1. Find :math:`k_i = \arg\max_k |\langle \varphi_k, r \rangle|`
2. Compute the equiangular direction :math:`u` in :math:`\operatorname{span}(\Phi_{\mathcal{A}})`
3. Determine the step size :math:`\gamma` from the catch-up condition
4. Update the prediction: :math:`\mu \mathrel{+}= \gamma u`
5. Compute the cross-validation error using the :class:`~openturns.FittingAlgorithm`

**Stopping criteria:**

Both algorithms stop when one of the following conditions is met:

- The cross-validation error has increased for :math:`k` consecutive
  iterations, where :math:`k` is given by the
  `SparseExpansion-ConsecutiveIncreases` key in
  :class:`~openturns.ResourceMap` (default: 2)
- The cross-validation error degrades by more than a factor
  :math:`\alpha` compared to the best error so far
- The cross-validation error exceeds an absolute maximum
- The best cross-validation error falls below a threshold

Available constructors:
    SparseExpansion(*inputSample, outputSample, distribution*)

    SparseExpansion(*inputSample, weights, outputSample, distribution*)

    SparseExpansion(*inputSample, outputSample, distribution, basis, basisSize, methodName*)

    SparseExpansion(*inputSample, weights, outputSample, distribution, basis, basisSize, methodName*)

Parameters
----------
inputSample : 2-d sequence of float of size :math:`n>0`
    Input sample.
weight : sequence of float of size :math:`n>0`, optional
    Input weights. By default, a uniform weight equals to :math:`1/n` is used.
outputSample : 2-d sequence of floats of size :math:`n>0`
    Output sample.
distribution : :class:`~openturns.Distribution`
    The distribution associated to the weighted sample *(inputSample, weights)*
basis : :class:`~openturns.OrthogonalBasis`, optional
    The orthonormal basis of the vector space on which the approximation is built.
    By default, a basis is built using an hyperbolic enumerate function
    :class:`~openturns.HyperbolicAnisotropicEnumerateFunction` associated to the
    :math:`q-` norm specified by the `FunctionalChaosAlgorithm-QNorm` key in
    :class:`~openturns.ResourceMap`.
    The basis is obtained through a tensorization of the univariate orthonormal
    polynomial bases associated to each marginal distributions of *distribution*.
basisSize : int, optional
    The size of the basis on which the approximation is built. If not given, it is
    given by the `FunctionalChaosAlgorithm-BasisSize` key in
    :class:`~openturns.ResourceMap` if this value is positive, or as the cardinal
    of the set of basis functions up to a degree given by the
    `FunctionalChaosAlgorithm-MaximumTotalDegree` key in
    :class:`~openturns.ResourceMap`.
methodName : str, optional
    The decomposition method used to solve the least-squares problem, of
    type :class:`~openturns.LeastSquaresMethod`. If not given, it is given by the
    `SparseExpansion-DecompositionMethod` key in
    :class:`~openturns.ResourceMap`.

See also
--------
openturns.LeastSquaresExpansion, openturns.IntegrationExpansion, openturns.FunctionalChaosAlgorithm, openturns.LeastSquaresMethod

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used by this class:

- *SparseExpansion-DecompositionMethod*: the default decomposition method
  used to solve the least-squares problem (default: SVD).
- *SparseExpansion-ConsecutiveIncreases*: the number of consecutive
  increases in cross-validation error required before stopping the basis
  selection (default: 2). Must be at least 1.

Examples
--------
Solves a functional linear least squares approximation using residual minimization with QR method:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> g = ot.SymbolicFunction('x', 'sin(x)')
>>> distribution = ot.Uniform(-2.0, 2.0)
>>> inSample = distribution.getSample(10)
>>> outSample = g(inSample)
>>> basis = ot.OrthogonalProductPolynomialFactory([ot.LegendreFactory()])
>>> basisSize = 5
>>> algo = otexp.SparseExpansion(inSample, outSample, distribution, basis, basisSize, 'QR')
>>> algo.run()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseExpansion::getActiveFunctions
R"RAW(Accessor to the active functions in the basis.

Returns
-------
activeFunctions : sequence of `int`
    The indices of the functions used for the approximation in the basis.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseExpansion::setActiveFunctions
R"RAW(Accessor to the active functions in the basis.

Parameters
----------
activeFunctions : sequence of `int`
    The indices of the functions used for the approximation in the basis.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseExpansion::getSelectionMethod
R"RAW(Accessor to the selection method.

Returns
-------
method : str
    The selection method used for basis function selection. Either ``"OMP"``
    for orthogonal matching pursuit or ``"LARS"`` for least angle regression.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseExpansion::setSelectionMethod
R"RAW(Accessor to the selection method.

Parameters
----------
method : str
    The selection method used for basis function selection. Either ``"OMP"``
    for orthogonal matching pursuit or ``"LARS"`` for least angle regression.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseExpansion::getFittingAlgorithm
R"RAW(Accessor to the fitting algorithm.

Returns
-------
fittingAlgorithm : :class:`~openturns.FittingAlgorithm`
    Fitting algorithm used to estimate the cross-validation error.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseExpansion::setFittingAlgorithm
R"RAW(Accessor to the fitting algorithm.

Parameters
----------
fittingAlgorithm : :class:`~openturns.FittingAlgorithm`
    Fitting algorithm used to estimate the cross-validation error.)RAW"
