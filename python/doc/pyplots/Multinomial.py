import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

myDist = ot.Multinomial(15, [0.3, 0.4])
myDist.setDescription(['$x_1$', '$x_2$'])
graphPDF = myDist.drawPDF()
graphCDF = myDist.drawCDF()


fig = plt.figure(figsize=(8, 4))
pdf_axis = fig.add_subplot(121)
cdf_axis = fig.add_subplot(122)
pdf_axis.set_xlim(auto=True)
cdf_axis.set_xlim(auto=True)

View(graphPDF, figure=fig, axes=[pdf_axis], add_legend=True)
View(graphCDF, figure=fig, axes=[cdf_axis], add_legend=True)
fig.suptitle("Multinomial Distribution: pdf and cdf")
