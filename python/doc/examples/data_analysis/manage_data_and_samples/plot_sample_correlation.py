"""
Estimate correlation coefficients
=================================
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
from openturns.usecases import ishigami_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# To illustrate the usage of the method mentioned above, we define a set of X/Y data using the :ref:`Ishigami model <use-case-ishigami>`. This classical model is defined in a data class :
im = ishigami_function.IshigamiModel()

# %%
# Create X/Y data
# We get the input variables description :
input_names = im.distributionX.getDescription()

size = 100
inputDesign = ot.SobolIndicesExperiment(
    im.distributionX, size, True).generate()
outputDesign = im.model(inputDesign)

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
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    pcc_indices, input_names, "PCC coefficients")
view = viewer.View(graph)

# %%
# PRCC coefficients
# --------------------
# We compute here `PRCC` coefficients using the `CorrelationAnalysis`

# %%
prcc_indices = ot.CorrelationAnalysis.PRCC(inputDesign, outputDesign)
print(prcc_indices)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    prcc_indices, input_names, "PRCC coefficients")
view = viewer.View(graph)

# %%
# SRC coefficients
# -------------------
# We compute here `SRC` coefficients using the `CorrelationAnalysis`

# %%
src_indices = ot.CorrelationAnalysis.SRC(inputDesign, outputDesign)
print(src_indices)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    src_indices, input_names, 'SRC coefficients')
view = viewer.View(graph)

# %%
# Case where coefficients sum to 1 :

# %%
scale_src_indices = ot.CorrelationAnalysis.SRC(inputDesign, outputDesign, True)
print(scale_src_indices)

# %%
# And its associated graph:

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    scale_src_indices, input_names, 'Scaled SRC coefficients')
view = viewer.View(graph)

# %%
# Finally, using signed src: we get the trend importance :

# %%
signed_src_indices = ot.CorrelationAnalysis.SignedSRC(
    inputDesign, outputDesign)
print(signed_src_indices)

# %%
# and its graph :

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    signed_src_indices, input_names, 'Signed SRC coefficients')
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
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    srrc_indices, input_names, 'SRRC coefficients')
view = viewer.View(graph)

# %%
# Pearson coefficients
# -----------------------
# We compute here the Pearson :math:`\rho` coefficients using the `CorrelationAnalysis`

# %%
pearson_correlation = ot.CorrelationAnalysis.PearsonCorrelation(
    inputDesign, outputDesign)
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
spearman_correlation = ot.CorrelationAnalysis.SpearmanCorrelation(
    inputDesign, outputDesign)
print(spearman_correlation)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(spearman_correlation,
                                                             input_names,
                                                             "Spearman correlation coefficients")
view = viewer.View(graph)
plt.show()
