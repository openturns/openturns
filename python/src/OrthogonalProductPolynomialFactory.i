// SWIG file OrthogonalProductPolynomialFactory.i

%{
#include "openturns/OrthogonalProductPolynomialFactory.hxx"
%}

%include OrthogonalProductPolynomialFactory_doc.i

%include openturns/OrthogonalProductPolynomialFactory.hxx
namespace OT{ %extend OrthogonalProductPolynomialFactory { OrthogonalProductPolynomialFactory(const OrthogonalProductPolynomialFactory & other) { return new OT::OrthogonalProductPolynomialFactory(other); } } }

%pythoncode %{
def __OrthogonalProductPolynomialFactory_repr_html(self):
    """Get HTML representation."""
    collection = self.getPolynomialFamilyCollection()
    dimension = len(collection)
    enumerateFunction = self.getEnumerateFunction()
    html = ""
    html += "<ul>\n"
    html += "  <li>Dimension : %d</li>\n" % (dimension)
    html += "  <li>Enumerate function : %s</li>\n" % (enumerateFunction)
    html += "</ul>\n"
    html += "\n"
    # Table of marginals
    html += "<table>\n"
    # Header
    html += "<tr>\n"
    html += "  <th>Index</th>\n"
    html += "  <th>Univariate polynomial</th>\n"
    html += "</tr>\n"
    # Content
    for i in range(dimension):
        univariatePolynomial = collection[i]
        html += "<tr>\n"
        html += "  <td>%d</td>\n" % (i)
        html += "  <td>%s</td>\n" % (univariatePolynomial)
        html += "</tr>\n"
    html += "</table>\n"

    return html

OrthogonalProductPolynomialFactory._repr_html_ = __OrthogonalProductPolynomialFactory_repr_html
%}
