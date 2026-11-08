%feature("docstring") OT::PostAnalyticalQuantile
R"RAW(FORM/SORM quantile estimator.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    PostAnalyticalQuantile(*output, alpha=0.95, beta=0.95*)

    PostAnalyticalQuantile(*nearestPointAlgorithm, output, alpha=0.95, beta=0.95*)

The algorithm estimates the quantile of a scalar output
:math:`Y=g(\mathbf X)` from the analytical FORM or SORM probability.
For a candidate threshold :math:`t`, the event
:math:`\{g(\mathbf X)>t\}` is analysed in the standard space, the design
point and the Hasofer reliability index are computed, and the analytical
probability is evaluated.  The quantile is the deterministic root of

.. math::

    P_{\mathrm{FORM/SORM}}(t)=1-\alpha .

The analytical probability is a smooth deterministic function of
:math:`t`, so the root is bracketed geometrically and bisected to
:meth:`getRootTolerance`.  The pure analytical quantile is labelled
``COVERAGE_NONE``: FORM and SORM are approximations, not sampling
guarantees, even when the limit state is linear.

When :meth:`setRefinementEnabled` is ``True``, the analytical quantile is
refined by importance sampling around the design point.  Each of the
:meth:`getReplicateCount` replicates draws an independent sample, solves
the weighted empirical root with that fixed sample, and the result is
the mean of the replicate estimates with a Student interval.  The refined
path may be labelled ``COVERAGE_ASYMPTOTIC`` when at least two replicates
are available.

Parameters
----------
output : :class:`~openturns.RandomVector`
    Scalar output random vector whose quantile is estimated.
nearestPointAlgorithm : :class:`~openturns.OptimizationAlgorithm`, optional
    Optimization algorithm used to search the design point.  Default is
    :class:`~openturns.AbdoRackwitz` with the input distribution mean as
    starting point.
alpha : float, optional
    Quantile level, in :math:`(0, 1)`.  Default is 0.95.
beta : float, optional
    Confidence level of the interval, in :math:`(0, 1)`.  Default is
    0.95.

Notes
-----
The result returned by :meth:`run` stores the analytical probability, the
design point, the reliability index, the curvature correction (for SORM),
the refined probability, the refinement standard error, the root bracket,
the root iteration count and the number of probability evaluations.

The following :class:`~openturns.ResourceMap` keys are used:

- ``PostAnalyticalQuantile-DefaultReplicateCount`` (``UnsignedInteger``, default: ``30``): default number of importance-sampling replicates used by the refinement.
- ``QuantileRootSolver-DefaultTolerance`` (``Scalar``, default: ``1.0e-4``): default relative root-finding tolerance.
- ``QuantileRootSolver-DefaultMaximumIteration`` (``UnsignedInteger``, default: ``100``): default maximum number of bisection iterations.
- ``QuantileRootSolver-PilotSampleSize`` (``UnsignedInteger``, default: ``1000``): default number of pilot output values used to start and scale the bracket.
- ``QuantileRootSolver-DefaultBracketExpansion`` (``UnsignedInteger``, default: ``40``): maximum number of geometric bracket expansions.
- ``QuantileSimulation-DefaultAlpha`` (``Scalar``, default: ``0.95``): default quantile level.
- ``QuantileSimulation-DefaultConfidenceLevel`` (``Scalar``, default: ``0.95``): default nominal confidence level.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> output = ot.CompositeRandomVector(ot.SymbolicFunction(["x"], ["x"]),
...                                   ot.RandomVector(ot.Normal()))
>>> algo = otexp.PostAnalyticalQuantile(output, 0.95, 0.95)
>>> algo.run()
>>> result = algo.getResult()
>>> estimate = result.getQuantileEstimate()
>>> designPoint = result.getDesignPoint()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::getNearestPointAlgorithm
R"RAW(Accessor to the nearest-point algorithm.

