%feature("docstring") OT::SlicedInverseRegression
R"RAW(Sliced Inverse Regression algorithm.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Sliced Inverse Regression (SIR, [li1991]_) is a dimension reduction technique used
to find the effective dimension reduction (EDR) space.
It assumes the following regression model:

.. math::

    Y = f(\Tr{\vect{\beta}} \vect{X}, \varepsilon)

where the response variable :math:`Y` is univariate,
the regressor :math:`\vect{X}` is :math:`\inputDim`-dimensional
the vector :math:`\vect{\beta}` represents the EDR directions and
:math:`\varepsilon` is independent of :math:`X`.

The algorithm proceeds as follows:

1. Sort the output :math:`Y` and divide it into :math:`H` slices of equal size
2. Center the input sample: :math:`\overline{\vect{X}} = \frac{1}{\sampleSize}\sum_{i=1}^\sampleSize \vect{X}_i`
3. Compute the QR decomposition of the centered input matrix:
   :math:`\vect{X} = \mat{Q} \mat{R}`
4. Standardize :math:`\mat{Z} = \sqrt{\sampleSize} \mat{Q}`
5. For each slice :math:`h`, compute the weighted mean:
   :math:`\overline{\mat{Z}}_h = \sqrt{\sampleSize_h} \cdot \text{mean}(\mat{Z}^{(h)})`
6. Compute the singular value decomposition of the slice-mean matrix:
   :math:`\overline{\mat{Z}} = \mat{U} \mat{\Sigma} \Tr{\mat{V}}`
7. The right singular vectors :math:`\vect{V}_k` (columns of :math:`\mat{V}`) are the
   eigenvectors of :math:`\Tr{\overline{\mat{Z}}} \overline{\mat{Z}}` with eigenvalues
   :math:`\lambda_k = \sigma_k^2 / \sampleSize`
8. Back-transform the eigenvectors to the original space:
   :math:`\vect{\alpha}_k = \sqrt{\sampleSize} \mat{R}^{-1} \vect{V}_k`
9. Keep only the first :math:`k` directions according to :math:`\texttt{nbModes}`
10. Normalize the directions: :math:`\hat{\vect{\alpha}}_k = \vect{\alpha}_k / \|\vect{\alpha}_k\|`

The transformation maps an input vector :math:`\vect{x}` to its coordinates in the EDR basis:

.. math::

    f(\vect{x}) = \Tr{\hat{\vect{\alpha}}} (\vect{x} - \overline{\vect{X}})

Parameters
----------
X : :class:`~openturns.Sample`
    Input sample
Y : :class:`~openturns.Sample`
    Output, supervision sample
sliceNumber : int, optional
    Number of slices, defaults to 10

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> N = 100
>>> X = ot.Normal([0.0] * 2, [0.1] * 2).getSample(N)
>>> X += [[i / (N - 1), 2 * i / (N - 1)] for i in range(N)]
>>> # X is centered internally by the SIR algorithm
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
