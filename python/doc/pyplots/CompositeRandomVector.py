import openturns as ot
from openturns.viewer import View

X = ot.RandomVector(ot.Normal())
f = ot.SymbolicFunction(['x'], ['x^2*sin(x)'])
Y = ot.CompositeRandomVector(f, X)
sample = Y.getSample(200)
histogram = ot.HistogramFactory().build(sample).drawPDF()
histogram.setTitle('Y=x^2*sin(x)')
View(histogram, figure_kw={'figsize': (6, 4)}, add_legend=False)
