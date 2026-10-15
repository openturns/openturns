import openturns as ot
import openturns.viewer as otv

X = ot.RandomVector(ot.Normal())
sample = X.getSample(200)
histogram = ot.HistogramFactory().build(sample).drawPDF()
histogram.setTitle("Sample from a RandomVector")
otv.View(histogram, figure_kw={"figsize": (6, 4)}, add_legend=False)
