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
    coefficients = self.getCoefficients()
    basis = self.getOrthogonalBasis()
    enumerateFunction = basis.getEnumerateFunction()
    indices = self.getIndices()
    nbCoeffs = indices.getSize()
    # Table of attributes
    inputDimension = self.getMetaModel().getInputDimension()
    outputDimension = self.getMetaModel().getOutputDimension()
    basisSize = self.getIndices().getSize()
    relativeErrors = self.getRelativeErrors()
    residuals = self.getResiduals()
    # Header
    html += "<ul>\n"
    html += "  <li>Input dimension : %d</li>\n" % (inputDimension)
    html += "  <li>Output dimension : %d</li>\n" % (outputDimension)
    html += "  <li>Basis size : %d</li>\n" % (basisSize)
    html += "  <li>Relative errors : %s</li>\n" % (relativeErrors)
    html += "  <li>Residuals : %s</li>\n" % (residuals)
    html += "</ul>\n"
    # Table of coefficients
    html += "<table>\n"
    # Header
    html += "<tr>\n"
    html += "  <th>Index</th>\n"
    html += "  <th>Global index</th>\n"
    html += "  <th>Multi-index</th>\n"
    html += "  <th>Coefficient</th>\n"
    html += "</tr>\n"
    # Content
    for k in range(nbCoeffs):
        absoluteIndex = indices[k]
        multiindex = enumerateFunction(absoluteIndex)
        html += "<tr>\n"
        html += "  <td>%d</td>\n" % (k)
        html += "  <td>%d</td>\n" % (absoluteIndex)
        html += "  <td>%s</td>\n" % (multiindex)
        html += "  <td>%s</td>\n" % (coefficients[k])
        html += "</tr>\n"
    html += "</table>\n"

    return html

FunctionalChaosResult._repr_html_ = __FunctionalChaosResult_repr_html
%}
