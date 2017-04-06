// SWIG file QuadraticFunction.i

%{
#include "openturns/QuadraticFunction.hxx"
%}

%include QuadraticFunction_doc.i

%include openturns/QuadraticFunction.hxx
namespace OT { %extend QuadraticFunction { QuadraticFunction(const QuadraticFunction & other) { return new OT::QuadraticFunction(other); } } }

%pythoncode %{
# deprecated
class QuadraticNumericalMathFunction(QuadraticFunction):
    def __init__(self, *args):
        super(QuadraticNumericalMathFunction, self).__init__(*args)
        openturns.common.Log.Warn('class QuadraticNumericalMathFunction is deprecated in favor of QuadraticFunction')
%}
