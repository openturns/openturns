import openturns as ot
from openturns.viewer import View

X = ot.RandomVector(ot.Normal())
sample = X.getSample(200)
histogram = ot.HistogramFactory().build(sample).drawPDF()
histogram.setTitle('X~N(0,1)')
View(histogram, figure_kw={'figsize': (6, 4)}, add_legend=False)
