// SWIG file FunctionalChaosResult.i

%{
#include "openturns/FunctionalChaosResult.hxx"
%}

%include FunctionalChaosResult_doc.i

%include openturns/FunctionalChaosResult.hxx

namespace OT{ %extend FunctionalChaosResult { FunctionalChaosResult(const FunctionalChaosResult & other) { return new OT::FunctionalChaosResult(other); } } }


%pythoncode %{
def __FunctionalChaosResult_repr_html(self):
    """Get HTML representation."""
    html = ""
    html += f"{self.getClassName()}\n"
    coefficients = self.getCoefficients()
    basis = self.getOrthogonalBasis()
    enumerateFunction = basis.getEnumerateFunction()
    indices = self.getIndices()
    indicesSize = indices.getSize()
    # Table of attributes
    inputDimension = self.getMetaModel().getInputDimension()
    outputDimension = self.getMetaModel().getOutputDimension()
    relativeErrors = self.getRelativeErrors()
    residuals = self.getResiduals()
    transformation = self.getTransformation()
    inverseTransformation = self.getInverseTransformation()
    orthogonalBasis = self.getOrthogonalBasis()
    distribution = self.getDistribution()
    fmt = openturns.common.ResourceMap.GetAsString("FunctionalChaosResult-PrintFormat")
    ell_threshold = openturns.common.ResourceMap.GetAsUnsignedInteger("FunctionalChaosResult-PrintEllipsisThreshold")
    ell_size = openturns.common.ResourceMap.GetAsUnsignedInteger("FunctionalChaosResult-PrintEllipsisSize")
    ellipsis = indicesSize * outputDimension > ell_threshold
    # Header
    html += "<ul>\n"
    html += f"  <li>input dimension: {inputDimension}</li>\n"
    html += f"  <li>output dimension: {outputDimension}</li>\n"
    html += f"  <li>distribution dimension: {distribution.getDimension()}</li>\n"
    html += f"  <li>transformation: {transformation.getInputDimension()} -> {transformation.getOutputDimension()}</li>\n"
    html += f"  <li>inverse transformation: {inverseTransformation.getInputDimension()} -> {inverseTransformation.getOutputDimension()}</li>\n"
    html += f"  <li>orthogonal basis dimension: {orthogonalBasis.getMeasure().getDimension()}</li>\n"
    html += f"  <li>indices size: {indicesSize}</li>\n"
    html += f"  <li>relative errors: {relativeErrors}</li>\n"
    html += f"  <li>residuals: {residuals}</li>\n"
    html += "</ul>\n"
    # Table of coefficients
    html += "<table>\n"
    # Header
    html += "  <tr>\n"
    html += "    <th>Index</th>\n"
    html += "    <th>Multi-index</th>\n"
    isEllipsisEnabled = (ellipsis and outputDimension > 2 * ell_size)
    if isEllipsisEnabled:
        actualNumberOfColumns = 3 + 2 * ell_size
    else:
        actualNumberOfColumns = 2 + outputDimension
    if isEllipsisEnabled:
        for j in range(ell_size):
            html += f"    <th>Coeff.#{j}</th>\n"
        html += "    <th>...</th>\n"
        for j in range(outputDimension - ell_size, outputDimension):
            html += f"    <th>Coeff.#{j}</th>\n"
    else:
        if outputDimension == 1:
            html += "    <th>Coeff.</th>\n"
        else:
            for j in range(outputDimension):
                html += f"    <th>Coeff.#{j}</th>\n"
    html += "  </tr>\n"
    # Content
    for i in range(indicesSize):
        if ellipsis and indicesSize > 2 * ell_size:
            if i == ell_size:
                html += "  <tr>\n"
                html += f'    <td colspan="{actualNumberOfColumns}">...</td>\n'
                html += "  </tr>\n" 
                continue
            else:
                if i > ell_size and i < indicesSize - ell_size:
                    continue
        globalIndex = indices[i]
        multiindex = enumerateFunction(globalIndex)
        html += "  <tr>\n"
        html += f"    <th>{i}</th>\n"
        html += f"    <td>{multiindex}</td>\n"
        if isEllipsisEnabled:
            for j in range(ell_size):
                html += "    <td>" + fmt.format(coefficients[i, j]) + "</td>\n"
            html += "    <td>...</td>\n"
            for j in range(outputDimension - ell_size, outputDimension):
                html += "    <td>" + fmt.format(coefficients[i, j]) + "</td>\n"
        else:
            for j in range(outputDimension):
                html += "    <td>" + fmt.format(coefficients[i, j]) + "</td>\n"
        html += "  </tr>\n"
    html += "</table>\n"

    return html

FunctionalChaosResult._repr_html_ = __FunctionalChaosResult_repr_html
%}
