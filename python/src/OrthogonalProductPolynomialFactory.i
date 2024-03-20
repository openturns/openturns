// SWIG file OrthogonalProductPolynomialFactory.i

%{
#include "openturns/OrthogonalProductPolynomialFactory.hxx"
%}

%include OrthogonalProductPolynomialFactory_doc.i

%copyctor OT::OrthogonalProductPolynomialFactory;

%include openturns/OrthogonalProductPolynomialFactory.hxx

%pythoncode %{
def __OrthogonalProductPolynomialFactory_repr_html(self):
    """Get HTML representation."""
    polynomialCollection = self.getPolynomialFamilyCollection()
    dimension = len(polynomialCollection)
    enumerateFunction = self.getEnumerateFunction()
    distribution = self.getMeasure()
    description = distribution.getDescription()
    html = ""
    html += "<ul>\n"
    html += f"  <li>dimension: {dimension}</li>\n"
    html += f"  <li>enumerate function: {enumerateFunction}</li>\n"
    html += "</ul>\n"
    html += "\n"
    # Table of marginals
    html += "<table>\n"
    # Header
    html += "  <tr>\n"
    html += "    <th>Index</th>\n"
    html += "    <th>Name</th>\n"
    html += "    <th>Distribution</th>\n"
    html += "    <th>Univariate polynomial</th>\n"
    html += "  </tr>\n"
    # Content
    for i in range(dimension):
        univariatePolynomial = polynomialCollection[i]
        univariateImplementation = univariatePolynomial.getImplementation()
        html += "  <tr>\n"
        html += f"    <td>{i}</td>\n"
        html += f"    <td>{description[i]}</td>\n"
        marginalName = distribution.getMarginal(i).getImplementation().getClassName()
        html += f"    <td>{marginalName}</td>\n"
        try:
            hasSpecificFamily = univariateImplementation.getHasSpecificFamily()
            if hasSpecificFamily:
                marginalPolynomial = univariateImplementation.getSpecificFamily()
                polynomialImplementation = marginalPolynomial.getImplementation()
                className = polynomialImplementation.getClassName()
            else:
                orthoAlgorithm = univariateImplementation.getOrthonormalizationAlgorithm()
                orthoImplementation = orthoAlgorithm.getImplementation()
                className = orthoImplementation.getClassName()
        except:
            className = univariateImplementation.getClassName()
        html += f"    <td>{className}</td>\n"
        html += "  </tr>\n"
    html += "</table>\n"

    return html

OrthogonalProductPolynomialFactory._repr_html_ = __OrthogonalProductPolynomialFactory_repr_html
%}
