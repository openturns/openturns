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
    html += f"  <li>dimension: {dimension}</li>\n"
    html += f"  <li>copula: {copula.getName()}</li>\n"
    html += "</ul>\n"
    html += "\n"
    # Table of marginals
    html += "<table>\n"
    # Header
    html += "  <tr>\n"
    html += "    <th>Index</th>\n"
    html += "    <th>Variable</th>\n"
    html += "    <th>Distribution</th>\n"
    html += "  </tr>\n"
    # Content
    for i in range(dimension):
        html += "  <tr>\n"
        html += f"    <td>{i}</td>\n"
        html += f"    <td>{description[i]}</td>\n"
        html += f"    <td>{collection[i]}</td>\n"
        html += "  </tr>\n"
    html += "</table>\n"

    return html

ComposedDistribution._repr_html_ = __ComposedDistribution_repr_html

def __ComposedDistribution_repr_markdown(self):
    """Get Markdown representation."""
    return self.__repr_markdown__()

ComposedDistribution._repr_markdown_ = __ComposedDistribution_repr_markdown
%}
