// SWIG file ComposedDistribution.i

%{
#include "openturns/ComposedDistribution.hxx"
%}

%include ComposedDistribution_doc.i

%include openturns/ComposedDistribution.hxx
namespace OT { %extend ComposedDistribution { ComposedDistribution(const ComposedDistribution & other) { return new OT::ComposedDistribution(other); } } }

%pythoncode %{
def __ComposedDistribution_repr_markdown(self):
    """Get Markdown representation."""
    return self.__repr_markdown__()

ComposedDistribution._repr_markdown_ = __ComposedDistribution_repr_markdown
%}
