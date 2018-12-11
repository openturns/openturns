import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

mySample = ot.Dirichlet([1.0, 2.0, 3.0]).getSample(100)
myOrderStatCop = ot.EmpiricalBernsteinCopula(mySample, 4)
myOrderStatCop.setDescription(['$u_1$', '$u_2$'])
graphPDF = myOrderStatCop.drawPDF()
graphCDF = myOrderStatCop.drawCDF()


fig = plt.figure(figsize=(8, 4))
pdf_axis = fig.add_subplot(121)
cdf_axis = fig.add_subplot(122)
pdf_axis.set_xlim(auto=True)
cdf_axis.set_xlim(auto=True)

View(graphPDF, figure=fig, axes=[pdf_axis], add_legend=True)
View(graphCDF, figure=fig, axes=[cdf_axis], add_legend=True)
fig.suptitle("EmpiricalBernsteinCopula: pdf and cdf")