Returns
-------
nearestPointAlgorithm : :class:`~openturns.OptimizationAlgorithm`
    Optimization algorithm used to search the design point.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::setNearestPointAlgorithm
R"RAW(Accessor to the nearest-point algorithm.

Parameters
----------
nearestPointAlgorithm : :class:`~openturns.OptimizationAlgorithm`
    Optimization algorithm used to search the design point.  The
    starting point is set to the input distribution mean when the
    algorithm does not define one.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::getAnalyticalMethod
R"RAW(Accessor to the analytical method.

Returns
-------
method : int
    Either ``FORM_APPROXIMATION`` or ``SORM_APPROXIMATION``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::setAnalyticalMethod
R"RAW(Accessor to the analytical method.

Parameters
----------
method : int
    Either ``FORM_APPROXIMATION`` or ``SORM_APPROXIMATION``.  Default is
    ``FORM_APPROXIMATION``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::getAnalyticalMethodDescription
R"RAW(Accessor to the analytical method description.

Returns
-------
description : str
    ``"FORM"`` or ``"SORM"``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::getSORMFormula
R"RAW(Accessor to the SORM curvature formula.

Returns
-------
formula : int
    One of ``BREITUNG``, ``HOHENBICHLER`` or ``TVEDT``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::setSORMFormula
R"RAW(Accessor to the SORM curvature formula.

Parameters
----------
formula : int
    One of ``BREITUNG``, ``HOHENBICHLER`` or ``TVEDT``.  Default is
    ``HOHENBICHLER``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::getSORMFormulaDescription
R"RAW(Accessor to the SORM curvature formula description.

Returns
-------
description : str
    ``"Breitung"``, ``"Hohenbichler"`` or ``"Tvedt"``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::getRefinementEnabled
R"RAW(Accessor to the importance-sampling refinement flag.

Returns
-------
refinementEnabled : bool
    Whether the analytical quantile is refined by importance sampling
    around the design point.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::setRefinementEnabled
R"RAW(Accessor to the importance-sampling refinement flag.

Parameters
----------
refinementEnabled : bool
    Whether the analytical quantile is refined by importance sampling
    around the design point.  Default is ``False``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::getReplicateCount
R"RAW(Accessor to the refinement replicate count.

Returns
-------
replicateCount : int
    Number of independent importance-sampling replicates used by the
    refinement.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::setReplicateCount
R"RAW(Accessor to the refinement replicate count.

Parameters
----------
replicateCount : int
    Number of independent importance-sampling replicates used by the
    refinement.  It must be positive; at least two replicates are
    required to build the Student interval.  Default is
    ``PostAnalyticalQuantile-DefaultReplicateCount``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::getRootTolerance
R"RAW(Accessor to the root-finding tolerance.

Returns
-------
tolerance : float
    Relative width of the final bracket.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::setRootTolerance
R"RAW(Accessor to the root-finding tolerance.

Parameters
----------
tolerance : float
    Relative width of the final bracket.  It must be finite and
    positive.  Default is ``QuantileRootSolver-DefaultTolerance``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::getMaximumRootIteration
R"RAW(Accessor to the maximum number of root-finding iterations.

Returns
-------
maximumIteration : int
    Maximum number of bisection iterations.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::setMaximumRootIteration
R"RAW(Accessor to the maximum number of root-finding iterations.

Parameters
----------
maximumIteration : int
    Maximum number of bisection iterations.  It must be positive.
    Default is ``QuantileRootSolver-DefaultMaximumIteration``.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::getPilotSampleSize
R"RAW(Accessor to the pilot sample size.

Returns
-------
pilotSampleSize : int
    Number of output values used to build the initial bracket.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PostAnalyticalQuantile::setPilotSampleSize
R"RAW(Accessor to the pilot sample size.

Parameters
----------
pilotSampleSize : int
    Number of output values used to build the initial bracket.  It must
    be positive.  Default is ``QuantileRootSolver-PilotSampleSize``.)RAW"
