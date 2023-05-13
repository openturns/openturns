// SWIG file DualLinearCombinationFunction.i

%{
#include "openturns/DualLinearCombinationFunction.hxx"
%}

%include DualLinearCombinationFunction_doc.i

%include openturns/DualLinearCombinationFunction.hxx
namespace OT { %extend DualLinearCombinationFunction { DualLinearCombinationFunction(const DualLinearCombinationFunction & other) { return new OT::DualLinearCombinationFunction(other); } } }

%pythoncode %{
def __DualLinearCombinationFunction_repr_html(self):
    """Get HTML representation."""
    return self.__repr_html__()

DualLinearCombinationFunction._repr_html_ = __DualLinearCombinationFunction_repr_html
%}
