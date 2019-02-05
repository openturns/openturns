import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View


mu = [2.0, 2.0]
sigma = [1.0, 1.0]
R = ot.CorrelationMatrix(2)
R[0, 1] = 0.8
myNormal1 = ot.Normal(mu, sigma, R)
R2 = ot.CorrelationMatrix(2)
R2[0, 1] = -0.8
mu2 = [3.0, 3.0]
myNormal2 = ot.Normal(mu2, sigma, R2)

myMixture = ot.Mixture([myNormal1, myNormal2], [0.3, 0.7])
myMixture.setDescription(['$x_1$', '$x_2$'])

graphPDF = myMixture.drawPDF()
graphCDF = myMixture.drawCDF()

fig = plt.figure(figsize=(8, 4))
pdf_axis = fig.add_subplot(121)
cdf_axis = fig.add_subplot(122)
pdf_axis.set_xlim(auto=True)
cdf_axis.set_xlim(auto=True)

View(graphPDF, figure=fig, axes=[pdf_axis], add_legend=True)
View(graphCDF, figure=fig, axes=[cdf_axis], add_legend=True)
fig.suptitle("Mixture: 0.3*Normal1 + 0.7*Normal2: pdf and cdf")
