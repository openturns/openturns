import openturns as ot
import openturns.viewer as otv


sample = ot.Normal(3).getSample(100)
distribution = ot.JointDistribution(
    [ot.HistogramFactory().build(sample.getMarginal(i)) for i in range(3)]
)
graph = ot.VisualTest.DrawPairsMarginals(sample, distribution)

otv.View(graph, figure_kw={"figsize": (6.0, 6.0)})
