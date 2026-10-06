%feature("docstring") OT::ScaledCovarianceModel
R"RAW(Covariance model scaled by a positive factor.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    ScaledCovarianceModel()

    ScaledCovarianceModel(*kernel, factor*)

Parameters
----------
kernel : :class:`~openturns.CovarianceModel`
    Inner covariance model :math:`C`.
factor : positive float
    Scaling factor :math:`\alpha > 0`, accessed through
    `getScaleFactor` and `setScaleFactor` to avoid any confusion with the
    input scale vector.

See Also
--------
:class:`~openturns.CovarianceModel`,
:class:`~openturns.experimental.SumCovarianceModel`,
:class:`~openturns.DiracCovarianceModel`

Notes
-----
The scaled model writes, for all :math:`(\vect{s}, \vect{t})`,

.. math::

    C_{\text{scaled}}(\vect{s}, \vect{t}) = \alpha \, C(\vect{s}, \vect{t})

Scale, amplitude, output correlation and nugget factor are delegated to
the inner model; the factor is appended to the full parameter. The inner
model is kept as-is, so the factor times the inner amplitude is a ridge
at fitting time: freeze one of them through the active parameters.

The ``__mul__`` and ``__rmul__`` operators map a factor
of 1.0 to the model itself and wrap any other positive factor.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> import openturns.testing as ott
>>> m = ot.ExponentialModel([1.0], [2.0])
>>> scaled = m * 3.0
>>> s = [0.0]
>>> t = [1.0]
>>> ott.assert_almost_equal(scaled.computeAsScalar(s, t), 3.0 * m.computeAsScalar(s, t))
)RAW"
