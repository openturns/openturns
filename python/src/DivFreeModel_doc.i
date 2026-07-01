%feature("docstring") OT::DivFreeModel
R"RAW(Divergence-free covariance model.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class builds a matrix-valued divergence-free covariance model from a scalar
covariance model :math:`C`, as described in [scheuerer2012]_.

Let :math:`C: \Rset^d \times \Rset^d \rightarrow \Rset` be a scalar covariance
model, and let :math:`\mat{H}(\vect{s}, \vect{t})` be the matrix of mixed
second derivatives:

.. math::

    H_{ij}(\vect{s}, \vect{t}) = \frac{\partial^2 C(\vect{s}, \vect{t})}{\partial s_i \partial t_j}

The divergence-free covariance model is defined as:

.. math::

    \mat{C}_{\mathrm{div}}(\vect{s}, \vect{t})
        = \mathrm{tr}(\mat{H}(\vect{s}, \vect{t})) \, \mat{I}_d
        - \mat{H}(\vect{s}, \vect{t})

The resulting model has input and output dimension :math:`d`. If the
underlying scalar model has :math:`\mathcal{C}^{1,1}` regularity, the
associated random vector field has divergence-free sample paths almost surely.

The model is defined for input dimensions :math:`d \ge 2`. For
:math:`d > 3` the model has been conjectured to work but only proven
for :math:`d = 2, 3` [scheuerer2012]_.

Parameters
----------
model : :class:`~openturns.CovarianceModel`
    A scalar covariance model :math:`C: \Rset^d \times \Rset^d \rightarrow \Rset`.

See Also
--------
openturns.CovarianceModel, openturns.experimental.CurlFreeModel

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> scale = [1.0, 1.5]
>>> amplitude = [1.0]
>>> cov = ot.SquaredExponential(scale, amplitude)
>>> divFreeCov = otexp.DivFreeModel(cov)
>>> print(divFreeCov.getInputDimension())
2
>>> print(divFreeCov.getOutputDimension())
2
>>> s = [0.5, 0.3]
>>> t = [0.1, 0.2]
>>> print(divFreeCov(s, t))
[[ 0.407544  0.0163745 ]
 [ 0.0163745 0.773696  ]]
)RAW"

%feature("docstring") OT::DivFreeModel::getCovarianceModel
"Accessor to the underlying scalar covariance model.

Returns
-------
model : :class:`~openturns.CovarianceModel`
    The underlying scalar covariance model."
