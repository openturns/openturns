// SWIG file LinearFunction.i

%{
#include "openturns/LinearFunction.hxx"
%}

%include LinearFunction_doc.i

%include openturns/LinearFunction.hxx
namespace OT { %extend LinearFunction { LinearFunction(const LinearFunction & other) { return new OT::LinearFunction(other); } } }

%pythoncode %{
# deprecated
class LinearFunction(LinearFunction):
    def __init__(self, *args):
        super(LinearFunction, self).__init__(*args)
        openturns.common.Log.Warn('class LinearFunction is deprecated in favor of LinearFunction')
%}
