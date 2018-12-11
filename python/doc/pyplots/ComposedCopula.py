import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

myColl = [ot.ClaytonCopula(0.3), ot.NormalCopula(3)]
myMergedCop = ot.ComposedCopula(myColl)
myMergedCop.setDescription(['$u_1$', '$u_2$', '$u_3$', '$u_4$', '$u_5$'])
graphPDF = myMergedCop.drawMarginal2DPDF(0, 1, [0.0] * 2, [1.0] * 2, [100] * 2)
graphPDF.setXTitle('$u_1$')
graphPDF.setYTitle('$u_2$')
graphCDF = myMergedCop.drawMarginal2DCDF(0, 1, [0.0] * 2, [1.0] * 2, [100] * 2)
graphCDF.setXTitle('$u_1$')
graphCDF.setYTitle('$u_2$')


fig = plt.figure(figsize=(10, 4))
pdf_axis = fig.add_subplot(121)
cdf_axis = fig.add_subplot(122)
pdf_axis.set_xlim(auto=True)
cdf_axis.set_xlim(auto=True)

View(graphPDF, figure=fig, axes=[pdf_axis], add_legend=True)
View(graphCDF, figure=fig, axes=[cdf_axis], add_legend=True)
fig.suptitle("ComposedCopula(Clayton(0.3), NormalCopula(3)): pdf and cdf")
