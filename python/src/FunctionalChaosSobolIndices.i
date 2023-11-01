// SWIG file FunctionalChaosSobolIndices.i

%{
#include "openturns/FunctionalChaosSobolIndices.hxx"
%}

%include FunctionalChaosSobolIndices_doc.i

%include openturns/FunctionalChaosSobolIndices.hxx
namespace OT { %extend FunctionalChaosSobolIndices { FunctionalChaosSobolIndices(const FunctionalChaosSobolIndices & other) { return new OT::FunctionalChaosSobolIndices(other); } } }

%pythoncode %{
def __FunctionalChaosSobolIndices_repr_html(self):
    """Get HTML representation."""
    chaosResult = self.getFunctionalChaosResult()
    inputDimension = chaosResult.getDistribution().getDimension()
    outputDimension = chaosResult.getMetaModel().getOutputDimension()
    enumerateFunction = chaosResult.getOrthogonalBasis().getEnumerateFunction()
    indices = chaosResult.getIndices()
    basisSize = indices.getSize()
    coefficients = chaosResult.getCoefficients()
    inputDescription = chaosResult.getDistribution().getDescription()
    sobolIndexFormat = openturns.ResourceMap.GetAsString("FunctionalChaosSobolIndices-PrintFormat")
    partOfVarianceThreshold = openturns.ResourceMap.GetAsScalar("FunctionalChaosSobolIndices-VariancePartThreshold")
    maximumNumberOfOutput = openturns.ResourceMap.GetAsUnsignedInteger("FunctionalChaosSobolIndices-MaximumNumberOfOutput")
    
    # compute the mean, the variance, the standard deviation
    randomVector = openturns.FunctionalChaosRandomVector(chaosResult)
    mean = randomVector.getMean()
    covarianceMatrix = randomVector.getCovariance()
    stdDev = openturns.Point(outputDimension)
    for i in range(outputDimension):
        stdDev[i] = covarianceMatrix[i, i] ** 0.5

    # quick summary
    basisSize = chaosResult.getReducedBasis().getSize()
    html = ""
    html += f"{self.getClassName()}\n"
    html += "<ul>\n"
    html += f"  <li>input dimension: {inputDimension}</li>\n"
    html += f"  <li>output dimension: {outputDimension}</li>\n"
    html += f"  <li>basis size: {basisSize}</li>\n"
    html += f"  <li>mean: {mean}</li>\n"
    html += f"  <li>std-dev: {stdDev}</li>\n"
    html += "</ul>\n"
    
    for i in range(outputDimension):
        if i > maximumNumberOfOutput:
            break
        if outputDimension > 1:
            html += f"Output marginal: {i}\n"

        # Table of Sobol' indices
        html += "<table>\n"
        # Header
        html += "  <tr>\n"
        html += "    <th>Input</th>\n"
        html += "    <th>Variable</th>\n"
        html += "    <th>Sobol' index</th>\n"
        html += "    <th>Total index</th>\n"
        html += "  </tr>\n"
        # Content
        for j in range(inputDimension):
            html += "  <tr>\n"
            html += f"    <td>{j}</td>\n"
            html += f"    <td>{inputDescription[j]}</td>\n"
            sobolIndex = self.getSobolIndex(j, i)
            html += "    <td>" + sobolIndexFormat.format(sobolIndex) + "</td>\n"
            sobolIndex = self.getSobolTotalIndex(j, i)
            html += "    <td>" + sobolIndexFormat.format(sobolIndex) + "</td>\n"
            html += "  </tr>\n"
            
        html += "</table>\n"

        # Compute the part of variance of each multi-index
        partOfVariance = self.getPartOfVariance(i)
        # Sort marginal variances in decreasing order
        marginalPartOfVarianceSample = openturns.Sample.BuildFromPoint(partOfVariance)
        order = marginalPartOfVarianceSample.argsort(False)

        # Table of part of variances indices
        html += "<table>\n"
        # Header
        html += "  <tr>\n"
        html += "    <th>Index</th>\n"
        html += "    <th>Multi-index</th>\n"
        html += "    <th>Part of variance</th>\n"
        html += "  </tr>\n"
        # Content
        for k in range(basisSize):
            sortedIndex = order[k]
            globalIndex = indices[sortedIndex]
            multiIndex = enumerateFunction(globalIndex)
            partOfVarianceIndex = marginalPartOfVarianceSample[sortedIndex]
            if partOfVarianceIndex[0] > partOfVarianceThreshold:
                html += "  <tr>\n"
                html += f"    <td>{sortedIndex}</td>\n"
                html += f"    <td>{multiIndex}</td>\n"
                html += "    <td>" + sobolIndexFormat.format(partOfVarianceIndex[0]) + "</td>\n"
                html += "  </tr>\n"

        html += "</table>\n"

    return html

FunctionalChaosSobolIndices._repr_html_ = __FunctionalChaosSobolIndices_repr_html
%}
