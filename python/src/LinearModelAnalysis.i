// SWIG file LinearModelAnalysis.i

%{
#include "openturns/LinearModelAnalysis.hxx"
%}

%include LinearModelAnalysis_doc.i

%copyctor OT::LinearModelAnalysis;

%include openturns/LinearModelAnalysis.hxx


%pythoncode %{
import math

def __LinearModelAnalysis_repr_html(self):
    """Get HTML representation."""
    def format_pvalue(pValue):
        fmtLargePValue = openturns.common.ResourceMap.GetAsString("LinearModelAnalysis-LargePValueFormat")
        fmtSmallPValue = openturns.common.ResourceMap.GetAsString("LinearModelAnalysis-SmallPValueFormat")
        if pValue == 0.0:
            pValueStr = "0.0"
        elif pValue > 1.e-3:
            pValueStr = fmtLargePValue.format(pValue)
        else:
            pValueStr = fmtSmallPValue.format(pValue)
        return pValueStr
        
    html = ""
    lmResult = self.getLinearModelResult()
    coefficients = lmResult.getCoefficients()
    basisSize = coefficients.getSize()
    hasIntercept = lmResult.hasIntercept()
    standardErrors = lmResult.getCoefficientsStandardErrors()
    tscores = self.getCoefficientsTScores()
    pValues = self.getCoefficientsPValues()
    names = lmResult.getCoefficientsNames()
    dof = lmResult.getDegreesOfFreedom()
    n = lmResult.getSampleResiduals().getSize()
    maximumNameLength = openturns.common.ResourceMap.GetAsUnsignedInteger("LinearModelAnalysis-PrintEllipsisThreshold")

    # Print the basis
    html += "<strong>Basis</strong>\n"
    html += "<br>\n"
    html += f"{lmResult.getFormula()}"
    html += "<br>\n"

    # Table of coefficients
    html += "<strong>Coefficients</strong>\n"
    html += "<br>\n"
    # Header
    html += "<table>\n"
    html += "  <tr>\n"
    html += f"    <th>Index</th>\n"
    html += f"    <th>Function</th>\n"
    html += f"    <th>Estimate</th>\n"
    html += f"    <th>Std Error</th>\n"
    html += f"    <th>t value</th>\n"
    html += f"    <th>Pr(>|t|)</th>\n"
    html += "  </tr>\n"
    # Content
    for i in range(pValues.getSize()):
        if len(names[i]) > maximumNameLength:
            fullName = names[i]
            printedName = fullName[0:maximumNameLength] + "..."
        else:
            printedName = names[i]
        html += "  <tr>\n"
        html += f"    <td>{i}</td>\n"
        html += f"    <td>{printedName}</td>\n"
        html += f"    <td>{coefficients[i]:.4e}</td>\n"
        html += f"    <td>{standardErrors[i]:.4e}</td>\n"
        html += f"    <td>{tscores[i]:.4e}</td>\n"
        html += f"    <td>{format_pvalue(pValues[i])}</td>\n"
        html += "  </tr>\n"

    html += "</table>\n"
    html += "<br>\n"

    # Print statistics
    html += "<ul>\n"
    stdError = self.getResidualsStandardError()
    html += f"  <li>Residual standard error: {stdError:.4e} " \
            f"on {dof} degrees of freedom </li>\n"

    # In case of only intercept in the basis, no more print
    if (basisSize == 1 and hasIntercept):
        html += "</ul>\n"
        return html

    fisherScore = self.getFisherScore()
    fisherPValue = self.getFisherPValue()
    html += f"  <li>F-statistic: {fisherScore:.4e}, "  \
            f"p-value: {format_pvalue(fisherPValue)}\n";

    #  R-squared & Adjusted R-squared tests
    test1 = lmResult.getRSquared();
    html += f"  <li>Multiple R-squared: {test1:.4f}</li>\n"
    test2 = lmResult.getAdjustedRSquared();
    html += f"  <li>Adjusted R-squared:  {test2:.4f}</li>\n"
    html += "</ul>\n"

    # Normality test of the residuals
    html += "<strong>Normality test of the residuals</strong>\n"
    html += "<br>\n"
    normalitytest1 = self.getNormalityTestResultAndersonDarling().getPValue()
    normalitytest2 = self.getNormalityTestResultChiSquared().getPValue()
    normalitytest3 = self.getNormalityTestResultKolmogorovSmirnov().getPValue()
    normalitytest4 = self.getNormalityTestCramerVonMises().getPValue()
    html += "<table>\n"
    html += "  <tr>\n"
    html += f"    <th>Normality test</th>\n"
    html += f"    <th>p-value</th>\n"
    html += "  </tr>\n"
    html += "  <tr>\n"
    html += f"    <td>Anderson-Darling</td>\n"
    html += f"    <td>{format_pvalue(normalitytest1)}</td>\n"
    html += "  </tr>\n"
    html += "  <tr>\n"
    html += f"    <td>ChiSquared</td>\n"
    html += f"    <td>{format_pvalue(normalitytest2)}</td>\n"
    html += "  </tr>\n"
    html += "  <tr>\n"
    html += f"    <td>Kolmogorov-Smirnov</td>\n"
    html += f"    <td>{format_pvalue(normalitytest3)}</td>\n"
    html += "  </tr>\n"
    html += "  <tr>\n"
    html += f"    <td>Cramer-Von Mises</td>\n"
    html += f"    <td>{format_pvalue(normalitytest4)}</td>\n"
    html += "  </tr>\n"
    html += "</table>\n"
    return html

LinearModelAnalysis._repr_html_ = __LinearModelAnalysis_repr_html
%}
