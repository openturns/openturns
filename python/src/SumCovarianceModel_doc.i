%feature("docstring") OT::SumCovarianceModel
R"RAW(Sum of covariance models on the same input and output spaces.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    SumCovarianceModel(*inputDimension=1*)

    SumCovarianceModel(*collection*)

Parameters
----------
inputDimension : int, optional
    Input dimension :math:`n` shared by all the summed models.
    A default collection of two absolute exponential models is built.
collection : sequence of :class:`~openturns.CovarianceModel`
    Collection :math:`(C_k)_{1 \leq k \leq K}` of models sharing the same
    input dimension :math:`n` and output dimension :math:`p`.

See Also
--------
:class:`~openturns.CovarianceModel`,
:class:`~openturns.experimental.ScaledCovarianceModel`,
:class:`~openturns.DiracCovarianceModel`,
:class:`~openturns.ProductCovarianceModel`

Notes
-----
The sum writes, for all :math:`(\vect{s}, \vect{t})`,

.. math::

    C(\vect{s}, \vect{t}) = \sum_{k=1}^K C_k(\vect{s}, \vect{t})

There is no unique scale, amplitude, output correlation or nugget factor
at the sum level: every hyperparameter lives in the members and the full
parameter is the concatenation of the member full parameters. The generic
accessors (`getScale`, `setScale`, `getAmplitude`, `setAmplitude`,
`getOutputCorrelation`, `setOutputCorrelation`, `getNuggetFactor`,
`setNuggetFactor`) raise an exception on a sum; use `getCollection` or
the full parameter accessors instead.

The ``__add__`` operator flattens nested
sums and folds identical atoms into ``ScaledCovarianceModel``
models, as :class:`~openturns.LinearCombinationFunction` flattens nested
linear combinations and sums the weights of identical atoms.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> import openturns.testing as ott
>>> m1 = ot.ExponentialModel([1.0], [2.0])
>>> m2 = ot.ExponentialModel([3.0], [4.0])
>>> m = m1 + m2
>>> s = [0.0]
>>> t = [1.0]
>>> ott.assert_almost_equal(m.computeAsScalar(s, t), m1.computeAsScalar(s, t) + m2.computeAsScalar(s, t))
>>> d = ot.DiracCovarianceModel(1)
>>> d.setAmplitude([0.5])
>>> d.setNuggetFactor(0.0)
>>> total = m + d
>>> ott.assert_almost_equal(total.computeAsScalar(s, s), m.computeAsScalar(s, s) + 0.25)
)RAW"
