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
from openturns.usecases import ishigami_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# To illustrate the usage of the method mentioned above, we define a set of X/Y data using the :ref:`Ishigami model <use-case-ishigami>`.
# This classical model is defined in a data class:
im = ishigami_function.IshigamiModel()

# %%
# Create X/Y data
# We get the input variables description :
input_names = im.distributionX.getDescription()

size = 100
inputDesign = ot.SobolIndicesExperiment(im.distributionX, size, True).generate()
outputDesign = im.model(inputDesign)

# %%
# Create a :class:`~openturns.CorrelationAnalysis` object to compute various estimates
# of the correlation between the inputs and the output.

corr_analysis = ot.CorrelationAnalysis(inputDesign, outputDesign)

# %%
# PCC coefficients
# ------------------

pcc_indices = corr_analysis.computePCC()
print(pcc_indices)

# %%
#

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    pcc_indices, input_names, "PCC coefficients"
)
view = viewer.View(graph)

# %%
# PRCC coefficients
# --------------------

prcc_indices = corr_analysis.computePRCC()
print(prcc_indices)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    prcc_indices, input_names, "PRCC coefficients"
)
view = viewer.View(graph)

# %%
# SRC coefficients
# -------------------

src_indices = corr_analysis.computeSRC()
print(src_indices)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    src_indices, input_names, "SRC coefficients"
)
view = viewer.View(graph)

# %%
# Normalized squared SRC coefficients (coefficients are made to sum to 1) :

squared_src_indices = corr_analysis.computeSquaredSRC(True)
print(squared_src_indices)

# %%

graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    squared_src_indices, input_names, "Squared SRC coefficients"
)
view = viewer.View(graph)


# %%
# SRRC coefficients
# --------------------

srrc_indices = corr_analysis.computeSRRC()
print(srrc_indices)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    srrc_indices, input_names, "SRRC coefficients"
)
view = viewer.View(graph)

# %%
# Pearson coefficients
# -----------------------
# We compute here the Pearson :math:`\rho` coefficients.

pearson_correlation = corr_analysis.computeLinearCorrelation()
print(pearson_correlation)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    pearson_correlation, input_names, "Pearson correlation coefficients"
)
view = viewer.View(graph)

# %%
# Spearman coefficients
# -----------------------
# We compute here the Spearman :math:`\rho_s` coefficients.

# %%
spearman_correlation = corr_analysis.computeSpearmanCorrelation()
print(spearman_correlation)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    spearman_correlation, input_names, "Spearman correlation coefficients"
)
view = viewer.View(graph)
plt.show()
