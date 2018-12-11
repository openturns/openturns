import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

myCop = ot.ExtremeValueCopula(ot.SymbolicFunction("t", "t^3/2-t/2+1"))
myCop.setDescription(['$u_1$', '$u_2$'])
graphPDF = myCop.drawPDF()
graphCDF = myCop.drawCDF()


fig = plt.figure(figsize=(8, 4))
pdf_axis = fig.add_subplot(121)
cdf_axis = fig.add_subplot(122)
pdf_axis.set_xlim(auto=True)
cdf_axis.set_xlim(auto=True)

View(graphPDF, figure=fig, axes=[pdf_axis], add_legend=True)
View(graphCDF, figure=fig, axes=[cdf_axis], add_legend=True)
fig.suptitle("ExtremeValueCopula: pdf and cdf")
