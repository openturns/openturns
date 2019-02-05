import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

myDist = [ot.Beta(1.5, 3.2, 0.0, 1.0),  ot.Beta(2.0, 4.3, 0.5, 1.2)]
myOrderStatDist = ot.MaximumEntropyOrderStatisticsDistribution(myDist)
myOrderStatDist.setDescription(['$x_1$', '$x_2$'])
graphPDF = myOrderStatDist.drawPDF()
graphCDF = myOrderStatDist.drawCDF()


fig = plt.figure(figsize=(8, 4))
pdf_axis = fig.add_subplot(121)
cdf_axis = fig.add_subplot(122)
pdf_axis.set_xlim(auto=True)
cdf_axis.set_xlim(auto=True)

View(graphPDF, figure=fig, axes=[pdf_axis], add_legend=True)
View(graphCDF, figure=fig, axes=[cdf_axis], add_legend=True)
fig.suptitle("Max Entropy Order Statistics Distribution: pdf and cdf")
