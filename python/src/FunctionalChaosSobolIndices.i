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
    html = ""
    html += "<ul>\n"
    html += "  <li>bla</li>\n" 
    html += "  <li>bla</li>\n" 
    html += "</ul>\n"
    return html

FunctionalChaosSobolIndices._repr_html_ = __FunctionalChaosSobolIndices_repr_html
%}
