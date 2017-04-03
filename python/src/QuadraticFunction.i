// SWIG file QuadraticFunction.i

%{
#include "openturns/QuadraticFunction.hxx"
%}

%include QuadraticFunction_doc.i

%include openturns/QuadraticFunction.hxx
namespace OT { %extend QuadraticFunction { QuadraticFunction(const QuadraticFunction & other) { return new OT::QuadraticFunction(other); } } }

%pythoncode %{
# deprecated
class QuadraticFunction(QuadraticFunction):
    def __init__(self, *args):
        super(QuadraticFunction, self).__init__(*args)
        openturns.common.Log.Warn('class QuadraticFunction is deprecated in favor of QuadraticFunction')
%}
