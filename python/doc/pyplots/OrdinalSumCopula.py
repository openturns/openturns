import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View


myCop1 = ot.GumbelCopula(2)
myCop2 = ot.NormalCopula(2)
alpha = 0.3
myOrdSumCop = ot.OrdinalSumCopula([myCop1, myCop2], [alpha])
myOrdSumCop.setDescription(['$u_1$', '$u_2$'])
graphPDF = myOrdSumCop.drawPDF()
graphCDF = myOrdSumCop.drawCDF()


fig = plt.figure(figsize=(8, 4))
pdf_axis = fig.add_subplot(121)
cdf_axis = fig.add_subplot(122)
pdf_axis.set_xlim(auto=True)
cdf_axis.set_xlim(auto=True)

View(graphPDF, figure=fig, axes=[pdf_axis], add_legend=True)
View(graphCDF, figure=fig, axes=[cdf_axis], add_legend=True)
fig.suptitle("Ordinal Sum of Copulas: Gumbel(2) and Normal(2): pdf and cdf")
