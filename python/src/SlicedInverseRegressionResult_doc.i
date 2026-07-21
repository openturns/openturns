%feature("docstring") OT::SlicedInverseRegressionResult
R"RAW(Result class for Sliced Inverse Regression.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Stores the EDR directions, eigenvalues and the center of the input
sample computed by the
:class:`~openturns.experimental.SlicedInverseRegression` algorithm.

The transformation maps an input vector :math:`\vect{x} \in \Rset^\inputDim` to
:math:`\Rset^k` via:

.. math::

    f(\vect{x}) = \Tr{\hat{\vect{\alpha}}} (\vect{x} - \overline{\vect{X}})

where :math:`\hat{\vect{\alpha}}` is the :math:`\inputDim \times k` matrix of
normalized EDR directions and :math:`\overline{\vect{X}}` is the input sample
mean.

The inverse transformation maps back from the reduced space:

.. math::

    f^{-1}(\vect{z}) = \overline{\vect{X}} + (\Tr{\hat{\vect{\alpha}}})^{+} \vect{z}

where :math:`(\Tr{\hat{\vect{\alpha}}})^{+}` denotes the Moore-Penrose pseudo-inverse
of :math:`\Tr{\hat{\vect{\alpha}}}`.

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
