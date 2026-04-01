%feature("docstring") OT::LeastSquaresStrategy
R"RAW(Least squares strategy for the approximation coefficients.

Available constructors:
    LeastSquaresStrategy(*weightedExp*)

    LeastSquaresStrategy(*weightedExp, approxAlgoImpFact*)

    LeastSquaresStrategy(*approxAlgoImpFact*)

    LeastSquaresStrategy(*measure, approxAlgoImpFact*)

    LeastSquaresStrategy(*measure, weightedExp, approxAlgoImpFact*)

    LeastSquaresStrategy(*inputSample, outputSample, approxAlgoImpFact*)

    LeastSquaresStrategy(*inputSample, weights, outputSample, approxAlgoImpFact*)

Parameters
----------
weightedExp : :class:`~openturns.WeightedExperiment`
    Experimental design used for the transformed input data.
    
    By default, the class :class:`~openturns.MonteCarloExperiment` is used.
approxAlgoImpFact : :class:`~openturns.ApproximationAlgorithmImplementationFactory`
    The factory that builds the desired :class:`~openturns.ApproximationAlgorithm`.
    
    By default, the class :class:`~openturns.PenalizedLeastSquaresAlgorithmFactory` is used.
measure : :class:`~openturns.Distribution`
    The input distribution :math:`\mu_{\inputRV}`.
    
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
FunctionalChaosAlgorithm, ProjectionStrategy, IntegrationStrategy

Notes
-----
This class is used in the :ref:`functional chaos expansion context <functional_chaos>` implemented
in the class :class:`~openturns.FunctionalChaosAlgorithm`. It is not usable outside this context.

The model is approximated by the meta model defined in :ref:`functional_chaos` by equation :eq:`metaModelPn`
and the coefficients :math:`(a_k)_{k \in I_n}` are computed by solving the least squares problem defined in :ref:`functional_chaos` by equation :eq:`metaModeleF`.

Refer to :ref:`least_squares` for more details on the resolution of least-squares problem.
)RAW"
