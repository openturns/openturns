%feature("docstring") OT::SparseGaussianProcessHessian
R"RAW(Sparse Gaussian process hessian.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class computes the hessian of the mean of the sparse Gaussian process prediction with
respect to the input point :math:`\vect{x}`:

.. math::

    \frac{\partial^2 \mu}{\partial x_j \partial x_k}(\vect{x}) = \sum_{m=1}^{M}
    \alpha_m \frac{\partial^2 k(\vect{z}_m, \vect{x})}{\partial x_j \partial x_k}

where :math:`\vect{\alpha} = \mat{L}_{uu}^{-t} \vect{m}_w`.

See also
--------
openturns.experimental.SparseGaussianProcessRegression, openturns.experimental.SparseGaussianProcessEvaluation, openturns.experimental.SparseGaussianProcessGradient
)RAW"
