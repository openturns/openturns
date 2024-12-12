%feature("docstring") OT::SlicedInverseRegression
R"RAW(Sliced Inverse Regression algorithm.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Parameters
----------
X : :class:`~openturns.Sample`
    Input sample
Y : :class:`~openturns.Sample`
    Output, supervision sample
sliceNumber : int, optional
    Number of slices, defaults to 10

Notes
-----
Sliced Inverse Regression (SIR) is a dimension reduction technique used
to find the effective dimension reduction (EDR) space. It assumes the
following regression model:

.. math::

    Y = f(\beta_1^T X, \ldots, \beta_k^T X, \varepsilon)

where :math:`\beta_1, \ldots, \beta_k` are the EDR directions and
:math:`\varepsilon` is independent of :math:`X`.

The algorithm proceeds as follows:

1. Center the input sample: :math:`\overline{X} = \frac{1}{n}\sum_{i=1}^n X_i`
2. Compute the QR decomposition of the centered input matrix:
   :math:`X = QR`
3. Standardize :math:`Z = \sqrt{n} Q`
4. Sort the output :math:`Y` and divide it into :math:`H` slices of equal size
5. For each slice :math:`h`, compute the weighted mean:
   :math:`\overline{Z}_h = \sqrt{n_h} \cdot \text{mean}(Z^{(h)})`
6. Compute the singular value decomposition of the slice-mean matrix:
   :math:`\overline{Z} = U \Sigma V^T`
7. The right singular vectors :math:`V_k` (columns of :math:`V`) are the
   eigenvectors of :math:`\overline{Z}^T \overline{Z}` with eigenvalues
   :math:`\lambda_k = \sigma_k^2 / n`
8. Back-transform the eigenvectors to the original space:
   :math:`\alpha_k = R^{-1} V_k`
9. Normalize the directions: :math:`\hat{\alpha}_k = \alpha_k / \|\alpha_k\|`

The transformation maps an input vector :math:`x` to its coordinates in
the EDR basis:

.. math::

    f(x) = \hat{\alpha}^T (x - \overline{X})

References
----------
Li, K. C. (1991). Sliced inverse regression for dimension reduction.
*Journal of the American Statistical Association*, 86(414), 316-327.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> N = 100
>>> X = ot.Normal([0.0] * 2, [0.1] * 2).getSample(N)
>>> X += [[-i / (N - 1), 2 * i / (N - 1)] for i in range(N)]
>>> X = X - X.computeMean()
>>> f = ot.SymbolicFunction(['x1', 'x2'], ['4*(x1+2*x2)+2'])
>>> Y = f(X) + ot.Normal(0.0, 0.2).getSample(N)
>>> algo = otexp.SlicedInverseRegression(X, Y)
>>> algo.run()
>>> transformation = algo.getResult().getTransformation()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SlicedInverseRegression::run
"Run the algorithm."

// ---------------------------------------------------------------------

%feature("docstring") OT::SlicedInverseRegression::getResult
"Accessor to the result.

Returns
-------
result : :class:`~openturns.experimental.SlicedInverseRegressionResult`
    Result class
"

// ---------------------------------------------------------------------

%feature("docstring") OT::SlicedInverseRegression::setSliceNumber
"Accessor to the number of slices.

Parameters
----------
sliceNumber : int
    Number of slices
"

// ---------------------------------------------------------------------

%feature("docstring") OT::SlicedInverseRegression::getSliceNumber
"Accessor to the number of slices.

Returns
-------
sliceNumber : int
    Number of slices
"

// ---------------------------------------------------------------------

%feature("docstring") OT::SlicedInverseRegression::setNbModes
"Accessor to the number of EDR directions.

Parameters
----------
nbModes : int
    Number of EDR directions retained.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::SlicedInverseRegression::getNbModes
"Accessor to the number of EDR directions.

Returns
-------
nbModes : int
    Number of EDR directions retained.
"
