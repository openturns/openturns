%feature("docstring") OT::IntegrationStrategy
R"RAW(Integration strategy for the approximation coefficients.

Available constructors:
    IntegrationStrategy(*measure*)

    IntegrationStrategy(*weightedExp*)

    IntegrationStrategy(*measure, weightedExp*)

    IntegrationStrategy(*inputSample, outputSample*)

    IntegrationStrategy(*inputSample, weights, outputSample*)

Parameters
----------
weightedExp : :class:`~openturns.WeightedExperiment`
    Experimental design used for the transformed input data. 
    
    By default the class :class:`~openturns.MonteCarloExperiment` is used.
measure : :class:`~openturns.Distribution`
    The input distribution :math:`\mu_{\inputRV}`
    with respect to which the basis is orthonormal.
    
    By default, the limit measure defined within the class
    :class:`~openturns.WeightedExperiment` is used.
inputSample : 2-d sequence of float
    The input sample of size :math:`\sampleSize`.
outputSample : 2-d sequence of float
    The output sample of size :math:`\sampleSize`.
weights : sequence of float
    Numerical point that are the weights associated to the input sample points
    such that the corresponding weighted experiment is a good approximation of
    :math:`\mu_{\inputRV}`.

    By default, all weights are equal to
    :math:`\omega_i = \frac{1}{\sampleSize}`.

See also
--------
FunctionalChaosAlgorithm, ProjectionStrategy, LeastSquaresStrategy

Notes
-----
This class is used in the :any:`functional chaos expansion context <functional_chaos>` implemented
in the class :class:`~openturns.FunctionalChaosAlgorithm`. It is not usable outside this context.

The model is approximated by the meta model defined in :any:`functional_chaos` by equation :eq:`metaModelPn`
and the coefficients :math:`a_k)_{k \in I_n}` are computed with the inner product defined in :any:`functional_chaos`
by equation :eq:`scalProd`. This computation makes sense if and only if the basis is orthonormal with respect to the distribution :math:`\mu_{\inputRV}`.
)RAW"
