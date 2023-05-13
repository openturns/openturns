// SWIG file IntegrationStrategy.i

%{
#include "openturns/IntegrationStrategy.hxx"
%}

%include IntegrationStrategy_doc.i

%include openturns/IntegrationStrategy.hxx
namespace OT{ %extend IntegrationStrategy { IntegrationStrategy(const IntegrationStrategy & other) { return new OT::IntegrationStrategy(other); } } }

%pythoncode %{
def __IntegrationStrategy_repr_html(self):
    """Get HTML representation."""
    html = openturns.ProjectionStrategy(self)._repr_html_()
    return html

IntegrationStrategy._repr_html_ = __IntegrationStrategy_repr_html
%}
