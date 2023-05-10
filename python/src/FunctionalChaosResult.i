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
    html += "<ul>\n"
    html += "  <li>bla</li>\n" 
    html += "  <li>bla</li>\n" 
    html += "</ul>\n"
    return html

FunctionalChaosResult._repr_html_ = __FunctionalChaosResult_repr_html
%}
