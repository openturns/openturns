%feature("docstring") OT::SparseGaussianProcessGradient
R"RAW(Sparse Gaussian process gradient.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class computes the gradient of the mean of the sparse Gaussian process prediction with
respect to the input point :math:`\vect{x}`:

.. math::

    \frac{\partial \mu}{\partial x_j}(\vect{x}) = \sum_{m=1}^{M}
    \alpha_m \frac{\partial k(\vect{z}_m, \vect{x})}{\partial x_j}

where :math:`\vect{\alpha} = \mat{L}_{uu}^{-t} \vect{m}_w`.

See also
--------
openturns.experimental.SparseGaussianProcessRegression, openturns.experimental.SparseGaussianProcessEvaluation
)RAW"
