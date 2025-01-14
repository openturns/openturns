import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

from math import pi
ot.RandomGenerator.SetSeed(1)
log_UnscaledPDF = ot.SymbolicFunction('x', 'log(cos(x)) + x')

rangePDF = ot.Interval(-pi / 2.0, pi / 2.0)
ratioAlgo = otexp.RatioOfUniforms(log_UnscaledPDF, rangePDF)

unscaled_PDF = ot.ComposedFunction(ot.SymbolicFunction('x', 'exp(x)'), log_UnscaledPDF)
norm_fact = ot.GaussLegendre().integrate(unscaled_PDF, rangePDF)[0]
exact_PDF = ot.LinearCombinationFunction([unscaled_PDF], [1.0 / norm_fact])

size = 100000
sample = ratioAlgo.getSample(size)

ks_algo = ot.KernelSmoothing()
ks_algo.setBoundaryCorrection(True)
ks_algo.setLowerBound(-pi / 2.0)
ks_algo.setUpperBound(pi / 2.0)
ks_pdf = ks_algo.build(sample)

g = ks_pdf.drawPDF()
draw = exact_PDF.draw(-pi / 2.0, pi / 2.0, 1001).getDrawable(0)
draw.setLineWidth(2)
g.add(draw)
g.setLegends(['Ratio of Uniforms', 'exact pdf'])
g.setLegendPosition('topleft')
g.setXTitle('x')

view = otv.View(g)
