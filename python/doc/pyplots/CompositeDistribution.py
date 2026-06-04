import openturns as ot
from matplotlib import pyplot as plt
import openturns.viewer as otv


distX = ot.Normal(0.0, 10.0)
myFunc = ot.SymbolicFunction(["x"], ["x+sin(x)"])
distFin = ot.CompositeDistribution(myFunc, distX)

graphPDF = distFin.drawPDF(1024)
graphPDF.setXTitle("x")
graphPDF.setLegendPosition("")
graphCDF = distFin.drawCDF(1024)
graphCDF.setXTitle("x")
graphCDF.setLegendPosition("")


fig = plt.figure(figsize=(8, 4))

pdf_axis = fig.add_subplot(121)
cdf_axis = fig.add_subplot(122)
pdf_axis.set_xlim(auto=True)
cdf_axis.set_xlim(auto=True)

otv.View(graphPDF, figure=fig, axes=[pdf_axis], add_legend=True)
otv.View(graphCDF, figure=fig, axes=[cdf_axis], add_legend=True)
fig.suptitle("CompositeDistribution: f(x)=x+sin(x); L=Normal(0.0, 10.0): pdf and cdf")
