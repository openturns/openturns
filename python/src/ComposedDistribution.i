// SWIG file ComposedDistribution.i

%{
#include "openturns/ComposedDistribution.hxx"
%}

%include ComposedDistribution_doc.i

%include openturns/ComposedDistribution.hxx
namespace OT { %extend ComposedDistribution { ComposedDistribution(const ComposedDistribution & other) { return new OT::ComposedDistribution(other); } } }

%pythoncode %{
def __ComposedDistribution_repr_html(self):
    """Get HTML representation."""
    dimension = self.getDimension()
    copula = self.getCopula()
    description = self.getDescription()
    collection = self.getDistributionCollection()
    html = ""
    html += "<ul>\n"
    html += "  <li>Dimension : %d</li>\n" % (dimension)
    html += "  <li>Copula : %s</li>\n" % (copula.getName())
    html += "</ul>\n"
    html += "\n"
    # Table of marginals
    html += "<table>\n"
    # Header
    html += "<tr>\n"
    html += "  <th>Index</th>\n"
    html += "  <th>Variable</th>\n"
    html += "  <th>Distribution</th>\n"
    html += "</tr>\n"
    # Content
    for i in range(dimension):
        html += "<tr>\n"
        html += "  <td>%d</td>\n" % (i)
        html += "  <td>%s</td>\n" % (description[i])
        html += "  <td>%s</td>\n" % (collection[i])
        html += "</tr>\n"
    html += "</table>\n"

    return html

ComposedDistribution._repr_html_ = __ComposedDistribution_repr_html
%}
