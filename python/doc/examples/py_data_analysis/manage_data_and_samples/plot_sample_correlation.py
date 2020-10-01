"""
Correlation analysis on samples
===============================
"""

# %%
# In this example we are going to estimate the correlation between an output sample Y and the corresponding inputs using various estimators:
#
# - Pearson coefficients
# - Spearman coefficients
# - PCC: Partial Correlation Coefficients
# - PRCC: Partial Rank Correlation Coefficient
# - SRC: Standard Regression Coefficients
# - SRRC: Standard Rank Regression Coefficient

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# To illustrate the usage of the method mentionned above, we define a set of X/Y data using the ususal `Ishigami` use-case.

# %%
# Create X/Y data
ot.RandomGenerator.SetSeed(0)
formula = ['X3+sin(pi_*X1)+7*sin(X2)*sin(pi_*X2)+' + \
           '1.2*((pi_*X3)*(pi_*X2))*sin(pi_*X1)']
input_names = ['X1', 'X2', 'X3']
model = ot.SymbolicFunction(input_names, formula)
distribution = ot.ComposedDistribution([ot.Uniform(-1.0, 1.0)] * 3, \
                                        ot.IndependentCopula(3))
size = 100
inputDesign = ot.SobolIndicesExperiment(distribution, size, True).generate()
outputDesign = model(inputDesign)

# %%
# PCC coefficients
# ------------------
# We compute here `PCC` coefficients using the `CorrelationAnalysis`

# %%
pcc_indices = ot.CorrelationAnalysis.PCC(inputDesign, outputDesign)
print(pcc_indices)

# %%
#  

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(pcc_indices, input_names, "PCC coefficients")
view = viewer.View(graph)

# %%
# PRCC coefficients
# --------------------
# We compute here `PRCC` coefficients using the `CorrelationAnalysis`

# %%
prcc_indices = ot.CorrelationAnalysis.PRCC(inputDesign, outputDesign)
print(prcc_indices)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(prcc_indices, input_names, "PRCC coefficients")
view = viewer.View(graph)

# %%
# SRC coefficients
# -------------------
# We compute here `SRC` coefficients using the `CorrelationAnalysis`

# %%
src_indices = ot.CorrelationAnalysis.SRC(inputDesign, outputDesign)
print(src_indices)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(src_indices, input_names, 'SRC coefficients')
view = viewer.View(graph)

# %%
# Case where coefficients sum to 1 : 

# %%
scale_src_indices = ot.CorrelationAnalysis.SRC(inputDesign, outputDesign, True)
print(scale_src_indices)

# %%
# And its associated graph:

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(scale_src_indices, input_names, 'Scaled SRC coefficients')
view = viewer.View(graph)

# %%
# Finally, using signed src: we get the trend importance :

# %%
signed_src_indices = ot.CorrelationAnalysis.SignedSRC(inputDesign, outputDesign)
print(signed_src_indices)

# %%
# and its graph :

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(signed_src_indices, input_names, 'Signed SRC coefficients')
view = viewer.View(graph)

# %%
#

# %%
# SRRC coefficients
# --------------------
# We compute here `SRRC` coefficients using the `CorrelationAnalysis`

# %%
srrc_indices = ot.CorrelationAnalysis.SRRC(inputDesign, outputDesign)
print(srrc_indices)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(srrc_indices, input_names, 'SRRC coefficients')
view = viewer.View(graph)

# %%
# Pearson coefficients
# -----------------------
# We compute here the Pearson :math:`\rho` coefficients using the `CorrelationAnalysis`

# %%
pearson_correlation = ot.CorrelationAnalysis.PearsonCorrelation(inputDesign, outputDesign)
print(pearson_correlation)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(pearson_correlation,
                                                     input_names,
                                                     "Pearson correlation coefficients")
view = viewer.View(graph)

# %%
# Spearman coefficients
# -----------------------
# We compute here the Pearson :math:`\rho_s` coefficients using the `CorrelationAnalysis`

# %%
spearman_correlation = ot.CorrelationAnalysis.SpearmanCorrelation(inputDesign, outputDesign)
print(spearman_correlation)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(spearman_correlation, 
                                                     input_names, 
                                                     "Spearman correlation coefficients")
view = viewer.View(graph)
plt.show()
