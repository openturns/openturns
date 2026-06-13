%feature("docstring") OT::SlicedInverseRegressionResult
R"RAW(Result class for Sliced Inverse Regression.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Stores the EDR directions, eigenvalues and the center of the input
sample computed by the
:class:`~openturns.experimental.SlicedInverseRegression` algorithm.

The transformation maps an input vector :math:`x \in \mathbb{R}^d` to
:math:`\mathbb{R}^k` via:

.. math::

    f(x) = \hat{\alpha}^T (x - \overline{X})

where :math:`\hat{\alpha}` is the :math:`d \times k` matrix of
normalized EDR directions and :math:`\overline{X}` is the input sample
mean.

The inverse transformation maps back from the reduced space:

.. math::

    f^{-1}(z) = \overline{X} + (\hat{\alpha}^T)^+ z

where :math:`(\hat{\alpha}^T)^+` denotes the Moore-Penrose pseudo-inverse
of :math:`\hat{\alpha}^T`.

References
----------
Li, K. C. (1991). Sliced inverse regression for dimension reduction.
*Journal of the American Statistical Association*, 86(414), 316-327.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SlicedInverseRegressionResult::getTransformation
"Transformation function accessor.

Returns
-------
transformation : :class:`~openturns.Function`
    Transformation function
"

// ---------------------------------------------------------------------

%feature("docstring") OT::SlicedInverseRegressionResult::getInverseTransformation
"Inverse transformation function accessor.

Returns
-------
inverseTransformation : :class:`~openturns.Function`
    Inverse transformation function
"

// ---------------------------------------------------------------------

%feature("docstring") OT::SlicedInverseRegressionResult::getDirections
"Directions accessor.

Returns
-------
directions : :class:`~openturns.Matrix`
    Directions matrix
"

// ---------------------------------------------------------------------

%feature("docstring") OT::SlicedInverseRegressionResult::getEigenvalues
"Eigen values accessor.

Returns
-------
eigenvalues : :class:`~openturns.Point`
    Eigen values
"
