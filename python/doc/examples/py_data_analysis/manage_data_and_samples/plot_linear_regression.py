"""
Build and validate a linear model
=================================
"""
# %%
# In this example we are going to build a linear regression model and validate it numerically and graphically.
#
# The linear model between links a scalar variable :math:`Y` and to an n-dimensional one :math:`\underline{X} = (X_i)_{i \leq n}`, as follows:
#
# .. math::
#    \tilde{Y} = a_0 + \sum_{i=1}^n a_i X_i + \varepsilon
#   
# where :math:`\varepsilon` is the residual, supposed to follow the Normal(0.0, 1.0) distribution.
#
# The linear model may be validated graphically if :math:`\underline{X}` is of dimension 1, by drawing on the same graph the cloud :math:`(X_i, Y_i)`.
#
# The linear model also be validate numerically with several tests:
#
# - LinearModelFisher: tests the nullity of the regression linear model coefficients (Fisher distribution used),
# - LinearModelResidualMean: tests, under the hypothesis of a gaussian sample, if the mean of the residual is equal to zero. It is based on the Student test (equality of mean for two gaussian samples).
#
#
# The hypothesis on the residuals (centered gaussian distribution) may be validated:
#
# - graphically if :math:`\underline{X}` is of dimension 1, by drawing the residual couples (:math:`\varepsilon_i, \varepsilon_{i+1}`), where the residual :math:`\varepsilon_i` is evaluated on the samples :math:`(X, Y)`.
# - numerically with the LinearModelResidualMean Test which tests, under the hypothesis of a gaussian sample, if the mean of the residual is equal to zero. It is based on the Student test (equality of mean for two gaussian samples).
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Generate X,Y samples
N = 1000
Xsample = ot.Triangular(1.0, 5.0, 10.0).getSample(N)
Ysample = Xsample * 3.0 + ot.Normal(0.5, 1.0).getSample(N)

# %%
# Generate a particular scalar sampleX
particularXSample = ot.Triangular(1.0, 5.0, 10.0).getSample(N)

# %%
# Create the linear model from Y,X samples
result = ot.LinearModelAlgorithm(Xsample, Ysample).getResult()

# Get the coefficients ai
print("coefficients of the linear regression model = ", result.getCoefficients())

# Get the confidence intervals of the ai coefficients
print("confidence intervals of the coefficients = ", ot.LinearModelAnalysis(result).getCoefficientsConfidenceInterval(0.9))


# %%
# Validate the model with a visual test
graph = ot.VisualTest.DrawLinearModel(Xsample, Ysample, result)
view = viewer.View(graph)

# %%
# Draw the graph of the residual values
graph = ot.VisualTest.DrawLinearModelResidual(Xsample, Ysample, result)
view = viewer.View(graph)

# %%
# Check the nullity of the regression linear model coefficients
resultLinearModelFisher = ot.LinearModelTest.LinearModelFisher(Xsample, Ysample, 
                                                               result, 0.10)
print("Test Success ? ", resultLinearModelFisher.getBinaryQualityMeasure())
print("p-value of the LinearModelFisher Test = ", resultLinearModelFisher.getPValue())
print("p-value threshold = ", resultLinearModelFisher.getThreshold())

# %%
# Check, under the hypothesis of a gaussian sample, if the mean of the residual is equal to zero
resultLinearModelResidualMean = ot.LinearModelTest.LinearModelResidualMean(Xsample, Ysample, 
                                                                           result, 0.10)
print("Test Success ? ", resultLinearModelResidualMean.getBinaryQualityMeasure())
print("p-value of the LinearModelResidualMean Test = ", resultLinearModelResidualMean.getPValue())
print("p-value threshold = ", resultLinearModelResidualMean.getThreshold())
if not viewer._noshow:
    plt.show()
