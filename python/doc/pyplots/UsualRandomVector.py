import openturns as ot
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)
ot.ResourceMap.Reload()

X = ot.RandomVector(ot.Normal())
sample = X.getSample(200)
histogram = ot.HistogramFactory().build(sample).drawPDF()
histogram.setTitle("X~N(0,1)")
otv.View(histogram, figure_kw={"figsize": (6, 4)}, add_legend=False)
