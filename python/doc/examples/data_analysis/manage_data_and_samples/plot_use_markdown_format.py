"""
Use Markdown to format results
==============================
"""
# %%
#
# In this example, we use the Markdown format to pretty-print
# results from an uncertainty quantification study.
# We first show how the Markdown function from the IPython module
# works.
# Then we show how this can be used within a uncertainty quantification
# study.
#
from IPython.display import Markdown
import openturns as ot
from openturns.usecases import stressed_beam
from IPython.display import display, HTML

# %%
# Use the Markdown function
# ~~~~~~~~~~~~~~~~~~~~~~~~~
#
# In this section, we show how to use the Markdown function to
# format a table.
#

# %%
markdown = ""
# Itemized list
markdown += "- item\n"
markdown += "- item\n"
markdown += "\n"
# Table
markdown += "| Index | Value |\n"
markdown += "|-------|-------|\n"
n = 5
for i in range(n):
    markdown += "| %5s | %5s |\n" % (i, 1 + i)
markdown += "\n"
print(markdown)

# %%
# Pretty-print
display(Markdown(markdown))

# %%
# Format a reliability study
# ~~~~~~~~~~~~~~~~~~~~~~~~~~
# In this section, we format the results of a reliability study.
# Before doing this, we must see that the library already provides
# pretty-print features which can be accessed through various
# objects.
# The most simple pretty-print feature is given by the
# :class:`~openturns.Sample`.
data = ot.Sample(5, 3)
data

# %%
# This can be convenient to format any result that can be
# stored within a `Sample`, i.e. any data set that
# has the shape of an array of floats.
# There are, however, situations where this framework is not
# general enough.
# For example, we may want to print strings, integers,
# lists of strings or list of integers (e.g. :class:`~openturns.Indices`)
# and these data types cannot be stored in a `Sample`.
# In the next example, we want to print an interval: this cannot
# be done easily using the `Sample` class.
# We could, of course, print the lower and upper bounds
# into two different columns of the array, but this is less
# straightforward to understand as an interval.

# %%
# The next function performs a reliability study based on the
# stressed beam.
# Given the number of outer loops, the maximum coefficient of variation
# and the block size, the function returns the result of the probability
# simulation algorithm.
# The function uses a Monte-Carlo experiment.


def computeReliabilityResult(maximumOuterSamplingSize, maximumCoV, blockSize=1):
    sm = stressed_beam.AxialStressedBeam()
    limitStateFunction = sm.model
    inputRandomVector = ot.RandomVector(sm.distribution)
    outputRandomVector = ot.CompositeRandomVector(limitStateFunction, inputRandomVector)
    myEvent = ot.ThresholdEvent(outputRandomVector, ot.Less(), 0.0)
    experiment = ot.MonteCarloExperiment()
    algoMC = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
    algoMC.setMaximumOuterSampling(maximumOuterSamplingSize)
    algoMC.setBlockSize(blockSize)
    algoMC.setMaximumCoefficientOfVariation(maximumCoV)
    algoMC.run()
    psaResult = algoMC.getResult()
    return psaResult


# %%
# Use the function.

maximumOuterSamplingSize = 100
maximumCoV = 0.0
psaResult = computeReliabilityResult(maximumOuterSamplingSize, maximumCoV)
psaResult

# %%
# We increase the sample size and compute the
# probability estimate and the corresponding confidence interval.
# The output of this algorithm is a `markdown` string which contains
# the results inside a Markdown table.

sampleSizeList = []
numberOfRepetitions = 10
sampleSizeFactor = 2
alpha = 0.05
maximumOuterSamplingSize = 2
markdown = ""
markdown += "| n | Pf | %.0f%%  C.I. |\n" % (100 * (1.0 - alpha))
markdown += "|---|----|--------------|\n"
for i in range(numberOfRepetitions):
    maximumOuterSamplingSize *= sampleSizeFactor
    psaResult = computeReliabilityResult(maximumOuterSamplingSize, maximumCoV)
    probability = psaResult.getProbabilityEstimate()
    pflen = psaResult.getConfidenceLength(1 - alpha)
    pfLower = probability - pflen / 2
    pfUpper = probability + pflen / 2
    markdown += "| %5d | %.3e | [%10.3e, %10.3e] |\n" % (
        maximumOuterSamplingSize,
        probability,
        pfLower,
        pfUpper,
    )
print(markdown)

# %%
# Pretty-print
display(Markdown(markdown))

# %%
# HTML
sampleSizeList = []
numberOfRepetitions = 10
sampleSizeFactor = 2
alpha = 0.05
maximumOuterSamplingSize = 2
html = "<table>\n"
html += "  <tr>\n"
html += "    <th> n </th>\n"
html += "    <th> Pf </th>\n"
html += "    <th> %.0f%%  C.I. </th>\n" % (100 * (1.0 - alpha))
html += "  </tr>\n"
for i in range(numberOfRepetitions):
    maximumOuterSamplingSize *= sampleSizeFactor
    psaResult = computeReliabilityResult(maximumOuterSamplingSize, maximumCoV)
    probability = psaResult.getProbabilityEstimate()
    pflen = psaResult.getConfidenceLength(1 - alpha)
    pfLower = probability - pflen / 2
    pfUpper = probability + pflen / 2
    html += "  <tr>\n"
    html += f"    <td> {maximumOuterSamplingSize} </td>\n"
    html += f"    <td> {probability:.3e} </td>\n"
    html += f"    <td> [{pfLower:10.3e}, {pfUpper:10.3e}] </td>\n"
    html += "  <tr>\n"
html += "</table>\n"
print(html)

# %%
# Pretty-print
HTML(html)
