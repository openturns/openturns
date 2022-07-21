%feature("docstring") OT::KarhunenLoeveReduction
"Perform the reduction of a field.

This object projects a field on the Karhunen-Loeve basis by computing
the coefficients, lifts the coefficients, combines them with the
value of the modes on the mesh which creates the reduced field.

Parameters
----------
result : :class:`~openturns.KarhunenLoeveResult`
    Decomposition result.
trend : :class:`~openturns.TrendTransform`, optional
    Process trend, useful when the basis built using the covariance function
    from the space of trajectories is not well suited to approximate the mean
    function of the underlying process.

See also
--------
KarhunenLoeveProjection, KarhunenLoeveLifting

Examples
--------
Create a KL decomposition of a Gaussian process:

>>> import openturns as ot
>>> numberOfVertices = 10
>>> interval = ot.Interval(-1.0, 1.0)
>>> mesh = ot.IntervalMesher([numberOfVertices - 1]).build(interval)
>>> covariance = ot.SquaredExponential()
>>> process = ot.GaussianProcess(covariance, mesh)
>>> sampleSize = 10
>>> sample = process.getSample(sampleSize)
>>> threshold = 0.0
>>> algo = ot.KarhunenLoeveSVDAlgorithm(sample, threshold)
>>> algo.run()
>>> klresult = algo.getResult()

Generate some trajectories and reduce them:

>>> sample2 = process.getSample(5)
>>> reduction = ot.KarhunenLoeveReduction(klresult)
>>> reduced = reduction(sample2)

Same, but into account the trend:

>>> trend = ot.TrendTransform(ot.P1LagrangeEvaluation(sample.computeMean()), mesh)
>>> reduction = ot.KarhunenLoeveReduction(klresult, trend)
>>> reduced = reduction(sample2)"

// ---------------------------------------------------------------------

%feature("docstring") OT::KarhunenLoeveReduction::setTrend
"Trend accessor.

Parameters
----------
trend : :class:`~openturns.TrendTransform`, optional
    Process trend, useful when the basis built using the covariance function
    from the space of trajectories is not well suited to approximate the mean
    function of the underlying process."
