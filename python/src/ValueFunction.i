// SWIG file ValueFunction.i

%{
#include "openturns/ValueFunction.hxx"
%}

%include ValueFunction_doc.i

%include openturns/ValueFunction.hxx
namespace OT { %extend ValueFunction { ValueFunction(const ValueFunction & other) { return new OT::ValueFunction(other); } } }

%pythoncode %{
# deprecated
class SpatialFunction(ValueFunction):
    def __init__(self, *args):
        super(SpatialFunction, self).__init__(*args)
        openturns.common.Log.Warn('class SpatialFunction is deprecated in favor of ValueFunction')
%}

