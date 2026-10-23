%feature("docstring") OT::SobolIndicesExperiment
R"RAW(Experiment to compute Sobol' indices.

Available constructors:
    SobolIndicesExperiment(*distribution, size, computeSecondOrder*)

    SobolIndicesExperiment(*experiment, computeSecondOrder*)

Parameters
----------
distribution : :class:`~openturns.Distribution`
    Distribution :math:`\mu` with an independent copula used to generate the
    set of input data.
size : positive int
    Size :math:`N` of each of the two independent initial samples.
    For the total size of the experiment see notes below.
experiment : :class:`~openturns.WeightedExperiment`
    Design of experiment used to sample the distribution.
computeSecondOrder : bool, defaults to False
    Whether to add points to compute second order indices

Notes
-----
Sensitivity algorithms rely on the definition of specific designs.

The :meth:`generate` method of this class produces a :class:`~openturns.Sample`
to be supplied to the constructor of one of the :class:`~openturns.SobolIndicesAlgorithm` implementations:

- :class:`~openturns.SaltelliSensitivityAlgorithm`,
- :class:`~openturns.MartinezSensitivityAlgorithm`,
- :class:`~openturns.MauntzKucherenkoSensitivityAlgorithm`,
- :class:`~openturns.JansenSensitivityAlgorithm`.

The chosen :class:`~openturns.SobolIndicesAlgorithm` implementation then uses the sample as *input design*,
which means it represents (but is not a realization of) a random vector :math:`\vect{X} = \left( X_1, \ldots, X_{\inputDim} \right)`.

Either the :class:`~openturns.Distribution` of :math:`\vect{X}`
or a :class:`~openturns.WeightedExperiment` that represents it
must be supplied to the class constructor.

If a :class:`~openturns.WeightedExperiment` is supplied, the class uses it directly.

If the distribution of :math:`\vect{X}` is supplied, the class generates a :class:`~openturns.WeightedExperiment`.
To do this, it duplicates the distribution: every marginal is repeated once to produce
a :math:`2 \inputDim`-dimensional distribution.
This trick makes it possible to choose a :class:`~openturns.WeightedExperiment` with non-iid samples
(that is a :class:`~openturns.LHSExperiment` or a :class:`~openturns.LowDiscrepancyExperiment`)
to represent the original :math:`\inputDim`-dimensional distribution.

The type of :class:`~openturns.WeightedExperiment` depends on the value of ``'SobolIndicesExperiment-SamplingMethod'`` in the :class:`~openturns.ResourceMap`:

    - ``'MonteCarlo'`` for a :class:`~openturns.MonteCarloExperiment`.
    - ``'LHS'`` for an :class:`~openturns.LHSExperiment` with *alwaysShuffle* and *randomShift* set to *True*.
    - ``'QMC'`` for a :class:`~openturns.LowDiscrepancyExperiment` (with *randomize* flag set to *False*) built from a :class:`~openturns.SobolSequence`.

``'MonteCarlo'`` is the default choice because it allows the chosen :class:`~openturns.SobolIndicesAlgorithm` implementation to use the asymptotic distribution of the estimators of the Sobol' indices.

Note that ``'QMC'`` is only possible if :math:`2 \inputDim \leqslant` :class:`~openturns.SobolSequence`:code:`.MaximumDimension`.
If ``'QMC'`` is specified but :math:`2 \inputDim >` :class:`~openturns.SobolSequence`:code:`.MaximumDimension`, the class falls back to ``'LHS'``.

>>> import openturns as ot
>>> print(ot.SobolSequence.MaximumDimension)
1111

Regardless of the type of :class:`~openturns.WeightedExperiment`,
the class splits it into two samples with the same size :math:`N`:
:math:`\mat{A}` and :math:`\mat{B}`.
Their columns are mixed in order to produce a very large sample: the *inputDesign* argument
taken by one of the constructors of every :class:`~openturns.SobolIndicesAlgorithm` implementation.

If *computeSecondOrder* is set to *False*, the input design is of size :math:`N(2+\inputDim)`.
The first :math:`N` rows contain the sample :math:`\mat{A}` and the next :math:`N` rows the sample :math:`\mat{B}`.
The last :math:`N \inputDim` rows contain :math:`\inputDim` copies of :math:`\mat{A}`,
each with a different column replaced by the corresponding column from :math:`\mat{B}`
(they are the matrices :math:`\mat{E}^i` from the documentation page of :class:`~openturns.SobolIndicesAlgorithm`).

If *computeSecondOrder* is set to *True* and :math:`\inputDim = 2`, the input design is the same as in
the case where *computeSecondOrder* is *False* (see [saltelli2002]_).

If *computeSecondOrder* is set to *True* and :math:`\inputDim \neq 2`, the input design size is :math:`N(2+2 \inputDim)`.
The first :math:`N(2+\inputDim)` rows are the same as when *computeSecondOrder* is *False*.
The last :math:`N \inputDim` rows contain :math:`\inputDim` copies of :math:`\mat{B}`,
each with a different column replaced by the corresponding column from :math:`\mat{A}`
(they are the matrices :math:`\mat{C}^i` from the documentation page of :class:`~openturns.SobolIndicesAlgorithm`).

See also
--------
SobolIndicesAlgorithm

Examples
--------

Create a sample suitable to estimate first and total order Sobol' indices:

>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> formula = ['sin(pi_*X1)+7*sin(pi_*X2)^2+0.1*(pi_*X3)^4*sin(pi_*X1)']
>>> model = ot.SymbolicFunction(['X1', 'X2', 'X3'], formula)
>>> distribution = ot.JointDistribution([ot.Uniform(-1.0, 1.0)] * 3)
>>> size = 10
>>> experiment = ot.SobolIndicesExperiment(distribution, size)
>>> sample = experiment.generate()

Create a sample suitable to estimate first, total order and second order Sobol' indices:

>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> formula = ['sin(pi_*X1)+7*sin(pi_*X2)^2+0.1*(pi_*X3)^4*sin(pi_*X1)']
>>> model = ot.SymbolicFunction(['X1', 'X2', 'X3'], formula)
>>> distribution = ot.JointDistribution([ot.Uniform(-1.0, 1.0)] * 3)
>>> size = 10
>>> computeSecondOrder = True
>>> experiment = ot.SobolIndicesExperiment(distribution, size, computeSecondOrder)
>>> sample = experiment.generate()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolIndicesExperiment::getWeightedExperiment
"Experiment accessor.

Returns
-------
experiment : :class:`~openturns.WeightedExperiment`
    The internal experiment."

