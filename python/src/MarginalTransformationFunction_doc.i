%feature("docstring") OT::MarginalTransformationFunction
R"RAW(Marginal transformation function.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Parameters
----------
inputDistributionCollection : sequence of :class:`~openturns.Distribution`
    Collection of input marginal distributions.
outputDistributionCollection : sequence of :class:`~openturns.Distribution`
    Collection of output marginal distributions.

Notes
-----
The class creates a :class:`~openturns.Function` that maps input points to output
points using a marginal transformation: :math:`G^{-1} \circ F` where :math:`F`
is the CDF of the input distribution and :math:`G^{-1}` is the quantile function
of the output distribution.

The transformation is applied componentwise:

.. math::

    y_i = G_i^{-1}(F_i(x_i))

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> inputColl = [ot.Normal(0.0, 1.0), ot.Normal(0.0, 1.0)]
>>> outputColl = [ot.Uniform(0.0, 1.0), ot.Uniform(0.0, 1.0)]
>>> f = otexp.MarginalTransformationFunction(inputColl, outputColl)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MarginalTransformationFunction::getInputDistributionCollection
R"RAW(Accessor to the input distribution collection.

Returns
-------
inputDistributionCollection : :class:`~openturns.DistributionCollection`
    The input marginal distributions.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MarginalTransformationFunction::getOutputDistributionCollection
R"RAW(Accessor to the output distribution collection.

Returns
-------
outputDistributionCollection : :class:`~openturns.DistributionCollection`
    The output marginal distributions.
)RAW"
