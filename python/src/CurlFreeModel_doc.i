%feature("docstring") OT::CurlFreeModel
R"RAW(Curl-free covariance model.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class builds a matrix-valued curl-free covariance model from a scalar
covariance model :math:`C`, as described in [scheuerer2012]_.

Let :math:`C: \Rset^d \times \Rset^d \rightarrow \Rset` be a scalar covariance
model. The curl-free covariance model is defined as:

.. math::

    \mat{C}_{\mathrm{curl}}(\vect{s}, \vect{t})_{ij}
        = \frac{\partial^2 C(\vect{s}, \vect{t})}{\partial s_i \partial t_j},
        \qquad \forall i,j \in \{1, \dots, d\}

For a stationary model :math:`C(\vect{s}, \vect{t}) = \phi(\vect{s} - \vect{t})`,
this becomes:

.. math::

    \mat{C}_{\mathrm{curl}}(\vect{\tau})_{ij}
        = - \frac{\partial^2 \phi(\vect{\tau})}{\partial \tau_i \partial \tau_j}

The resulting model has input and output dimension :math:`d`. If the
underlying scalar model has :math:`\mathcal{C}^{1,1}` regularity, the
associated random vector field has curl-free sample paths almost surely.

The model is defined for input dimensions :math:`d \ge 2`. For
:math:`d > 3` the model has been conjectured to work but only proven
for :math:`d = 2, 3` [scheuerer2012]_.

Parameters
----------
model : :class:`~openturns.CovarianceModel`
    A scalar covariance model :math:`C: \Rset^d \times \Rset^d \rightarrow \Rset`.

See Also
--------
openturns.CovarianceModel, openturns.experimental.DivFreeModel

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> scale = [1.0, 1.5]
>>> amplitude = [1.0]
>>> cov = ot.SquaredExponential(scale, amplitude)
>>> curlFreeCov = otexp.CurlFreeModel(cov)
>>> print(curlFreeCov.getInputDimension())
2
>>> print(curlFreeCov.getOutputDimension())
2
>>> s = [0.5, 0.3]
>>> t = [0.1, 0.2]
>>> print(curlFreeCov(s, t))
[[  0.773696  -0.0163745 ]
 [ -0.0163745  0.407544  ]]
)RAW"

%feature("docstring") OT::CurlFreeModel::getCovarianceModel
"Accessor to the underlying scalar covariance model.

Returns
-------
model : :class:`~openturns.CovarianceModel`
    The underlying scalar covariance model."
