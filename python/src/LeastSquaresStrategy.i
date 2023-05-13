// SWIG file LeastSquaresStrategy.i

%{
#include "openturns/LeastSquaresStrategy.hxx"
%}

%include LeastSquaresStrategy_doc.i

%include openturns/LeastSquaresStrategy.hxx
namespace OT{ %extend LeastSquaresStrategy { LeastSquaresStrategy(const LeastSquaresStrategy & other) { return new OT::LeastSquaresStrategy(other); } } }

%pythoncode %{
def __LeastSquaresStrategy_repr_html(self):
    """Get HTML representation."""
    html = openturns.ProjectionStrategy(self)._repr_html_()
    return html

LeastSquaresStrategy._repr_html_ = __LeastSquaresStrategy_repr_html
%}

